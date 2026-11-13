#include "esp32s3.h"

WebServer server(80);

volatile int  volumeRichiesto = -1;
volatile bool audioInRiproduzione = false;
uint8_t volumeCorrente = VOLUME_DEFAULT;

// ==========================================
// STATO CONDIVISO TRA LOOP E TASK AI
// ==========================================
static SemaphoreHandle_t mutexStato = nullptr;
static TaskHandle_t taskAI = nullptr;
static volatile bool aiOccupata = false;

static String domandaRichiesta;
static bool richiestaAutomatica = false;

static String ultimoTesto, ultimoErrore, ultimaDomanda, testoDaVocalizzare;
static uint32_t ultimaSeq = 0, ultimaDurataMs = 0;
static uint8_t *ultimaFoto = nullptr;
static size_t ultimaFotoLen = 0;

struct Blocco {
    Blocco() { xSemaphoreTake(mutexStato, portMAX_DELAY); }
    ~Blocco() { xSemaphoreGive(mutexStato); }
};

static const char PROMPT_SISTEMA[] =
    "Sei AdmAIre, l'assistente vocale integrato negli occhiali di una persona non vedente. "
    "L'immagine mostra ciò che ha davanti, ripresa ad altezza occhi. "
    "Rispondi sempre in italiano con frasi brevi e naturali da ascoltare, senza markdown, elenchi o emoji. "
    "Indica le posizioni come davanti, a sinistra, a destra e la distanza approssimativa.";

static const char PROMPT_SCANSIONE[] =
    "Segnala in massimo 15 parole l'ostacolo o il pericolo più importante sul percorso "
    "(gradini, marciapiedi, buche, pali, veicoli, persone, porte) e dove si trova. "
    "Se c'è un cartello o un testo importante leggilo. "
    "Se il percorso è libero rispondi solo: Via libera.";

static void *allocaGrande(size_t dimensione) {
    void *p = ps_malloc(dimensione);
    return p ? p : malloc(dimensione);
}

// ==========================================
// TESTO E VOCE
// ==========================================
String pulisciTesto(const String &testo) {
    String s = testo;
    int fineRagionamento = s.indexOf("</think>");
    if (fineRagionamento >= 0) s = s.substring(fineRagionamento + 8);
    for (const char *c : {"*", "#", "_", "`", "\""}) s.replace(c, "");
    s.replace("\n", " ");
    s.replace("\r", " ");
    while (s.indexOf("  ") >= 0) s.replace("  ", " ");
    s.trim();
    if (s.length() > MAX_CARATTERI_VOCE) {
        int taglio = s.lastIndexOf(' ', MAX_CARATTERI_VOCE);
        s = s.substring(0, taglio > 0 ? taglio : MAX_CARATTERI_VOCE);
    }
    return s;
}

void accodaVoce(const String &testo) {
    Blocco b;
    testoDaVocalizzare = testo;
}

bool prelevaTestoDaVocalizzare(String &out) {
    if (!mutexStato) return false;
    Blocco b;
    if (testoDaVocalizzare.isEmpty()) return false;
    out = testoDaVocalizzare;
    testoDaVocalizzare = "";
    return true;
}

static bool eViaLibera(String testo) {
    testo.toLowerCase();
    return testo.indexOf("via libera") >= 0;
}

// ==========================================
// FOTOCAMERA
// ==========================================
bool inizializzaXiaoCamera() {
    camera_config_t config = {};
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    // Groq conta ogni immagine come un numero fisso di token: la VGA migliora
    // il riconoscimento (testi, cartelli) senza costi aggiuntivi.
    bool psram = psramFound();
    config.frame_size = psram ? FRAMESIZE_VGA : FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
    config.fb_location = psram ? CAMERA_FB_IN_PSRAM : CAMERA_FB_IN_DRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    if (esp_camera_init(&config) != ESP_OK) return false;

    // Il modulo è montato capovolto: ribalta l'immagine (rotazione di 180°) direttamente
    // nel sensore, così il JPEG esce già corretto sia per l'AI sia per la pagina web.
    sensor_t *sensore = esp_camera_sensor_get();
    if (sensore) {
        sensore->set_vflip(sensore, 1);
    }
    return true;
}

