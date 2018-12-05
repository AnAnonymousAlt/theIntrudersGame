
const char * usage =
"                                                               \n"
"daytime-server:                                                \n"
"                                                               \n"
"Simple server program that shows how to use socket calls       \n"
"in the server side.                                            \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   daytime-server <port>                                       \n"
"                                                               \n"
"Where 1024 < port < 65536.             \n"
"                                                               \n"
"In another window type:                                       \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where daytime-server  \n"
"is running. <port> is the port number you used when you run   \n"
"daytime-server.                                               \n"
"                                                               \n"
"Then type your name and return. You will get a greeting and   \n"
"the time of the day.                                          \n"
"                                                               \n";
#include "slaves.hh"
#include "tools.hh"
#include <vector>
#include <dlfcn.h>
#include <math.h>
#include "dir_functions.hh"
#include <pthread.h>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "myhttpd.hh"
#include "mywebsocket.hh"
pthread_mutex_t lock;
const char * logfile = "log.txt";
const char * secret = "XY";
int port;
int QueueLength = 5;
std::vector<std::string> module_list;
std::vector<void *> op_list;
long time_max, time_min;
long clock_begin;
struct itimerspec new_value;
void zombie_catcher(int signum, siginfo_t * info, void * vp) {
  //printf("zombie\n");
  int status;
  waitpid(info->si_pid, &status, WNOHANG);
}

int request_counter;
// ignore alarm clock
void ALARMhandler(int sig) {
  signal(SIGALRM, SIG_IGN);
  signal(SIGALRM, ALARMhandler);
}

