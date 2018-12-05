#include "myhttpd.hh"
void iconSlave(int socket);
void logSlave(int socket);
void statsSlave(int socket);
void modSlave(int socket, char * file, char * query);
const char * URLParser(const char * request_s);
void logWriter(const char * request_s);
void cgiSlave(int socket, char * file, char * query);
void wrongURLSlave(int socket, const char * request_s);
void directorySlave(int socket, char * dir, char * query);
void fileSlave(int socket, char * request_s, TargetType target_type);
bool bouncerSlave(const char * request_s);
