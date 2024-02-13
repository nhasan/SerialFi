#include "SerialFi.h"

const byte bufsize = 64;
const char newline = '\n';
char buffer[bufsize];

byte recvData(char *buf, byte len) {
    byte i = peer.readBytesUntil('\n', buf, len-1);
    buf[i] = '\0';
    while(isspace(buf[i-1])) {
        buf[--i] = '\0';
    }
    return i;
}

void loop() {
    byte len = recvData(buffer, bufsize);
    if (len > 0) {
        sendToAllClients(buffer);
    }
    ws.cleanupClients();
}
