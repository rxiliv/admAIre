#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "Audio.h" 
#include "mbedtls/base64.h"

// ==========================================
// I TUOI DATI WI-FI ED API
// ==========================================
const char* ssid     = ""; 
const char* password = "";     
const char* openrouter_key = "";

// ==========================================
// PINOUT HARDWARE ADMAIRE
// ==========================================
#define I2S_BCLK      D1
#define I2S_LRC       D2
#define I2S_DOUT      D3
#define PIN_BATTERIA  D4

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39
#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13

// ==========================================
// ISTANZE GLOBALI
// ==========================================
Audio audio;
WebServer server(80);

String testoDaVocalizzare = ""; 
String testoCorrenteAudio = ""; // Buffer protetto per la RAM
unsigned long timestampRicezioneTesto = 0;

// Prototipi funzioni
bool inizializzaXiaoCamera();
void gestisciRoot();
void gestisciCattura();
void gestisciAnalisi();
void gestisciBatteria();
String pulisciTesto(String testoSporco);

// ============================================================================
// WEB APP - CONSOLE DISPOSITIVO
// ============================================================================
const char PAGINA_HTML[] PROGMEM = R"HTMLPAGE(<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
<title>AdmAIre — Console</title>
<style>
:root{
  --bg:#14161a;
  --surface:#1d2025;
  --surface-raised:#23272d;
  --line:#33373e;
  --text:#edeae2;
  --text-dim:#a4aab2;
  --amber:#f2a33d;
  --amber-dim:#8a6326;
  --teal:#4fd1ae;
  --danger:#e5484d;
  --radius:14px;
}
*{box-sizing:border-box;}
html,body{margin:0;padding:0;}
body{
  background:var(--bg);
  color:var(--text);
  font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;
  -webkit-font-smoothing:antialiased;
  min-height:100vh;
  padding:20px 16px 40px;
}
.mono{font-family:ui-monospace,SFMono-Regular,Menlo,Consolas,"Courier New",monospace;}
.wrap{max-width:420px;margin:0 auto;}

header.top{display:flex;align-items:center;justify-content:space-between;margin-bottom:22px;}
header.top .brand{display:flex;flex-direction:column;}
header.top .brand .name{font-size:22px;font-weight:800;letter-spacing:0.5px;}
header.top .brand .tag{font-size:11px;letter-spacing:1.5px;text-transform:uppercase;color:var(--text-dim);margin-top:2px;}
.status-dot{display:flex;align-items:center;gap:6px;font-size:11px;letter-spacing:1px;text-transform:uppercase;color:var(--teal);}
.status-dot .dot{width:8px;height:8px;border-radius:50%;background:var(--teal);box-shadow:0 0 0 3px rgba(79,209,174,0.15);}

.eyebrow{font-size:11px;font-weight:700;letter-spacing:1.5px;text-transform:uppercase;color:var(--text-dim);margin:0 0 10px 2px;}
section{margin-bottom:22px;}
.panel{background:var(--surface);border:1px solid var(--line);border-radius:var(--radius);padding:20px;}

.telemetry{display:flex;align-items:center;gap:20px;}
.gauge{position:relative;width:96px;height:96px;flex-shrink:0;}
.gauge svg{transform:rotate(-90deg);}
.gauge-track{fill:none;stroke:var(--line);stroke-width:8;}
.gauge-value{fill:none;stroke:var(--teal);stroke-width:8;stroke-linecap:round;transition:stroke-dashoffset 0.6s ease, stroke 0.3s ease;}
.gauge-label{position:absolute;inset:0;display:flex;align-items:center;justify-content:center;flex-direction:column;}
.gauge-label .pct{font-size:20px;font-weight:800;}
.gauge-label .icon{font-size:13px;margin-top:2px;}
.tele-info{flex:1;min-width:0;}
.tele-info .row{display:flex;justify-content:space-between;font-size:12px;color:var(--text-dim);padding:4px 0;border-bottom:1px dashed var(--line);}
.tele-info .row:last-child{border-bottom:none;}
.tele-info .row span:last-child{color:var(--text);}

