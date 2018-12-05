#include "tools.hh"
#include <string>
#include <fstream>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
void swiper(struct Request * request) {
  delete request->_request_type;
  delete request->_target;
  delete request->_auth;
  delete request->_query;
  delete request->_HTTP_accept;
  delete request->_host;
  delete request->_user;
  delete request->_referer;
  delete request->_content_type;
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
  request->_secure = false;
  delete request;
  request = NULL;
}

char * headerMaker (TargetType target_type) {
  std::string header =
    "HTTP/1.1 200 Document follows\r\n"
    "Server: CS 252 lab5\r\n"
    "Content-type: ";
  switch (target_type) {
    case GIF_T:
      header = header + "image/gif\r\n\r\n";
      break;
    case BMP_T:
      header = header + "image/bmp\r\n\r\n";
      break;
    case ICO_T:
      header = header + "image/x-icon\r\n\r\n";
      break;
    case JPG_T:
      header = header + "image/jpeg\r\n\r\n";
      break;
    case PNG_T:
      header = header + "image/png\r\n\r\n";
      break;
    case SVG_T:
      header = header + "image/svg+xml\r\n\r\n";
      break;
    case HTM_T:
    case CSS_T:
    case IDX_T:
      header = header + "text/html\r\n\r\n";
      break;
    default:
      header = header + "text/plain\r\n\r\n";
  }
  char * r = new char[header.size()+1];
  int i;
  for (i = 0; i < header.size(); i++) {
    r[i] = header.at(i);
  }
  r[i] = '\0';
  return r;
}

