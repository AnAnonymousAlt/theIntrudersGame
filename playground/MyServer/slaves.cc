#include <string>
#include "slaves.hh"
#include <vector>
#include <unistd.h>
#include "tools.hh"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "dir_functions.hh"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <math.h>
#include <dlfcn.h>
//#include "myhttpd.hh"
void logSlave(int socket) {
  int fd = open(logfile, O_RDONLY);
  struct stat sb;
  fstat(fd, &sb);
  int size = sb.st_size;
  char * d = new char[size];
  char * mem = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
  strcpy(d, mem);
  const char * header = "HTTP/1.1 200 OK\r\nServer: CS 252 lab5\r\nContent-type: text/plain\r\n\r\n";
  write(socket, header, strlen(header));
  write(socket, d, size);
  munmap(mem, size);
  close(fd);
  return;
}

void statsSlave(int socket) {
  std::string plain_header =
    "HTTP/1.1 200 Document follows\r\nServer: CS 252 lab5\r\nContent-type: text/plain\r\n\r\n";
  std::string name = "NAME: Xiaoyan Li\n";
  std::string login = "LOGIN: li2185\n";
  std::string number_request = "NUMBER OF REQUEST: ";
  std::string elapse_time = "TIME ELAPSE SINCE SERVER STARTED: ";

  long r = (long)time(NULL) - clock_begin;
  elapse_time += std::to_string(r);
  elapse_time += " seconds\n";
  number_request += std::to_string(request_counter);
  number_request += "\n";
  std::string max_time = "MAX REQUEST TIME: "
    + std::to_string((double)time_max / pow(10.0, 9.0)) + " second\n";
  std::string min_time = "MIN REQUEST TIME: "
    + std::to_string((double)time_min / pow(10.0, 9.0)) + " second\n";
  std::string document = name + login + number_request + elapse_time + max_time + min_time;
  write(socket, plain_header.c_str(), plain_header.size());
  write(socket, document.c_str(), document.size());
  return;
}
extern std::vector<std::string> module_list;
extern std::vector<void *> op_list;
void modSlave(int socket, char * file, char * query) {
  void * op;
  std::string request(file);
  if (module_list.size() > 0) {
    bool exist = false;
    int i = 0;
    for(std::vector<std::string>::iterator it = module_list.begin(); it < module_list.end(); ++it, ++i) {
      if (*it == request) {
        exist = true;
        op = op_list[i];
        break;
      }
    }
    if (!exist) {
      op = dlopen(request.c_str(), RTLD_LAZY);
      module_list.push_back(request);
      op_list.push_back(op);
    }
  }
  else {
    op = dlopen(request.c_str(), RTLD_LAZY);
    module_list.push_back(request);
    op_list.push_back(op);
  }
  void (*httprun)(int socket, char * query);
  *(void**)(&httprun) = dlsym(op, std::string("httprun").c_str());
  if (httprun) {
    const char * header = "HTTP/1.1 200 Document follows\r\nServer: CS 252 lab5\r\n";
    write(socket, header, strlen(header));
    httprun(socket, query);
  }
  return;
}


void cgiSlave(int socket, char * file, char * query) {
  // fork child process
  int pid = fork();
  if (pid == 0) {
    // child
    // set env REQUEST_METHOD=GET
    setenv((const char *)"REQUEST_METHOD", (const char*)"GET",1);
    if (query != NULL) {
      setenv((const char *)"QUERY_STRING", query, 1);
    }
    // redirect output
    dup2(socket, 1);
    printf("HTTP/1.1 200 OK\r\nServer: CS 252 lab5\r\n");
    fflush(stdout);
    // char ** argv = (char**)malloc(sizeof(char*));
    // argv[0] = strdup(file);
    execv(file, NULL);
  }
  return;
}


void directorySlave(int socket, char * dir, char * query) {
  std::string mail =
    "HTTP/1.1 200 Document follows\r\n"
    "Server: CS 252 lab5\r\n"
    "Content-type: text/html\r\n"
    "\r\n";
  char * tmpHTML = dirHTMLMaker(dir, query);
  write(socket, mail.c_str(),mail.size());
  write(socket, tmpHTML, strlen(tmpHTML));
  delete tmpHTML;
}

void fileSlave(int socket, char * file, TargetType target_type) {
  char * header = headerMaker(target_type);
  char * content;
  int size = fileReader(file, target_type, &content);
  printf("\n================|| response ||===================\n\n%s\n%s"
      , header, content);
  printf("\n=================================================\n\n");
  write(socket, header, strlen(header));
  write(socket, content, size);
  delete header;
  delete content;
  return;
}