.scan-btn{
  position:relative;width:100%;display:flex;align-items:center;justify-content:center;gap:12px;
  background:var(--surface-raised);border:1px solid var(--amber-dim);color:var(--amber);
  font-size:15px;font-weight:700;letter-spacing:0.5px;text-transform:uppercase;
  padding:18px;border-radius:12px;cursor:pointer;overflow:hidden;
  -webkit-tap-highlight-color:transparent;
}
.scan-btn:active{transform:scale(0.98);}
.scan-btn:disabled{color:var(--text-dim);border-color:var(--line);cursor:not-allowed;}
.scan-btn .ring{width:20px;height:20px;border-radius:50%;border:2px solid currentColor;position:relative;flex-shrink:0;}
.scan-btn.scanning .ring{
  background:conic-gradient(from 0deg, var(--amber) 0deg, transparent 90deg, transparent 360deg);
  animation:sweep 1s linear infinite;border-color:transparent;
}
.scan-btn.scanning .ring::after{content:"";position:absolute;inset:3px;border-radius:50%;background:var(--surface-raised);}
@keyframes sweep{to{transform:rotate(360deg);}}

.preview-frame{margin-top:16px;border-radius:10px;overflow:hidden;border:1px solid var(--line);background:#000;display:none;}
.preview-frame img{display:block;width:100%;}

.log{background:#0b0c0e;border:1px solid var(--line);border-radius:12px;padding:14px 16px;height:150px;overflow-y:auto;font-size:12.5px;line-height:1.6;}
.log .entry{border-bottom:1px solid #1a1c20;padding-bottom:8px;margin-bottom:8px;}
.log .entry:last-child{border-bottom:none;margin-bottom:0;padding-bottom:0;}
.log .t{color:var(--text-dim);margin-right:8px;}
.log .msg{color:var(--text);}
.log .msg.err{color:var(--danger);}
.log .msg.ai{color:var(--teal);}

button:focus-visible, .scan-btn:focus-visible{outline:2px solid var(--amber);outline-offset:3px;}
@media (prefers-reduced-motion: reduce){.scan-btn.scanning .ring{animation:none;}}
</style>
</head>
<body>
<div class="wrap">
  <header class="top">
    <div class="brand">
      <div class="name">AdmAIre</div>
      <div class="tag">Console dispositivo</div>
    </div>
    <div class="status-dot"><span class="dot"></span>Online</div>
  </header>

  <section>
    <div class="eyebrow">Telemetria</div>
    <div class="panel telemetry">
      <div class="gauge">
        <svg viewBox="0 0 96 96" width="96" height="96">
          <circle class="gauge-track" cx="48" cy="48" r="42"></circle>
          <circle class="gauge-value" id="gaugeArc" cx="48" cy="48" r="42" stroke-dasharray="264" stroke-dashoffset="264"></circle>
        </svg>
        <div class="gauge-label">
          <div class="pct mono" id="batVal">--%</div>
          <div class="icon" id="chgIcon">●</div>
        </div>
      </div>
      <div class="tele-info">
        <div class="row"><span>Stato</span><span id="batState">—</span></div>
        <div class="row"><span>Autonomia</span><span id="batTime" class="mono">—</span></div>
        <div class="row"><span>Rete</span><span class="mono">connesso</span></div>
      </div>
    </div>
  </section>

  <section>
    <div class="eyebrow">Comando</div>
    <div class="panel">
      <button id="snapBtn" class="scan-btn" onclick="avviaCicloAnalisi()">
        <span class="ring" aria-hidden="true"></span>
        <span id="btnLabel">Avvia scansione</span>
      </button>
      <div class="preview-frame" id="previewFrame">
        <img id="preview" alt="Ultima immagine acquisita dalla fotocamera">
      </div>
    </div>
  </section>

  <section>
    <div class="eyebrow">Registro eventi</div>
    <div class="panel log" id="log" role="log" aria-live="polite"></div>
  </section>
</div>

<script>
function scriviLog(testo, tipo){
  const logDiv=document.getElementById('log');
  const time=new Date().toLocaleTimeString('it-IT',{hour:'2-digit',minute:'2-digit',second:'2-digit'});
  const cls=tipo?(' '+tipo):'';
  logDiv.insertAdjacentHTML('beforeend','<div class="entry"><span class="t mono">'+time+'</span><span class="msg'+cls+'"></span></div>');
  logDiv.lastElementChild.querySelector('.msg').textContent = testo;
  logDiv.scrollTop=logDiv.scrollHeight;
}
scriviLog('Sistema inizializzato.');

const GAUGE_CIRC = 264;
async function aggiornaStatoBatteria(){
  try{
    let r=await fetch('/battery');
    let data=await r.json();
    document.getElementById('batVal').innerText=data.pct+'%';
    document.getElementById('chgIcon').innerText=data.charging?'⚡':'●';
    document.getElementById('batState').innerText=data.charging?'In carica':'A batteria';
    document.getElementById('batTime').innerText=data.time;
    let color = (data.charging || data.pct>20) ? 'var(--teal)' : 'var(--danger)';
    let arc=document.getElementById('gaugeArc');
    arc.style.stroke=color;
    let offset = GAUGE_CIRC - (GAUGE_CIRC*Math.min(data.pct,100)/100);
    arc.style.strokeDashoffset=offset;
  }catch(e){}
}
setInterval(aggiornaStatoBatteria,5000);
aggiornaStatoBatteria();

async function avviaCicloAnalisi(){
  const btn=document.getElementById('snapBtn');
  const label=document.getElementById('btnLabel');
  btn.disabled=true;
  btn.classList.add('scanning');
  label.innerText='Acquisizione...';
  scriviLog('Attivazione fotocamera...');
  try{
    let res=await fetch('/capture');
    if(!res.ok) throw new Error('Errore fotocamera');
    let base64Img=await res.text();
    const frame=document.getElementById('previewFrame');
    const img=document.getElementById('preview');
    img.src='data:image/jpeg;base64,'+base64Img;
    frame.style.display='block';

    label.innerText='Analisi in corso...';
    scriviLog('Invio al motore AI...');
    let resAi=await fetch('/analyze');
    let testoAi=await resAi.text();
    scriviLog(testoAi, 'ai');

    label.innerText='Riproduzione...';
    setTimeout(()=>{
      label.innerText='Avvia scansione';
      btn.disabled=false;
      btn.classList.remove('scanning');
    },2000);
  }catch(err){
    scriviLog('Errore: '+err.message,'err');
    label.innerText='Riprova';
    btn.disabled=false;
    btn.classList.remove('scanning');
  }
}
</script>
</body>
</html>
)HTMLPAGE";

void gestisciRoot() {
    server.send_P(200, "text/html", PAGINA_HTML);
}

void setup() {
    Serial.begin(115200);
    delay(2000); 
    Serial.println("--- AVVIO SISTEMA ADMAIRE ---");

    // Verifica PSRAM
    Serial.printf("[PSRAM] dimensione totale: %u byte, libera: %u byte\n",
                  ESP.getPsramSize(), ESP.getFreePsram());
    if (ESP.getPsramSize() == 0) {
        Serial.println("[PSRAM] ATTENZIONE: nessuna PSRAM rilevata! Controlla Tools > PSRAM in Arduino IDE.");
    }

    setCpuFrequencyMhz(160); 

    if (!inizializzaXiaoCamera()) {
        Serial.println("[ERRORE CRITICO] Inizializzazione modulo Camera fallita!");
        while (true) { delay(1000); }
    }
    Serial.println("[OK] Interfaccia Camera configurata.");

    WiFi.persistent(false);
    WiFi.disconnect(true);
    delay(500);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    Serial.print("[Wi-Fi] Connessione a " + String(ssid) + "...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\n[OK] Connesso! IP: http://");
    Serial.println(WiFi.localIP());

    WiFi.setSleep(false);

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21); 

    // Test di Avvio
    audio.connecttospeech("Sistema pronto", "it");

    server.on("/", HTTP_GET, gestisciRoot);
    server.on("/capture", HTTP_GET, gestisciCattura);
    server.on("/analyze", HTTP_GET, gestisciAnalisi);
    server.on("/battery", HTTP_GET, gestisciBatteria);
    
    server.enableCORS(true); 
    server.begin();
    Serial.println("[SISTEMA] Pronti all'uso.");
}

void loop() {
    audio.loop();
    server.handleClient(); 

    if (testoDaVocalizzare != "") {
        // Per garantire la totale pulizia del Wi-Fi
        if (millis() - timestampRicezioneTesto > 2500) {
            testoCorrenteAudio = pulisciTesto(testoDaVocalizzare);
            testoDaVocalizzare = ""; 
            
            if (testoCorrenteAudio.length() > 0) {
                Serial.print("[AUDIO] Riproduzione voce in corso: ");
                Serial.println(testoCorrenteAudio);
                Serial.printf("[AUDIO] Heap libero prima del TTS: %u\n", ESP.getFreeHeap());

                if (audio.isRunning()) {
                    audio.stopSong();
                    delay(200);
                }

                bool avviato = audio.connecttospeech(testoCorrenteAudio.c_str(), "it");
                if (!avviato) {
                    Serial.println("[AUDIO] connecttospeech() ha restituito false, ritento tra 400ms");
                    delay(400);
                    avviato = audio.connecttospeech(testoCorrenteAudio.c_str(), "it");
                    Serial.printf("[AUDIO] Secondo tentativo: %s\n", avviato ? "OK" : "FALLITO");
                }
            }
        }
    }
}

// Filtro del testo
String pulisciTesto(String testoSporco) {
    String pulito = testoSporco;
    pulito.replace("*", "");    
    pulito.replace("\"", "");   
    pulito.replace("#", "");    
    pulito.replace("\\n", " "); 
    pulito.replace("\n", " ");
    pulito.replace("\r", " ");
    pulito.trim();              
    return pulito;
}

void gestisciCattura() {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) { server.send(500, "text/plain", "ERRORE ACQUISIZIONE FOTO"); return; }
    size_t sizeBase64 = ((fb->len + 2) / 3) * 4 + 1;
    char *bufferBase64 = (char *)malloc(sizeBase64);
    size_t out_len = 0;
    mbedtls_base64_encode((unsigned char *)bufferBase64, sizeBase64, &out_len, fb->buf, fb->len);
    server.send(200, "text/plain", String(bufferBase64));
    free(bufferBase64);
    esp_camera_fb_return(fb);
}

