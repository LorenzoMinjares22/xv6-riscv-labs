#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"




int
main (int argc, char *argv[])

{

  int time = uptime();
  printf("up %d %s\n", time, "clocks");


  exit(0);

}