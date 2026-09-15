#include "Audio.h" 

#include "esp32s3.h"

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
String testoCorrenteAudio = ""; // Buffer protetto per la RAM

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

// Callback diagnostico della libreria
void audio_info(const char *info) {
    Serial.print("[AUDIO INFO] ");
    Serial.println(info);
}

void audio_eof_speech(const char *info) {
    Serial.print("[AUDIO] Riproduzione terminata: ");
    Serial.println(info);
}