// ============================================================================
// ENDPOINT AI - STAFFETTA MEMORIA E GESTIONE AUDIO SICURA
// ============================================================================
void gestisciAnalisi() {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) { 
        server.send(500, "text/plain", "Errore fotocamera"); 
        return;
    }

    size_t sizeBase64 = ((fb->len + 2) / 3) * 4 + 1;
    char *bufferBase64 = (char *)malloc(sizeBase64);
    size_t out_len = 0;
    mbedtls_base64_encode((unsigned char *)bufferBase64, sizeBase64, &out_len, fb->buf, fb->len);
    
    String payload;
    payload.reserve(sizeBase64 + 1000); 
    
    payload += "{\"model\":\"nvidia/nemotron-3-nano-omni-30b-a3b-reasoning:free\",\"max_tokens\":20,\"reasoning\":{\"enabled\":false},\"messages\":[{\"role\":\"user\",\"content\":[{\"type\":\"text\",\"text\":\"Sei l'assistente di un non vedente. Rispondi in italiano in modo telegrafico (massimo 8 parole). Indica solo l'ostacolo principale (strada rialzata, ostacoli). Altrimenti, rispondi 'via libera'.\"},{\"type\":\"image_url\",\"image_url\":{\"url\":\"data:image/jpeg;base64,";
    payload += bufferBase64;
    payload += "\"}}]}]}";
    
    free(bufferBase64);
    esp_camera_fb_return(fb);

    String responseText = "";

    // Allocazione standard
    {
        WiFiClientSecure client;
        client.setInsecure();
        
        HTTPClient http;
        http.setReuse(false); // Impedisce il blocco di rete post-chiamata
        http.begin(client, "https://openrouter.ai/api/v1/chat/completions");
        http.setTimeout(25000); 
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "Bearer " + String(openrouter_key));
        
        int httpCode = http.POST(payload);
        
        // Distruzione della foto
        payload = ""; 
        
        if (httpCode == 200) {
            String response = http.getString();
            int contentIdx = response.indexOf("\"content\"");
            if (contentIdx != -1) {
                int duePunti = response.indexOf(":", contentIdx);
                int avvioQuote = response.indexOf("\"", duePunti);
                if (avvioQuote != -1) {
                    int fineQuote = response.indexOf("\"", avvioQuote + 1);
                    while (fineQuote != -1 && response.charAt(fineQuote - 1) == '\\') {
                        fineQuote = response.indexOf("\"", fineQuote + 1);
                    }
                    responseText = response.substring(avvioQuote + 1, fineQuote);
                    responseText.replace("\\n", " ");
                    responseText.replace("\\\"", "");
                }
            }
        } else {
            String errBody = http.getString();
            Serial.print("[AI] Errore HTTP ");
            Serial.print(httpCode);
            Serial.print(": ");
            Serial.println(errBody);
            responseText = "Errore di rete: " + String(httpCode);
        }
        
        http.end(); 
        client.stop(); // Chiusura hardware della porta di rete
    } 

    // Avviso al telefono di non tenere la porta occupata
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", responseText);
    
    if (responseText != "" && !responseText.startsWith("Errore") && responseText.trim() != "via libera") {
        testoDaVocalizzare = responseText;
        timestampRicezioneTesto = millis();
    }
}

