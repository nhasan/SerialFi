#ifndef WebServer_h
#define WebServer_h

#include "SerialFi.h"

void handleNotFound(AsyncWebServerRequest *request);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void sendToAllClients(const char *msg);

#endif
