#pragma once

#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "mbedtls/base64.h"

#include "html_page.h"

#if __has_include("secrets.h")
#include "secrets.h"
#else
#error "Manca secrets.h: copia secrets.example.h in secrets.h e inserisci Wi-Fi e chiave Groq"
#endif

// ==========================================
// PINOUT HARDWARE ADMAIRE
// ==========================================
#define I2S_BCLK      D1
#define I2S_LRC       D2
#define I2S_DOUT      D3
#define PIN_BATTERIA  D4
// Opzionale: partitore 100k/100k dal pin 5V (VUSB) a un GPIO libero per
// rilevare con certezza quando il cavo USB è collegato (carica in corso).
// #define PIN_USB_SENSE D5

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
// CONFIGURAZIONE AI (Groq, API compatibile OpenAI)
// ==========================================
#define GROQ_URL          "https://api.groq.com/openai/v1/chat/completions"
#define GROQ_MODEL        "qwen/qwen3.8-27b"
#define MAX_CARATTERI_VOCE 200   // limite del TTS di Google usato da ESP32-audioI2S

// ==========================================
// CONFIGURAZIONE BATTERIA
// ==========================================
#define BATT_PARTITORE     2.0f   // R1 = R2 = 100k
#define BATT_CALIBRAZIONE  1.00f  // correggi confrontando con un multimetro
#define BATT_CAPACITA_MAH  1600
#define BATT_CARICA_MA     100    // corrente di carica del caricatore integrato XIAO ESP32S3
#define BATT_CONSUMO_MA    200    // consumo medio stimato (Wi-Fi + camera + audio)
#define BATT_CADUTA_MV     30     // caduta di tensione sotto carico da compensare

#define VOLUME_DEFAULT     21     // 0..21

extern WebServer server;
extern volatile int  volumeRichiesto;
extern volatile bool audioInRiproduzione;
extern uint8_t volumeCorrente;

bool inizializzaXiaoCamera();
void avviaTaskAI();
void configuraServerWeb();
void aggiornaBatteria();
void accodaVoce(const String &testo);
bool prelevaTestoDaVocalizzare(String &out);
String pulisciTesto(const String &testo);
