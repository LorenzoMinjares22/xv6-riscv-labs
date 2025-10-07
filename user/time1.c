#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pstat.h"  // for struct rusage

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: time1 command [args...]\n");
    exit(1);
  }

  int start = uptime();  // record start ticks
  int pid = fork();

  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    //child
    exec(argv[1], argv + 1);
    fprintf(2, "time1: exec %s failed\n", argv[1]);
    exit(1);
  }

  // Parent  wait for child using wait2()
  int status;
  struct rusage ru;

  wait2(&status, &ru); // new syscall that gives CPU time

  int end = uptime();  // record end ticks

  int elapsed = end - start;
  int cpu = ru.cputime;
  int percent = (elapsed == 0) ? 0 : (cpu *  100) / elapsed;

  printf("elapsed time: %d ticks, cpu time: %d ticks, %d%% CPU\n",
         elapsed, cpu, percent);

  exit(0);
}
