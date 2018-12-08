#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "functions.hh"
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <signal.h>
#include <math.h>
#include <sys/mman.h>
#include <fcntl.h>
int main(int argc, char ** argv) {
  std::string s = "thisis a string";
  int i = s.find("a s");
  printf("%d\n", i);
  return 0;
}
