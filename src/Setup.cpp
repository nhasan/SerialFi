#include "SerialFi.h"

#define TIME_ZONE "EST5EDT,M3.2.0,M11.1.0"
#define NTP_SERVER "pool.ntp.org"

DNSServer dns;
const char *deviceName;
settings eeprom;
SoftwareSerial peer(D2, D1);

void configModeCallback (AsyncWiFiManager *wiFiManager) {
}

void setup() {
  deviceName = charptr(F(DEVICE_NAME));
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D0, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(D0, LOW);

  EEPROM.begin(sizeof(eeprom));
  EEPROM.get(0, eeprom);
  if (strcmp(eeprom.ident, deviceName) != 0 ) {
    memset(&eeprom, 0, sizeof(eeprom));
    strncpy(eeprom.ident, deviceName, sizeof(eeprom.ident));
    eeprom.baud = 9600;
    EEPROM.put(0, eeprom);
    EEPROM.commit();
  }

  Serial.begin(115200);
  peer.begin(eeprom.baud);

  WiFi.hostname(deviceName);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  
  Serial.println(deviceName);

  AsyncWiFiManager wifiManager(&server, &dns);
  //wifiManager.resetSettings();
  wifiManager.setDebugOutput(false);
  wifiManager.setConfigPortalTimeout(300);
  wifiManager.setAPCallback(configModeCallback);
  char ssid[17];
  char passwd[8];
  sprintf(passwd, "%04X", ESP.getChipId() & 0xFFFF);
  sprintf(ssid, "%s-%s", deviceName, passwd);
  Serial.println(F("Connecting with WiFiManager."));
  Serial.printf(PSTR("AP: ssid=%s, password=%s\n"), ssid, passwd);
  wifiManager.autoConnect(ssid, passwd);

  if(WiFi.status() != WL_CONNECTED) {
      Serial.println(F("Failed to connect to WiFi. Restarting."));
      delay(5000);
      ESP.reset();
  }

  Serial.printf(PSTR("WiFi connected. SSID=%s, IP=%s, channel=%d, RSSI %ddb\n"),
    WiFi.SSID().c_str(), WiFi.localIP().toString().c_str(), WiFi.channel(), WiFi.RSSI());
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(D0, HIGH);

  if (MDNS.begin(deviceName)) {
    MDNS.addService("http","tcp",80);
  }
  LLMNR.begin(deviceName);

  if (!LittleFS.begin()) {
    Serial.println(F("Unable to mount Flash filesystem using LittleFS"));
    WiFi.disconnect();
    return;
  }

  // Set current time using NTP
  configTime(charptr(F(TIME_ZONE)), charptr(F(NTP_SERVER)));
  
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.printf(PSTR("Webserver started on port %d\n"), HTTP_PORT);
}

