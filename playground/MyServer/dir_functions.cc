#include "dir_functions.hh"
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string>
struct table{
  std::string name;
  std::string time;
  std::string size;
  std::string line;
  table(std::string n, std::string t, std::string s, std::string l) {
    name = n;
    time = t;
    size = s;
    line = l;
  }
};

enum SortType {NA, ND, MA, MD, SA, SD, DA, DD};
// compare different string variable depending on SortType
bool compareSlave(struct table t1, struct table t2, SortType st) {
  if (st == NA) {
    return t1.name.compare(t2.name) > 0;
  }
  else if (st == ND) {
    return t1.name.compare(t2.name) < 0;
  }
  else if (st == MA) {
    return t1.time.compare(t2.time) > 0;
  }
  else if (st == MD) {
    return t1.time.compare(t2.time) < 0;
  }
  else if (st == SA) {
    printf("%s\n", t1.size.c_str());
    return t1.size.compare(t2.size) > 0;
  }
  else if (st == SD) {
    printf("%s\n", t1.size.c_str());
    return t1.size.compare(t2.size) < 0;
  }
  return false;
}
// simple insertion sort using compareSlave
void sortSlave(std::vector<struct table>& v, SortType st) {
  int i = 1;
  while (i < v.size()) {
    int j = i;
    while (j > 0) {
      if (compareSlave(v.at(j-1), v.at(j), st)) {
        struct table tmp = v.at(j);
        v.at(j) = v.at(j-1);
        v.at(j-1) = tmp;
      }
      j--;
    }
    i++;
  }
}
// get time string of a file
std::string timeSlave(std::string file) {
  // inintializing statbuffer
  struct stat sb;
  if (stat(file.c_str(), &sb))
    return "????";
  // reformating time
  struct tm * tmb = localtime(&sb.st_mtim.tv_sec);
  // making string
  std::stringstream ss;
  ss << tmb->tm_year + 1900 << "-";
  ss << (tmb->tm_mon<10?"0":"") << tmb->tm_mon << "-";
  ss << (tmb->tm_mday<10?"0":"") << tmb->tm_mday << " ";
  ss << (tmb->tm_hour<10?"0":"") << tmb->tm_hour << ":";
  ss << (tmb->tm_min<10?"0":"") << tmb->tm_min << ":";
  ss << (tmb->tm_sec<10?"0":"") << tmb->tm_sec;
  return ss.str();
}
// get size string of a file
std::string sizeSlave(std::string file) {
  // initial stat
  struct stat sb;
  if(stat(file.c_str(), &sb))
    return "????";
  double size = sb.st_size;
  // reformat size
  int counter = 0; // record unit
  while (size / 1000.0 > 1) {
    counter++;
    size = size / 1000.0;
  }
  // make string
  std::stringstream ss;
  ss.precision(1);
  ss << size;
  // get unit
  switch (counter) {
    case 0:
      ss << "B";
      break;
    case 1:
      ss << "KB";
      break;
    case 2:
      ss << "MB";
      break;
    case 3:
      ss << "GB";
      break;
  }
  return ss.str();

}
// read and save file stat
struct table tableSlave(struct dirent * file_d, std::string dir_s) {
  std::string part1 = "<tr><td valign=\"top\"><img src=\"";
  std::string part2 = "\" alt=\"[";
  std::string part3 = "]\"></td><td><a href=\"";
  std::string delim = "\">";
  std::string part4 = "</a></td><td align=\"right\"> $T</td><td align=\"right\"> $S</td><td>&nbsp;</td></tr>\n";
  std::string line;
  std::string img = "/icons/unknown.gif";
  std::string type = "   ";
  std::string file(file_d->d_name);
  //file = 
  // time
  size_t time_pos = part4.find("$T");
  part4.erase(time_pos, 2);
  std::string time_s = timeSlave(dir_s + file);
  part4.insert(time_pos, time_s);
  // size
  size_t size_pos = part4.find("$S");
  part4.erase(size_pos, 2);
  std::string size_s = sizeSlave(dir_s + file);
  if (file_d->d_type != DT_DIR)
    size_s = sizeSlave(dir_s + file);
  else  size_s = "-";
  part4.insert(size_pos, size_s);