// Processes time request
int main( int argc, char ** argv )
{ 
  signal(SIGALRM, ALARMhandler);
  request_counter = 0;
  FILE * fp = fopen(logfile, "r+");
  if (fp == NULL) {
    fp = fopen(logfile, "w+");
    fprintf(fp, 
        "========================================================"
        "\n\t\t\tLOGFILE\n"
        "========================================================\n");
  }
  fclose(fp);

  time_min = -1;
  time_max = -1;

  port = 1044;
  struct sigaction act;
  act.sa_sigaction = zombie_catcher;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO | SA_RESTART;
  int sigr = sigaction(SIGCHLD, &act, NULL);
  if (sigr) {
    perror("sigaction error");
    exit(2);
  }
  pthread_mutex_init(&lock, NULL);
  bool forkmode = false;
  bool threadmode = false;
  bool poolmode = false;
  // Print usage if not enough arguments
  if ( argc < 2 ) {
    port = 1044;
  }
  // Get the port from the arguments
  else if (argc < 3)
    port = atoi( argv[1] );
  else {
    if(argv[1][0] == '-') {
      port = atoi( argv[2] );
      if (argv[1][1] == 'p') poolmode = true;
      else if (argv[1][1] == 'f') forkmode = true;
      else if (argv[1][1] == 't') threadmode = true;
    }
    else if(argv[2][0] == '-') {
      port = atoi( argv[1] );
      if (argv[2][1] == 'p') poolmode = true;
      else if (argv[2][1] == 'f') forkmode = true;
      else if (argv[2][1] == 't') threadmode = true;
    }
  }
  // Set the IP address and port for this server
  struct sockaddr_in serverIPAddress; 
  memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
  serverIPAddress.sin_family = AF_INET;
  serverIPAddress.sin_addr.s_addr = INADDR_ANY;
  serverIPAddress.sin_port = htons((u_short) port);

  // Allocate a socket
  int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
  if ( masterSocket < 0) {
    perror("socket");
    exit( -1 );
  }

  // Set socket options to reuse port. Otherwise we will
  // have to wait about 2 minutes before reusing the sae port number
  int optval = 1; 
  int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
      (char *) &optval, sizeof( int ) );

  // Bind the socket to the IP address and port
  int error = bind( masterSocket,
      (struct sockaddr *)&serverIPAddress,
      sizeof(serverIPAddress) );
  if ( error ) {
    perror("bind");
    exit( -1 );
  }

  // Put socket in listening mode and set the 
  // size of the queue of unprocessed connections
  error = listen( masterSocket, QueueLength);
  if ( error ) {
    perror("listen");
    exit( -1 );
  }
  // timer and clock start
  clock_begin = (long)time(NULL);
  timer_t timer_request;
  timer_create(CLOCK_REALTIME, NULL, &timer_request);
  new_value.it_value.tv_sec = 10;
  new_value.it_value.tv_nsec = 0;
  new_value.it_interval.tv_sec = 0;
  new_value.it_interval.tv_nsec = 0;
  // pool setting
  if (poolmode) {
    pthread_t tid[5];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    for (int i =0; i < 5; i++) {
      pthread_create(&tid[i], &attr, (void *(*)(void *))poolSlave, (void *)masterSocket);
    }
    pthread_join(tid[0], NULL);
    return 0;
  }

  while ( 1 ) {
    // Accept incoming connections
    struct sockaddr_in clientIPAddress;
    int alen = sizeof( clientIPAddress );
    int slaveSocket = accept( masterSocket,
        (struct sockaddr *)&clientIPAddress,
        (socklen_t*)&alen);

    if ( slaveSocket < 0 ) {
      perror( "accept" );
      exit( -1 );
    }
    // fork
    if (forkmode) {
      forkSlave(slaveSocket);
      close(slaveSocket);
    }
    // pthread
    else if (threadmode) {
      pthread_t t;
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      pthread_create(&t, &attr, (void * (*) (void *)) threadSlave, (void *)slaveSocket);
    }
    else {
      // set timer
      new_value.it_value.tv_sec = 10;
      timer_settime(timer_request, 0, &new_value, NULL);
      struct itimerspec curr_value;
      timer_gettime(timer_request, &curr_value);
      long start_time = curr_value.it_value.tv_nsec;
      // Process request.
      whip(slaveSocket);
      // Close socket
      close( slaveSocket );
      timer_gettime(timer_request, &curr_value);
      long end_time = curr_value.it_value.tv_nsec;
      long elapse_time = start_time - end_time;
      if (elapse_time < 0) {
        elapse_time = start_time + (pow(10.0,9.0) * 10.0) - end_time;
      }
      if (time_max == -1) {
        time_max = elapse_time;
        time_min = elapse_time;
      }
      else {
        if (elapse_time > time_max) {
          time_max = elapse_time;
        }
        if (elapse_time < time_min) {
          time_min = elapse_time;
        }
      }
      new_value.it_value.tv_sec = 0;
      timer_settime(timer_request, 0, &new_value, NULL);
    }
  }
}
void poolSlave(int masterSocket) {
  while (1) {
    struct sockaddr_in clientIPAddress;
    int alen = sizeof( clientIPAddress );
    pthread_mutex_lock(&lock);
    int slaveSocket = accept( masterSocket,
        (struct sockaddr *)&clientIPAddress,
        (socklen_t*)&alen);
    pthread_mutex_unlock(&lock);
    if ( slaveSocket < 0 ) {
      perror( "accept" );
      exit( -1 );
    }
    // Process request.
    whip(slaveSocket);
    // Close socket
    close( slaveSocket );
  }
}
void threadSlave (int socket) {
  whip(socket);
  close(socket);
}
void forkSlave (int socket) {
  int pid = fork();
  if (pid == 0) {
    whip(socket);
    close(socket);
    exit(EXIT_SUCCESS);
  }
}
void whip (int socket) {
  char * request_s = (requestReader(socket));
  if (strlen(request_s) == 0) return;
  printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
  printf("---REQUEST---");
  printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
  printf("%s\n", request_s);  
  printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
  printf("\\\\\\\\\\\\\\\\\\\\\\\\\\");
  printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
  // make it string to parse
  std::string r(request_s);
  // Websocket
  if (r.find("Upgrade:") != std::string::npos
      && r.find("websocket") != std::string::npos) {
    websocketSlave(socket, request_s);
  }
  // unsecure
  if (r.find("Authorization") == std::string::npos) {
    std::string unsecure_message =
      "HTTP/1.1 401 Unauthorized\r\n"
      "WWW-Authenticate: Basic realm=\"myhttpd-cs252\"\r\n\r\n";
    write(socket, unsecure_message.c_str(), unsecure_message.size());
    return;
  }
  // 301
  std::string rr(r);
  rr.erase(0, rr.find(" ") + 1);
  rr.erase(rr.find(" "), std::string::npos);
  if (rr.find("?") != std::string::npos) {
    rr.erase(rr.find("?"), std::string::npos);
  }
  if (rr.compare("/") == 0) {
    fileSlave(socket, (char*)"http-root-dir/htdocs/index.html", HTM_T);
    return;
  }
  std::ifstream ifs;
  ifs.open("dir_index.txt", std::ifstream::in);
  std::string line;
  bool pair = false; // has the url in websites
  bool ucmp = false; // uncomplete
  while(getline(ifs, line)) {
    if (rr.compare(line) == 0) {
      pair = true;
    }
    line.erase(line.size()-1,1);
    if (rr.compare(line) == 0) {
      ucmp = true;
    }
  }
  ifs.close();
  // umcomplete url
  if (ucmp) {
    // make new location
    std::string url;
    // no referer
    if (r.find("Referer") == std::string::npos) {
      url = std::string("http://data.cs.purdue.edu:1044") + rr + std::string("/");
    }
    // find referer
    else {
      url = r;
      url.erase(0,url.find("Referer"));
      url.erase(url.find("\r\n"), std::string::npos);
      url.erase(0, std::string("Referer: ").size());
      url.erase(url.size()-1, 1);
      url += rr;
      url += std::string("/");
    }
    std::string header = 
      std::string("HTTP/1.1 301 Moved Permanently\r\nLocation: ") + url + std::string("\r\n\r\n");
    write(socket, header.c_str(), header.size());
    return;
  }
  // check file list
  ifs.open("file_index.txt", std::ifstream::in);
  while (getline(ifs, line)) {
    if (rr.compare(line) == 0) {
      pair = true;
    }
  }
  ifs.close();
  // check function list
  ifs.open("func_index.txt", std::ifstream::in);
  while (getline(ifs, line)) {
    if (rr.compare(line) == 0) {
      pair = true;
    }
  }
  // 404
  if (!pair) {
    std::string empty_page_message =
      "HTTP/1.1 404 Not Found\r\n"
      "content-type: text/html\r\n"
      "\r\n";
    (char*)"http-root-dir/htdocs/404.html";
    char * r;
    int size = fileReader((char*)"http-root-dir/htdocs/404.html", HTM_T, &r);
    write(socket, empty_page_message.c_str(), empty_page_message.size());
    write(socket, r, size);
    return;
  }
  request_counter++;
  struct Request * request = new struct Request();
  request->_request_type = NULL;
  request->_target = NULL;
  request->_auth = NULL;
  request->_target_type = NUL_T;
  request->_query = NULL;
  request->_user = NULL;
  request->_host = NULL;
  request->_HTTP_accept = NULL;
  request->_referer = NULL;
  request->_content_type = NULL;
  request->_secure = true;
  // parse request
  bool p_r = requestParser(request_s, request); // parser result
  logWriter(std::string(std::string(request->_host) + std::string("\t\t\t") + std::string(request->_target)).c_str());
  envWriter(request);
  printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\t\t\tSTRUCT REQUEST");
  printf("\n-----------------------------------------------------------\n");
  printf("request->_request_type\"%s\"\n", request->_request_type);
  if (request->_query != NULL)  printf("request->_query = \"%s\"\n", request->_query);
  printf("request->_target = \"%s\"\n", request->_target);
  printf("request->_host = \"%s\"\n", request->_host);
  printf("request->_HTTP_accept = %s\n", request->_HTTP_accept);
  printf("request->_user = %s\n", request->_user);
  printf("request->_target_type = %d\n", request->_target_type);
  if (request->_referer != NULL) printf("request->_referer = \"%s\"\n", request->_referer);
  printf("request->_content_type = %s\n", request->_content_type);
  printf("\n-----------------------------------------------------------");
  printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

  switch (request->_target_type) {
    case TXT_T:
    case IDX_T:
    case BMP_T:
    case ICO_T:
    case PNG_T:
    case SVG_T:
    case GIF_T:
    case JPG_T:
    case HTM_T:
    case CSS_T:
      fileSlave(socket, request->_target, request->_target_type);
      break;
    case DIR_T:
      directorySlave(socket, request->_target, request->_query);
      break;
    case LOG_T:
      logSlave(socket);
      break;
    case STA_T:
      statsSlave(socket);
      break;
    case CGI_T:
      cgiSlave(socket, request->_target, request->_query);
      break;
    case MOD_T:
      modSlave(socket, request->_target, request->_query);
      break;
  }
  delete request_s;
  swiper(request);
  /*
  // set env after every request
  envWriter(request.c_str());
  printf("%s\n", request.c_str());
  // check if it is CGI request
  if (modJudger(request)) {
  return modSlave(socket, request.c_str());
  }
  if (cgiJudge(request)) {
  return cgiSlave(socket, request_c_str());
  }
   */
}
