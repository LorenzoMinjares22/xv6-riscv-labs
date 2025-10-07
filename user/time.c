#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pstat.h"   // struct rusage

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: time1 command [args...]\n");
    exit(1);
  }

  int start = uptime();          // wall-clock (ticks) before running child
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "time1: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // child: run the requested program (with its args)
    exec(argv[1], argv + 1);
    fprintf(2, "time1: exec %s failed\n", argv[1]);
    exit(1);
  }

  // parent  wait2 for child 
  int status = -1;
  struct rusage ru;
  if (wait2(&status, &ru) < 0) {
    fprintf(2, "time1: wait2 failed\n");
    exit(1);
  }

  int end = uptime();            //all time affter child
  int elapsed = end - start;
  int cpu = ru.cputime;
  int percent = (elapsed == 0) ? 0 : (cpu * 100) / elapsed;
  if (percent > 100) percent = 100;  // safety cap

  printf("elapsed time: %d ticks, cpu time: %d ticks, %d%% CPU\n",
         elapsed, cpu, percent);
  exit(0);
}
