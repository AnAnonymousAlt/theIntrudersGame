#include "myhttpd.hh"

char * headerMaker(TargetType target_type);
bool requestParser(char * request_s, struct Request * r);
char * requestReader (int socket);
bool modJudger(const char * request_s);
bool dofJudger(const char * request_s);
void envWriter(struct Request * request);
bool cgiJudger(const char * request_s);
bool newModChecker(const char * mod_s);
void swiper(struct Request * request);
int fileReader(char * file, TargetType target_type, char ** content);

const char * fileNameParser(const char * request_s);
const char * URLParser(const char * request_s);
void logWriter(const char * request_s);

