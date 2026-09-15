#pragma once

#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "mbedtls/base64.h"

#include "html_page.h"

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
// I TUOI DATI WI-FI ED API
// ==========================================
inline const char* ssid     = ""; 
inline const char* password = "";     
inline const char* openrouter_key = "";

extern WebServer server;

extern String testoDaVocalizzare;
extern unsigned long timestampRicezioneTesto;

// Prototipi funzioni
bool inizializzaXiaoCamera();
void gestisciRoot();
void gestisciCattura();
void gestisciAnalisi();
void gestisciBatteria();
String pulisciTesto(String testoSporco);