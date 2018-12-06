#ifndef MYHTTPD_H
#define MYHTTPD_H
extern const char * logfile;
extern int port;
extern long time_max, time_min;
extern long clock_begin;
extern int request_counter;
typedef enum TargetType {
//  0      1     2       3     4       5   
  DIR_T, CGI_T, TXT_T, MOD_T, LOG_T, STA_T, 
//  6     7       8      9     10 
  UCP_T, IDX_T, HTM_T, BMP_T, GIF_T, 
//  11    12     13    14
  ICO_T, JPG_T, PNG_T, SVG_T,
// 15     16     17
  NAN_T, CSS_T, NUL_T
  } TargetType;
struct Request {
  char * _request_type;
  char * _target;
  TargetType _target_type;
  char * _auth;
  char * _query;
  char * _HTTP_accept;
  char * _host;
  char * _user;
  char * _referer;
  char * _content_type;
  bool _secure;
};
void whip(int socket);
void forkSlave(int socket);
void threadSlave(int socket);
void poolSlave(int master_socket);
#endif