static camera_fb_t *scattaFoto() {
    // Con un solo buffer il primo frame è quello rimasto in memoria: lo scartiamo
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) esp_camera_fb_return(fb);
    return esp_camera_fb_get();
}

static void salvaUltimaFoto(const camera_fb_t *fb) {
    uint8_t *copia = (uint8_t *)allocaGrande(fb->len);
    if (!copia) return;
    memcpy(copia, fb->buf, fb->len);
    Blocco b;
    free(ultimaFoto);
    ultimaFoto = copia;
    ultimaFotoLen = fb->len;
}

// ==========================================
// AI - GROQ
// ==========================================
// Il corpo della richiesta (JSON + immagine base64) viene costruito in un unico
// buffer in PSRAM, senza copie intermedie in String.
static char *costruisciCorpo(const camera_fb_t *fb, const String &domanda, size_t &lunghezza) {
    JsonDocument doc;
    doc["model"] = GROQ_MODEL;
    doc["reasoning_effort"] = "none";
    doc["temperature"] = 0.2;
    doc["max_completion_tokens"] = domanda.isEmpty() ? 60 : 160;
    JsonArray messaggi = doc["messages"].to<JsonArray>();
    JsonObject sistema = messaggi.add<JsonObject>();
    sistema["role"] = "system";
    sistema["content"] = PROMPT_SISTEMA;
    JsonObject utente = messaggi.add<JsonObject>();
    utente["role"] = "user";
    JsonArray contenuto = utente["content"].to<JsonArray>();
    JsonObject testo = contenuto.add<JsonObject>();
    testo["type"] = "text";
    testo["text"] = domanda.isEmpty() ? String(PROMPT_SCANSIONE)
                                      : "Domanda dell'utente: " + domanda + "\nRispondi in massimo 35 parole.";
    JsonObject immagine = contenuto.add<JsonObject>();
    immagine["type"] = "image_url";
    immagine["image_url"]["url"] = "@IMG@";

    String json;
    serializeJson(doc, json);
    int segnaposto = json.indexOf("@IMG@");

    static const char PREFISSO[] = "data:image/jpeg;base64,";
    size_t capienza = json.length() + sizeof(PREFISSO) + ((fb->len + 2) / 3) * 4 + 1;
    char *corpo = (char *)allocaGrande(capienza);
    if (!corpo) return nullptr;

    size_t pos = segnaposto;
    memcpy(corpo, json.c_str(), pos);
    memcpy(corpo + pos, PREFISSO, sizeof(PREFISSO) - 1);
    pos += sizeof(PREFISSO) - 1;
    size_t scritti = 0;
    mbedtls_base64_encode((unsigned char *)corpo + pos, capienza - pos, &scritti, fb->buf, fb->len);
    pos += scritti;
    const char *coda = json.c_str() + segnaposto + 5;
    size_t lenCoda = strlen(coda);
    memcpy(corpo + pos, coda, lenCoda);
    lunghezza = pos + lenCoda;
    return corpo;
}

static int inviaAGroq(uint8_t *corpo, size_t lunghezza, String &risposta) {
    // Client persistente: la connessione TLS resta aperta tra una richiesta e
    // l'altra, evitando ~1 s di handshake a ogni scansione.
    static WiFiClientSecure client;
    static HTTPClient http;
    static bool inizializzato = false;
    if (!inizializzato) {
        client.setInsecure();
        client.setHandshakeTimeout(10);
        http.setReuse(true);
        http.setConnectTimeout(8000);
        http.setTimeout(15000);
        inizializzato = true;
    }

    int codice = -1;
    for (int tentativo = 0; tentativo < 2 && codice < 0; tentativo++) {
        if (!http.begin(client, GROQ_URL)) continue;
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "Bearer " GROQ_API_KEY);
        codice = http.POST(corpo, lunghezza);
        if (codice > 0) {
            risposta = http.getString();
            http.end();
        } else {
            // Connessione riutilizzata scaduta: chiudi e riprova da zero
            Serial.printf("[AI] Tentativo %d fallito: %s\n", tentativo + 1, http.errorToString(codice).c_str());
            http.end();
            client.stop();
        }
    }
    return codice;
}

