#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

int main(int argc, char **argv)
{
    struct pstat uproc[NPROC];
    int nprocs;
    int i;
    char *state;
    static char *states[] = {
        [SLEEPING_C] "sleeping",
        [RUNNABLE_C] "runnable",
        [RUNNING_C] "running ",
        [ZOMBIE_C] "zombie  "};

    nprocs = getprocs(uproc);
    if (nprocs < 0)
        exit(-1);

    printf("pid\tstate\t\tsize\tppid\tname\tpriority\n");
    for (i = 0; i < nprocs; i++)
    {
        state = states[uproc[i].state];
        printf("%d\t%s\t%l\t%d\t%s\t%d\n", uproc[i].pid, state,
               uproc[i].size, uproc[i].ppid, uproc[i].name, uproc[i].priority);
    }

    exit(0);
}