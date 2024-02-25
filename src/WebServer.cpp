#include <ArduinoJson.h>

#include "WebServer.h"

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");
char json[256];

void handleNotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    Serial.printf_P(PSTR("Recieved from client #%u: %s\n"), client->id(), data);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data, len);
    if (error) {
      Serial.print(F("JSON parsing error: "));
      Serial.println(error.f_str());
      return;
    }
    const char* type = doc[F("type")];
    if (strcmp_P(type, F("baud")) == 0) {
      uint32 baud = doc[F("value")];
      peer.flush();
      peer.begin(baud);
      eeprom.baud = baud;
      EEPROM.put(0, eeprom);
      EEPROM.commit();
      Serial.printf_P(PSTR("Changed serial port baud to %d bps\n"), baud);
    } else if (strcmp_P(type, F("passthrough")) == 0) {
      const char* value = doc[F("value")];
      int pos = 0;
      while (value[pos] != '\0' && !txbuf.isFull()) {
        txbuf.push(value[pos++]);
      }
      if ((value[pos] != '\0' && txbuf.isFull())) {
        Serial.printf_P(PSTR("Tx buffer was full, only %d chars sent.\n"), pos);
      }
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      {
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      JsonDocument doc;
      doc[F("type")] = F("info");
      doc[F("baud")] = eeprom.baud;
      doc[F("device")] = deviceName;
      serializeJson(doc, json);
      Serial.printf_P(PSTR("Sending to client #%u: %s\n"), client->id(), json);
      ws.text(client->id(), json);
      }
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf_P(PSTR("WebSocket client #%u disconnected\n"), client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(client, arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void sendToAllClients(const char *msg) {
  if (ws.count() > 0) {
    JsonDocument doc;
    doc[F("type")] = F("data");
    doc[F("content")] = msg;
    serializeJson(doc, json);
    Serial.printf_P(PSTR("Sending: %s\n"), json);
    ws.textAll(json);
  }
}