static String interpretaRisposta(int codice, const String &risposta, String &testo) {
    if (codice < 0) return "Connessione al servizio AI non riuscita";
    if (codice != 200) {
        Serial.printf("[AI] Errore HTTP %d: %s\n", codice, risposta.c_str());
        if (codice == 429) return "Troppe richieste, riprova tra qualche secondo";
        if (codice == 401 || codice == 403) return "Chiave API Groq non valida";
        return "Errore del servizio AI " + String(codice);
    }
    JsonDocument filtro;
    filtro["choices"][0]["message"]["content"] = true;
    JsonDocument doc;
    if (deserializeJson(doc, risposta, DeserializationOption::Filter(filtro))) return "Risposta AI non valida";
    testo = pulisciTesto(doc["choices"][0]["message"]["content"] | "");
    return testo.isEmpty() ? "Nessuna risposta dall'AI" : "";
}

static void eseguiAnalisi(const String &domanda, bool automatica) {
    uint32_t inizio = millis();
    String testo, errore;

    camera_fb_t *fb = scattaFoto();
    if (!fb) {
        errore = "Fotocamera non disponibile";
    } else {
        salvaUltimaFoto(fb);
        size_t lunghezza = 0;
        char *corpo = costruisciCorpo(fb, domanda, lunghezza);
        esp_camera_fb_return(fb);
        if (!corpo) {
            errore = "Memoria insufficiente";
        } else {
            String risposta;
            int codice = inviaAGroq((uint8_t *)corpo, lunghezza, risposta);
            free(corpo);
            errore = interpretaRisposta(codice, risposta, testo);
        }
    }

    uint32_t durata = millis() - inizio;
    Serial.printf("[AI] %lu ms -> %s\n", durata, errore.isEmpty() ? testo.c_str() : errore.c_str());

    Blocco b;
    ultimoTesto = testo;
    ultimoErrore = errore;
    ultimaDomanda = domanda;
    ultimaDurataMs = durata;
    ultimaSeq++;
    // In modalità automatica restiamo in silenzio se non c'è nulla da segnalare
    if (!errore.isEmpty()) {
        if (!automatica) testoDaVocalizzare = errore;
    } else if (!(automatica && eViaLibera(testo))) {
        testoDaVocalizzare = testo;
    }
}

static void taskAnalisi(void *) {
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        String domanda;
        bool automatica;
        {
            Blocco b;
            domanda = domandaRichiesta;
            automatica = richiestaAutomatica;
        }
        eseguiAnalisi(domanda, automatica);
        aiOccupata = false;
    }
}

void avviaTaskAI() {
    mutexStato = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(taskAnalisi, "analisiAI", 16384, nullptr, 1, &taskAI, 0);
}

static bool avviaAnalisi(const String &domanda, bool automatica) {
    if (aiOccupata || !taskAI) return false;
    {
        Blocco b;
        domandaRichiesta = domanda;
        richiestaAutomatica = automatica;
    }
    aiOccupata = true;
    xTaskNotifyGive(taskAI);
    return true;
}

// ==========================================
// BATTERIA
// ==========================================
// La XIAO ESP32S3 non espone lo stato del caricatore: senza PIN_USB_SENSE la
// carica viene stimata dall'andamento della tensione nel tempo.
static const uint8_t STORICO_N = 30;    // un campione ogni 10 s -> 5 minuti
static float storicoMv[STORICO_N];
static uint8_t storicoConteggio = 0, storicoIndice = 0;

static struct {
    float mv = 0;
    int pct = -1;
    bool inCarica = false;
    bool rilevata = false;
} batteria;

static float valoreStorico(uint8_t campioniFa) {
    return storicoMv[(storicoIndice + STORICO_N - 1 - campioniFa) % STORICO_N];
}

static float pendenzaMvAlMinuto() {
    float sx = 0, sy = 0, sxx = 0, sxy = 0;
    int n = storicoConteggio;
    for (int i = 0; i < n; i++) {
        float x = i / 6.0f;
        float y = valoreStorico(n - 1 - i);
        sx += x; sy += y; sxx += x * x; sxy += x * y;
    }
    return (n * sxy - sx * sy) / (n * sxx - sx * sx);
}