  // edit dir name
  if (file_d->d_type == DT_DIR) {
    file = file +"/";
  }
  /* HTML can automatically understand ../ or ./ */
  line = part1 + img + part2 + type + part3 + file + delim + file + part4;
  struct table t(file, time_s, size_s, line);
  return t;
}

// get sort content of a directory
char * dirHTMLMaker(char * d_s, char * q) {
  std::string dir_s(d_s);
  SortType st;
  // prepare SortType
  if (q != NULL) {
    std::string sort_request(q);
    std::size_t C_i = sort_request.find("C=");
    char C = sort_request.at(C_i + 2);
    std::size_t O_i = sort_request.find("O=");
    char O = sort_request.at(O_i + 2);
    if (C == 'N' && O == 'A') {
      st = NA;
    }
    else if (C == 'N' && O == 'D') {
      st = ND;
    }
    else if (C == 'M' && O == 'A') {
      st = MA;
    }
    else if (C == 'M' && O == 'D') {
      st = MD;
    }
    else if (C == 'S' && O == 'A') {
      st = SA;
    }
    else if (C == 'S' && O == 'D') {
      st = SD;
    }
    else if (C == 'D' && O == 'A') {
      st == DA;
    }
    else if (C == 'D' && O == 'D') {
      st = DD;
    }
  }
  else st = NA;
  // open directory
  DIR * dir = opendir(dir_s.c_str());
  // prepare HTML
  std::string header = 
    "<html>\n"
    " <head>\n"
    "  <title>HOME</title>\n"
    " </head>\n"
    " <body>\n"
    "<h1>Index of</h1>\n"
    "<table>\n";
  std::string sort = 
    "<tr>"
    "<th valign=\"top\"><img src=\"/icons/index.gif\" alt=\"[ICO]\"></th>"
    "<th><a href=\"?C=N;O=D\">Name</a></th>"
    "<th><a href=\"?C=M;O=A\">Last modified</a></th>"
    "<th><a href=\"?C=S;O=A\">Size</a></th>"
    "<th><a href=\"?C=D;O=A\">Description</a></th>"
    "</tr>\n"
    "   <tr><th colspan=\"5\"><hr></th></tr>\n";
  std::string ending =
    "   <tr><th colspan=\"5\"><hr></th></tr>\n"
    "</table>\n"
    "</body></html>\n";
  // edit SORT header for next click
  std::string st_needle = "?C=";
  std::string st_substitute;
  switch(st) {
    case NA:
      st_needle += "N";
      st_substitute = "D";
      break;
    case ND:
      st_needle += "N";
      st_substitute = "A";
      break;
    case MA:
      st_needle += "M";
      st_substitute = "D";
      break;
    case MD:
      st_needle += "M";
      st_substitute = "A";
      break;
    case SA:
      st_needle += "S";
      st_substitute = "D";
      break;
    case SD:
      st_needle += "S";
      st_substitute = "A";
      break;
  }
  size_t st_pos = sort.find(st_needle);
  st_pos += std::string("?C=M;O=").size();
  sort[st_pos] = st_substitute[0];
  // prepare HTML
  std::string html = header + sort;
  // read dir
  struct dirent * dr= readdir(dir);
  // get all child in the parent directory
  // store in vector to sort
  std::vector<struct table> v;
  while (dr != NULL) {
    struct table t = tableSlave(dr, dir_s);
    v.push_back(t);
    dr = readdir(dir);
  }
  // pass the whole table to sort function
  sortSlave(v, st);
  // fill the table to HTML string
  for (std::vector<struct table>::iterator it = v.begin(); it != v.end(); ++it) {
    html += it->line;
  }
  html += ending;
  char * c = new char[html.size() + 1];
  int i;
  for (i = 0; i < html.size(); i++) {
    c[i] = html.at(i);
  }
  c[i] = '\0';
  return c;
}

