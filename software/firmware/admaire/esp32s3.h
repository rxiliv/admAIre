#pragma once

#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "mbedtls/base64.h"

#include "html_page.h"

WebServer server;

String testoDaVocalizzare;
unsigned long timestampRicezioneTesto;

// Prototipi funzioni
bool inizializzaXiaoCamera();
void gestisciRoot();
void gestisciCattura();
void gestisciAnalisi();
void gestisciBatteria();
String pulisciTesto(String testoSporco);