static void aggiornaStatoCarica() {
#ifdef PIN_USB_SENSE
    batteria.inCarica = analogReadMilliVolts(PIN_USB_SENSE) > 1500;
#else
    if (batteria.mv >= 4190) { batteria.inCarica = true; return; }
    if (storicoConteggio >= 4) {
        // Collegare/scollegare il cavo provoca un salto di tensione immediato
        float salto = batteria.mv - valoreStorico(3);
        if (salto > 35) { batteria.inCarica = true; return; }
        if (salto < -35) { batteria.inCarica = false; return; }
    }
    if (storicoConteggio >= 12) {
        float pendenza = pendenzaMvAlMinuto();
        if (pendenza > 0.6f) batteria.inCarica = true;
        else if (pendenza < -0.3f) batteria.inCarica = false;
    }
#endif
}

// Curva di scarica tipica di una cella LiPo (tensione a vuoto -> %)
static int percentualeDaTensione(float mv) {
    static const uint16_t curva[][2] = {
        {4200, 100}, {4150, 95}, {4110, 90}, {4080, 85}, {4020, 80}, {3980, 75}, {3950, 70},
        {3910, 65}, {3870, 60}, {3850, 55}, {3840, 50}, {3820, 45}, {3800, 40}, {3790, 35},
        {3770, 30}, {3750, 25}, {3730, 20}, {3710, 15}, {3690, 10}, {3610, 5}, {3500, 0}};
    const int n = sizeof(curva) / sizeof(curva[0]);
    if (mv >= curva[0][0]) return 100;
    if (mv <= curva[n - 1][0]) return 0;
    for (int i = 1; i < n; i++) {
        if (mv >= curva[i][0]) {
            float f = (mv - curva[i][0]) / (float)(curva[i - 1][0] - curva[i][0]);
            return lroundf(curva[i][1] + f * (curva[i - 1][1] - curva[i][1]));
        }
    }
    return 0;
}

static void avvisiBatteriaScarica() {
    static int ultimaSoglia = 100;
    if (batteria.inCarica) { ultimaSoglia = 100; return; }
    if (storicoConteggio < 3) return;
    for (int soglia : {15, 5}) {
        if (batteria.pct <= soglia && ultimaSoglia > soglia) {
            ultimaSoglia = soglia;
            accodaVoce(soglia == 5 ? "Batteria quasi scarica, collega il caricatore"
                                   : "Batteria al " + String(soglia) + " per cento");
        }
    }
}

void aggiornaBatteria() {
    static uint32_t ultimaLettura = 0, ultimoStorico = 0;
    uint32_t ora = millis();
    // Durante la riproduzione l'amplificatore fa calare la tensione: saltiamo la lettura
    if (ora - ultimaLettura < 1000 || audioInRiproduzione) return;
    ultimaLettura = ora;

    uint32_t somma = 0;
    for (int i = 0; i < 16; i++) somma += analogReadMilliVolts(PIN_BATTERIA);
    float mv = somma / 16.0f * BATT_PARTITORE * BATT_CALIBRAZIONE;

    batteria.rilevata = mv > 2800 && mv < 4500;
    if (!batteria.rilevata) {
        batteria.mv = 0;
        batteria.pct = -1;
        storicoConteggio = 0;
        return;
    }
    batteria.mv = batteria.mv == 0 ? mv : batteria.mv * 0.9f + mv * 0.1f;

    if (storicoConteggio == 0 || ora - ultimoStorico >= 10000) {
        ultimoStorico = ora;
        storicoMv[storicoIndice] = batteria.mv;
        storicoIndice = (storicoIndice + 1) % STORICO_N;
        if (storicoConteggio < STORICO_N) storicoConteggio++;
        aggiornaStatoCarica();
    }

    // Durante la carica la tensione misurata è più alta di quella reale della cella
    batteria.pct = percentualeDaTensione(batteria.mv + (batteria.inCarica ? -BATT_CADUTA_MV : BATT_CADUTA_MV));
    avvisiBatteriaScarica();
}

static String formattaDurata(float ore) {
    int h = (int)ore, m = (int)((ore - h) * 60);
    return h > 0 ? String(h) + " h " + String(m) + " min" : String(m) + " min";
}