// get all information from request_s
// (can discard request_s after calling this function)
bool requestParser(char * request_s, struct Request * r) {
  std::string request(request_s);
  int start_pos;
  int end_pos;
  int i;
  int sl;
  // find GET
  start_pos = 0; // start from the very beginning
  end_pos = request.find(" ", start_pos); // The first space is after Request Type
  sl = end_pos - start_pos;
  r->_request_type = new char[sl + 1];
  i = 0;
  while (i < sl) {
    r->_request_type[i] = request.at(i + start_pos);
    i++;
  }
  r->_request_type[i] = '\0';

  // get path and query
  start_pos = request.find(" ") + 1;
  end_pos = request.find(" " , start_pos);
  sl = end_pos - start_pos;
  char * tmp_path_info = new char[sl + 1];
  i = 0;
  while (i < sl) {
    tmp_path_info[i] = request.at(i + start_pos);
    i++;
  }
  tmp_path_info[i] = '\0';
  std::string path_info(tmp_path_info);
  delete tmp_path_info;
  // if there is query
  if (path_info.find("?") != std::string::npos) {
    // add query
    start_pos = path_info.find("?") + 1;
    end_pos = path_info.size();
    sl = end_pos - start_pos;
    r->_query = new char[sl + 1];
    i = 0;
    while (i < sl) {
      r->_query[i] = path_info.at(start_pos + i);
      i++;
    }
    r->_query[i] = '\0';
    // delete query from path info
    path_info.erase(path_info.find("?"), std::string::npos);
  }

  // if mod
  if (path_info.find(".so") != std::string::npos) {
    path_info = "http-root-dir" + path_info;
    r->_target_type = MOD_T;
  }
  // if cgi
  else if (path_info.find("cgi-bin/") != std::string::npos) {
    path_info = "http-root-dir" + path_info;
    r->_target_type = CGI_T;
  }
  // if icon
  else if (path_info.find("icons/") != std::string::npos) {
    path_info = "http-root-dir" + path_info;
    r->_target_type = ICO_T;
  }
  // if favicon
  else if (path_info.find("favicon") != std::string::npos) {
    path_info = "." + path_info;
    r->_target_type = ICO_T;
  }
  // if log
  else if (path_info.compare("/log") == 0) {
    r->_target_type = LOG_T;
  }
  // if stats
  else if (path_info.find("/stats") != std::string::npos) {
    r->_target_type = STA_T;
  }
  // if index
  else if (path_info.compare("/") == 0) {
    path_info = "http-root-dir/htdocs" + path_info + "index.html";
    r->_target_type = IDX_T;
  }
  else {
    // complete path
    path_info = "http-root-dir/htdocs" + path_info;
    // check if the file is a directory
    struct stat sb;
    int stat_return = stat(path_info.c_str(), &sb);
    // if the file is directory
    if (S_ISDIR(sb.st_mode)) {
      // complete directory
      r->_target_type = DIR_T;
    }
    else {
      // if html
      if (path_info.find("html") != std::string::npos) {
        r->_target_type = HTM_T;
      }
      // if css
      else if (path_info.find("css") != std::string::npos) {
        r->_target_type = CSS_T;
      }
      // PICS
      // ico
      else if (path_info.find(".ico") != std::string::npos) {
        r->_target_type = ICO_T;
      }
      // bmp
      else if (path_info.find(".bmp") != std::string::npos) {
        r->_target_type = BMP_T;
      }
      // bmp
      else if (path_info.find(".png") != std::string::npos) {
        r->_target_type = PNG_T;
      }
      // svg
      else if (path_info.find(".svg") != std::string::npos) {
        r->_target_type = SVG_T;
      }
      // gif
      else if (path_info.find(".gif") != std::string::npos) {
        r->_target_type = GIF_T;
      }
      // jpg
      else if (path_info.find(".jpg") != std::string::npos || path_info.find(".jpeg") != std::string::npos) {
        r->_target_type = JPG_T;
      }
      // other
      else {
        r->_target_type = TXT_T;
      }
      // set target
    }
  }
  //set target
  start_pos = 0;
  end_pos = path_info.size();
  sl = end_pos - start_pos;
  r->_target = new char[sl + 1];
  i = 0;
  while (i < sl) {
    r->_target[i] = path_info.at(start_pos + i);
    i++;
  }
  r->_target[i] = '\0';

  // find http accept
  start_pos = request.find(" ") + 1;
  start_pos = request.find(" " + start_pos) + 1;
  end_pos = request.find("\r\n", start_pos);
  sl = end_pos - start_pos;
  r->_HTTP_accept = new char[sl + 1];
  i = 0;
  while (i < sl) {
    r->_HTTP_accept[i] = request.at(i + start_pos);
    i++;
  }
  r->_HTTP_accept[i] = '\0';

  // set Authorization
  if (request.find("Authorization") != std::string::npos) {
    start_pos = request.find("Authorization: ") + std::string("Authorization: ").size(); // start position
    end_pos = request.find("\r\n", start_pos); // end position
    sl = end_pos - start_pos; // string length
    r->_auth = new char[sl + 1]; // new char string
    i = 0;
    while (i < sl) {
      r->_auth[i] = request.at(start_pos + i); // store char one by one
      i++;
    }
    r->_auth[i] = '\0'; // end of string
  }

  // find referer
  if (request.find("Referer") != std::string::npos) {
    start_pos = request.find("Referer: ") + std::string("Referer: ").size();
    end_pos = request.find("\r\n", start_pos);
    sl = end_pos - start_pos;
    r->_referer = new char[sl + 1];
    i = 0;
    while (i < sl) {
      r->_referer[i] = request.at(i + start_pos);
      i++;
    }
    r->_referer[i] = '\0';
  }
  else {
    r->_referer = new char[std::string("http://data.cs.purdue.edu:1044/").size() + 1];
    strcpy(r->_referer, std::string("http://data.cs.purdue.edu:1044/").c_str());
  }
  // find host
  if (request.find("Host:") != std::string::npos) {
    start_pos = request.find("Host:") + std::string("Host: ").size();
    end_pos = request.find("\r\n", start_pos);
    sl = end_pos - start_pos;
    r->_host = new char[sl + 1];
    i = 0;
    while (i < sl) {
      r->_host[i] = request.at(i + start_pos);
      i++;
    }
    r->_host[i] = '\0';
  }

  // find user
  std::string user = request.substr(request.find("User-Agent"), std::string::npos);
  if (request.find("User-Agent") != std::string::npos) {
    start_pos = request.find("User-Agent") + std::string("User-Agent: ").size();
    end_pos = request.find("\r\n", start_pos);
    sl = end_pos - start_pos;
    r->_user = new char[sl + 1];
    i = 0;
    while (i < sl) {
      r->_user[i] = request.at(i + start_pos);
      i++;
    }
    r->_user[i] = '\0';
  }
  // find content_type
  if (request.find("Accept: ") != std::string::npos) {
    start_pos = request.find("Accept: ") + std::string("Accept: ").size();
    end_pos = request.find("\r\n", start_pos);
    sl = end_pos - start_pos;
    r->_content_type = new char[sl + 1];
    i = 0;
    while (i < sl) {
      r->_user[i] = request.at(i + start_pos);
      i++;
    }
    r->_user[i] = '\0';
  }
  return true;
}


