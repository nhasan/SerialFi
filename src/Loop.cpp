#include "SerialFi.h"

#define BUF_SIZE 128

const char newline = '\n';
CircularBuffer<char, BUF_SIZE> txbuf;

void loop() {
  static int pos = 0;
  static char buffer[BUF_SIZE];
  if (peer.available()) {
    char c = peer.read();
    if (c != '\n') {
      buffer[pos++] = c;
    }
    if (c == '\n' || pos == sizeof(buffer)) {
      while(isspace(buffer[pos-1])) {
        buffer[--pos] = '\0';
      }
      sendToAllClients(buffer);
      pos = 0;
    }
  }
  if (!txbuf.isEmpty()) {
    char c = txbuf.shift();
    peer.write(c);
  }
  ws.cleanupClients();
}