static void riempiBatteria(JsonObject b) {
    b["detected"] = batteria.rilevata;
    b["pct"] = batteria.pct;
    b["mv"] = (int)batteria.mv;
    b["charging"] = batteria.inCarica;
#ifdef PIN_USB_SENSE
    b["estimated"] = false;
#else
    b["estimated"] = true;
#endif
    if (!batteria.rilevata) {
        b["time"] = "Batteria non rilevata";
    } else if (batteria.inCarica) {
        float ore = (100 - batteria.pct) / 100.0f * BATT_CAPACITA_MAH / BATT_CARICA_MA * 1.15f;
        b["time"] = batteria.pct >= 99 ? String("Carica completata") : "circa " + formattaDurata(ore) + " al termine";
    } else {
        float ore = batteria.pct / 100.0f * BATT_CAPACITA_MAH / BATT_CONSUMO_MA;
        b["time"] = batteria.pct <= 5 ? String("Batteria scarica") : "circa " + formattaDurata(ore) + " rimanenti";
    }
}

// ==========================================
// SERVER WEB
// ==========================================
static void inviaJson(int codice, JsonDocument &doc) {
    String out;
    serializeJson(doc, out);
    server.sendHeader("Cache-Control", "no-store");
    server.send(codice, "application/json", out);
}

static void gestisciRoot() {
    server.send_P(200, "text/html", PAGINA_HTML);
}

static void gestisciStato() {
    JsonDocument doc;
    doc["busy"] = (bool)aiOccupata;
    doc["speaking"] = (bool)audioInRiproduzione;
    doc["volume"] = volumeCorrente;
    doc["rssi"] = WiFi.RSSI();
    doc["uptime"] = millis() / 1000;
    doc["heap"] = ESP.getFreeHeap();
    doc["psram"] = ESP.getFreePsram();
    doc["model"] = GROQ_MODEL;
    {
        Blocco b;
        doc["seq"] = ultimaSeq;
        doc["text"] = ultimoTesto;
        doc["error"] = ultimoErrore;
        doc["question"] = ultimaDomanda;
        doc["ms"] = ultimaDurataMs;
        doc["photo"] = ultimaFoto != nullptr;
    }
    riempiBatteria(doc["battery"].to<JsonObject>());
    inviaJson(200, doc);
}

static void gestisciBatteria() {
    JsonDocument doc;
    riempiBatteria(doc.to<JsonObject>());
    inviaJson(200, doc);
}

static void gestisciAnalisi() {
    String domanda = server.arg("q");
    domanda.trim();
    bool avviata = avviaAnalisi(domanda.substring(0, 300), server.arg("auto") == "1");
    JsonDocument doc;
    doc["started"] = avviata;
    inviaJson(avviata ? 202 : 409, doc);
}

static void gestisciFoto() {
    Blocco b;
    if (!ultimaFoto) {
        server.send(404, "text/plain", "Nessuna foto disponibile");
        return;
    }
    server.sendHeader("Cache-Control", "no-store");
    server.send_P(200, "image/jpeg", (const char *)ultimaFoto, ultimaFotoLen);
}

static void gestisciRipeti() {
    {
        Blocco b;
        if (!ultimoTesto.isEmpty()) testoDaVocalizzare = ultimoTesto;
    }
    server.send(204);
}

static void gestisciVolume() {
    if (server.hasArg("v")) {
        volumeCorrente = constrain(server.arg("v").toInt(), 0, 21);
        volumeRichiesto = volumeCorrente;
    }
    JsonDocument doc;
    doc["volume"] = volumeCorrente;
    inviaJson(200, doc);
}

void configuraServerWeb() {
    server.on("/", HTTP_GET, gestisciRoot);
    server.on("/status", HTTP_GET, gestisciStato);
    server.on("/battery", HTTP_GET, gestisciBatteria);
    server.on("/analyze", HTTP_GET, gestisciAnalisi);
    server.on("/photo.jpg", HTTP_GET, gestisciFoto);
    server.on("/repeat", HTTP_GET, gestisciRipeti);
    server.on("/volume", HTTP_GET, gestisciVolume);
    server.onNotFound([]() { server.send(404, "text/plain", "Non trovato"); });
    server.enableCORS(true);
    server.begin();
}