char * requestReader (int socket) {
  char * s = new char[1];
  char * ending = new char[3];
  std::string request;
  while (read(socket, s, 1)) {
    if(s[0] == '\000') break;
    request.push_back(s[0]);
    if (ending[0] == '\r' && ending[1] == '\n' && ending[2] == '\r' && s[0] == '\n') {
      break;
    }
    else {
      ending[0] = ending[1];
      ending[1] = ending[2];
      ending[2] = s[0];
    }
  }
  delete s;
  delete ending;
  char * r = new char[request.size() + 1];
  strcpy(r, request.c_str());
  return r;
}

void envWriter(struct Request * request) {
  if (request->_query != NULL) {
    setenv((const char*)"QUERY_STRING", request->_query, 1);
  }
  setenv((const char*)"SERVER_SOFTWARE", (const char*)"SERVER_SOFTWARE",1);
  setenv((const char*)"SERVER_NAME", (const char*)"CS 252 lab5", 1);
  setenv((const char*)"GATEWAY_INTERFACE", (const char*)"GATEWAY_INTERFACE", 1);
  setenv((const char*)"SERVER_PROTOCOL", (const char*)"SERVER_PROTCOL", 1);
  setenv((const char*)"SERVER_PORT", std::to_string(port).c_str(), 1);
  setenv((const char*)"REQUEST_METHOD", request->_request_type, 1);
  setenv((const char*)"HTTP_ACCEPT", request->_HTTP_accept, 1);
  setenv((const char*)"PATH_INFO", request->_target, 1);
  setenv((const char*)"PATH_TRANSLATED", request->_referer, 1);
  setenv((const char*)"SCRIPT_NAME", request->_target, 1);
  setenv((const char*)"REMOTE_HOST", request->_host, 1);
  setenv((const char*)"REMOTE_ADDR", (const char*)"REMOTE_ADDR", 1);
  setenv((const char*)"REMOTE_USER", request->_user, 1);
  setenv((const char*)"AUTH_TYPE", request->_auth, 1);
  setenv((const char*)"CONTENT_TYPE", std::to_string(request->_target_type).c_str(), 1);
  setenv((const char*)"CONTENT_LENGTH", (const char*)"CONTENT_LENGTH", 1);
}

// read file and store the content in r
// return size
int fileReader(char * file, TargetType target_type, char ** r) {
  std::string content;
  if (target_type == TXT_T || target_type == HTM_T || target_type == IDX_T) {
    std::ifstream ifs(file);
    std::string line;
    if (ifs.is_open()) {
      while(getline(ifs, line)) {
        content = content + line;
        content = content +  "\n";
      }
    }
    ifs.close();
    *r = new char[content.size() + 1];
    char * r_tmp = *r;
    strcpy(r_tmp, content.c_str());
    return content.size();
  }
  else {
    std::ifstream ifs(file, std::ifstream::binary);
    ifs.seekg(0, ifs.end);
    int size = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    *r = new char[size];
    ifs.read(*r, size);
    ifs.close();
    return size;
  }
}

void logWriter(const char * l) {
  int fd = open(logfile, O_RDWR);
  struct stat sb;
  fstat(fd, &sb);
  int size = sb.st_size;
  size = size + strlen(l);
  lseek(fd, size-1, 0);
  write(fd, "", 1);
  fsync(fd);
  char * mem = (char*)mmap(NULL, size + 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  strcat(mem, l);

  msync(mem, size + 1024, MS_SYNC);
  munmap (mem, size + 1024);
  close(fd);
}
