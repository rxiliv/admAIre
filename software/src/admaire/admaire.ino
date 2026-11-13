#include "Audio.h"
#include <ESPmDNS.h>

#include "esp32s3.h"

// ==========================================
// ISTANZE GLOBALI
// ==========================================
Audio audio;
String testoCorrenteAudio = ""; // Buffer protetto per la RAM

static void connettiWiFi() {
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("[Wi-Fi] Connessione a " WIFI_SSID "...");
    uint32_t inizio = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - inizio > 30000) {
            Serial.println("\n[Wi-Fi] Timeout, riavvio...");
            ESP.restart();
        }
        delay(250);
        Serial.print(".");
    }
    // Il risparmio energetico del Wi-Fi aggiunge centinaia di ms di latenza
    WiFi.setSleep(false);
    Serial.print("\n[OK] Connesso! Apri http://");
    Serial.print(WiFi.localIP());
    Serial.println(" oppure http://admaire.local");
}

static void callbackAudio(Audio::msg_t m) {
    if (m.e == Audio::evt_eof) audioInRiproduzione = false;
    if (m.msg) Serial.printf("[AUDIO] %s\n", m.msg);
}

void setup() {
    Serial.begin(115200);
#if ARDUINO_USB_CDC_ON_BOOT
    // Senza PC collegato la seriale USB non deve rallentare il programma
    Serial.setTxTimeoutMs(0);
#endif
    delay(1000);
    Serial.println("--- AVVIO SISTEMA ADMAIRE ---");

    Serial.printf("[PSRAM] dimensione totale: %u byte, libera: %u byte\n",
                  ESP.getPsramSize(), ESP.getFreePsram());
    if (ESP.getPsramSize() == 0) {
        Serial.println("[PSRAM] ATTENZIONE: nessuna PSRAM rilevata! Imposta Tools > PSRAM > OPI PSRAM in Arduino IDE.");
    }

    setCpuFrequencyMhz(160);

    if (!inizializzaXiaoCamera()) {
        Serial.println("[ERRORE CRITICO] Inizializzazione modulo Camera fallita!");
        while (true) { delay(1000); }
    }
    Serial.println("[OK] Interfaccia Camera configurata.");

    connettiWiFi();
    MDNS.begin("admaire");
    MDNS.addService("http", "tcp", 80);

    Audio::audio_info_callback = callbackAudio;
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(volumeCorrente);

    avviaTaskAI();
    configuraServerWeb();

    accodaVoce("Sistema pronto");
    Serial.println("[SISTEMA] Pronti all'uso.");
}

void loop() {
    audio.loop();
    server.handleClient();
    aggiornaBatteria();

    if (volumeRichiesto >= 0) {
        audio.setVolume(volumeRichiesto);
        volumeRichiesto = -1;
    }

    if (prelevaTestoDaVocalizzare(testoCorrenteAudio)) {
        Serial.printf("[AUDIO] Riproduzione: %s (heap libero %u)\n", testoCorrenteAudio.c_str(), ESP.getFreeHeap());
        if (audio.isRunning()) audio.stopSong();
        bool avviato = audio.connecttospeech(testoCorrenteAudio.c_str(), "it");
        if (!avviato) {
            delay(150);
            avviato = audio.connecttospeech(testoCorrenteAudio.c_str(), "it");
            Serial.printf("[AUDIO] Secondo tentativo: %s\n", avviato ? "OK" : "FALLITO");
        }
        audioInRiproduzione = avviato;
    }
    if (audioInRiproduzione && !audio.isRunning()) audioInRiproduzione = false;
}
