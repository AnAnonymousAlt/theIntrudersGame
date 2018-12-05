#include <string>
std::string dirSlave(std::string dir);
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
