#include "Arduino.h"
#include <AsyncWebSocket.h>

#pragma once
#ifndef ROBOTARM_H
#define ROBOTARM_H
const char *ssid = "ESP32-TOAST-AP";
const char *password = "TEJ3M12021";
const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;
const int armPin = 14;
const int basePin = 27;
const int gripperPin = 16;

void onIndexRequest(AsyncWebServerRequest *request);
void onCSSRequest(AsyncWebServerRequest *request);
void onJSRequest(AsyncWebServerRequest *request);

#endif  // ROBOTARM_H