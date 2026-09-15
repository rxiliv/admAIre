#include "esp32s3.h"

WebServer server(80);

String testoDaVocalizzare = ""; 
unsigned long timestampRicezioneTesto = 0;

void gestisciRoot() {
    server.send_P(200, "text/html", PAGINA_HTML);
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
    
    if (responseText != "" && !responseText.startsWith("Errore") && responseText != " via libera") {
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