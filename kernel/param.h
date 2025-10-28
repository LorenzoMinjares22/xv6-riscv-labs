#define NPROC        64  // maximum number of processes
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE       1000  // size of file system in blocks
#define MAXPATH      128   // maximum file path name
#define MAXPRIO      40    // maximum amount of priority a process can have hw3
#define DEFAULT_PRIO 0   // priority defult value hw3
#define SCHED_POLICY_RR    0
#define SCHED_POLICY_PRIO  1
#define SCHED_POLICY SCHED_POLICY_PRIO
// Aging config
#define AGING_ENABLED       1       
#define AGING_INTERVAL_TCK  25         // every 25 ticks of waiting = +1 effective prio


//enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };