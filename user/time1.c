#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: time1 command [args...]\n");
    exit(1);
  }

  int start = uptime();   // record time before running the command
  int pid = fork();

  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // child
    exec(argv[1], argv + 1);
    fprintf(2, "time1: exec %s failed\n", argv[1]);
    exit(1);
  }

  // parent wait for child to finish
  wait(0);

  int end = uptime();     // record time after child finishes
  int elapsed = end - start;

  printf("elapsed time: %d ticks\n", elapsed);
  exit(0);
}
