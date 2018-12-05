#include "mywebsocket.hh"
#include "libwshandshake/libwshandshake.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <math.h>
// send back response
void sendResponse(int socket) {

}

void websocketSlave(int socket, char * request_s) {
  printf("******************WEBSOCKET********************\n");
  printf("%s\n", request_s);
  printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
  char * key;
  std::string r(request_s);
  int start_pos = r.find("Sec-WebSocket-Key: ") + std::string("Sec-WebSocket-Key: ").size();
  int end_pos = r.find("\r\n", start_pos);
  int i = 0;
  int sl = end_pos - start_pos;
  key = new char[sl + 1];
  while (i < sl) {
    key[i] = r.at(i + start_pos);
    i++;
  }
  key[i] = '\0';
  char * output = new char[29];
  WebSocketHandshake::generate(key, output);
  printf("\noutput = \"%s\"\n", output);
  std::string rh = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
  rh += std::string(output);
  rh += std::string("\r\n\r\n");
  delete key;
  delete output;
  write(socket, rh.c_str(), rh.size());
  
  // print bytes to check what's going on
  char * buffer = new char[113+14];
  read(socket, buffer, 4);
  for(int i = 0; i < 127 * 8; i++) {
    int tn_i = pow(2, i%8);
    char tn = 255 & tn_i;
    int ri = i/8;
    tn = tn & buffer[ri];
    if (tn != 0) printf("1 ");
    else printf("0 ");
    if (i % 32 == 31) {
      printf("\n");
    }
    else if (i%8 == 7) {
      printf(" ||  ");
    }
  }
  printf("\n");
  fflush(stdout);
  // read content len 113 is from length I got above
  char * content = new char[113];
  read(socket, content, 113);
  char * mask = new char[4];
  mask[0] = buffer[10];
  mask[1] = buffer[11];
  mask[2] = buffer[12];
  mask[3] = buffer[13];
  char * decoded = new char[114];
  // decoding 
  i = 0;
  while (i < 113) {
    decoded[i] == content[i] ^ mask[i % 4];
    i++;
  }
  decoded[i] = '\0';
  int j = 0;
  // print decoded string (none)
  while (j < 113) {
    printf("%c", decoded[j]);
    j++;
  }
  delete buffer, decoded, mask, buffer;
  /*char * buffer = new char[1];
  // start reading
  while (1) {
    // first byte:
    // fin rsv1 rsv2 rsv3 opcode*4
    read(socket, buffer, 1);
    char t = buffer[0];
    // fin
    char end_c = 128;
    bool end = (t & end_c) == 128;
    // opcode
    char opcode = 15;
    opcode = opcode & t;
    if (end) printf("end!\n");
    else printf("NOTEND!\n");
    printf("opcode is %d\n", opcode);
    // read second byte
    read(socket, buffer, 1);
    t = buffer[0];
    char mask = 128 & t;
    char payload_len = 127 & t;
    size_t l;
    if (payload_len == 126) {
      read(socket, buffer, 1);
      t = buffer[0];
      l = t;
      read(socket, buffer, 1);
      t = buffer[0];
      l = l * 256 + t;
      read(socket, buffer, 1);
      read(socket, buffer, 1);
      read(socket, buffer, 1);
      read(socket, buffer, 1);
      read(socket, buffer, 1);
      read(socket, buffer, 1);
    }
    else if (payload_len == 127) {
      read(socket, buffer, 1);
      t = buffer[0];
      l = t;
      read(socket, buffer, 1);
      t = buffer[0];
      l = l * 256 + t;
      read(socket, buffer, 1);
      t = buffer[0];
      l = l * 256 + t;
      read(socket, buffer, 1);
      t = buffer[0];
      l = l * 256 + t;
      read(socket, buffer, 1);
      t = buffer[0];
      l = l * 256 + t;
      read(socket, buffer, 1);
      t = buffer[0];
      l = l * 256 + t;
      read(socket, buffer, 1);
      t = buffer[0];
      l = l * 256 + t;
      read(socket, buffer, 1);
      t = buffer[0];
      l = l * 256 + t;
    }
    else {
      l = payload_len;
      read(socket, buffer, 1);
      read(socket, buffer, 1);
      read(socket, buffer, 1);
      read(socket, buffer, 1);
      read(socket, buffer, 1);
      read(socket, buffer, 1);
      read(socket, buffer, 1);
      read(socket, buffer, 1);
    }
    char * mask_key = new char[4];
    read(socket, mask_key, 4);
    char * payload = new char[l];
    read(socket, payload, l);
    char * decoded = new char[l];
    for (size_t i = 0; i < l; i++) {
      decoded[i] = payload[i] ^ mask_key[i % 4];
      printf("%c", decoded[i]);
    }
    delete mask_key;
    delete payload;
    delete decoded;
    break;
  }
  delete buffer;
  char * tw = new char[16];
  tw[0] = 128 + 1;
  tw[1] = 0 + 2;
  tw[3] = 0;
  tw[4] = 0;
  tw[5] = 0;
  tw[6] = 0;
  tw[7] = 0;
  tw[8] = 0;
  tw[9] = 0;
  tw[10] = 0;
  tw[11] = 0;
  tw[12] = 0;
  tw[13] = 0;
  tw[14] = 'A';
  tw[15] = '\0';
  write(socket, tw, 16);
  delete tw;*/
}