void gestisciBatteria() {
    uint32_t v = 0;
    for(int i=0; i<10; i++) { v += analogReadMilliVolts(PIN_BATTERIA); delay(5); }
    float vBat = (v / 10.0) * 2.0 / 1000.0; 
    bool inCarica = (vBat > 4.25);
    float pct = (vBat - 3.7) * 200.0;
    if(pct > 100.0) pct = 100.0; if(pct < 0.0) pct = 0.0;
    String tempoStimato = "";
    if (inCarica) {
        float oreRimaste = ((100.0 - pct) / 100.0) * 30.0;
        int h = (int)oreRimaste; int m = (int)((oreRimaste - h) * 60);
        tempoStimato = (pct >= 99.0) ? "Carica Completata" : "Circa " + String(h) + "h " + String(m) + "m al termine";
    } else {
        float oreRimaste = (pct / 100.0) * 10.0;
        int h = (int)oreRimaste; int m = (int)((oreRimaste - h) * 60);
        tempoStimato = (pct <= 5.0) ? "Batteria Scarica! Collegare l'alimentatore." : "Circa " + String(h) + "h " + String(m) + "m rimanenti";
    }
    String jsonResponse = "{\"pct\":" + String((int)pct) + ",\"charging\":" + (inCarica ? "true" : "false") + ",\"time\":\"" + tempoStimato + "\"}";
    server.sendHeader("Connection", "close");
    server.send(200, "application/json", jsonResponse);
}

// ============================================================================
// INIZIALIZZAZIONE FOTOCAMERA E AUDIO
// ============================================================================
bool inizializzaXiaoCamera() {
    camera_config_t config;
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
    config.frame_size = FRAMESIZE_QVGA;  
    config.jpeg_quality = 10;            
    config.fb_count = 1;                 
    config.fb_location = CAMERA_FB_IN_PSRAM; 
    config.grab_mode = CAMERA_GRAB_LATEST;

    esp_err_t err = esp_camera_init(&config);
    return (err == ESP_OK);
}

// Callback diagnostico della libreria
void audio_info(const char *info) {
    Serial.print("[AUDIO INFO] ");
    Serial.println(info);
}

void audio_eof_speech(const char *info) {
    Serial.print("[AUDIO] Riproduzione terminata: ");
    Serial.println(info);
}