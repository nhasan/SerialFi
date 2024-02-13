#ifndef SerialFi_h
#define SerialFi_h

#define SERIALFI_VERSION "0.1"
#define DEVICE_NAME "SerialFi"
#define HTTP_PORT 80

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266LLMNR.h>
#include <DNSServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <LittleFS.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

#include "utilities.h"
#include "WebServer.h"

struct settings {
    char ident[10];
    uint32 baud;
};

extern PGM_P deviceName;
extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern SoftwareSerial peer;
extern settings eeprom;

#endif