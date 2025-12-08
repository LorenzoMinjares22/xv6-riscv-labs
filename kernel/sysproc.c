#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;
  struct proc *p = myproc();

  if(argint(0, &n) < 0)
    return -1;

  addr = p->sz;
  if (n == 0)
    return addr;

  uint64 new_sz = addr + n;
  if(new_sz < p->sz){
    return (uint64)-1;
  }
  p->sz = new_sz;
  /*old eager allocatoin, we don't call growproc right away for lazy allocatoin*/
  /*if(growproc(n) < 0)
    return -1;*/
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_freepmem(void)
{
  uint64 pages = kfreepages_count();
  return pages * PGSIZE;
}

                                                                                   
// Helper reads sem_t from user memory
static int
fetch_sem_index(int argn, int *idx)
{
  uint64 uaddr;
  struct proc *p = myproc();

  if (argaddr(argn, &uaddr) < 0)
    return -1;

  if (copyin(p->pagetable, (char *)idx, uaddr, sizeof(int)) < 0)
    return -1;

  if (*idx < 0 || *idx >= NSEM)
    return -1;

  return 0;
}


uint64
sys_sem_init(void)
{
  uint64 uaddr;   // user address of sem_t
  int pshared;
  int value;
  int idx;
  struct proc *p = myproc();
  int ksem;      // sem_t is typedef int

  // get arguments
  if (argaddr(0, &uaddr) < 0 || argint(1, &pshared) < 0 || argint(2, &value) < 0)
    return -1;

  if (value < 0)
    return -1;

  // allocate a semaphore slot
  idx = semalloc();
  if (idx < 0)
    return -1;

  // initialize the semaphore's count
  struct semaphore *s = &semtable.sem[idx];
  acquire(&s->lock);
  s->count = value;
  release(&s->lock);


  ksem = idx;
  if (copyout(p->pagetable, uaddr, (char *)&ksem, sizeof(ksem)) < 0) {
    semdealloc(idx);
    return -1;
  }

  
  return 0;
}


uint64
sys_sem_destroy(void)
{
  int idx;

  // read sem index from user space
  if (fetch_sem_index(0, &idx) < 0)
    return -1;

  // mark it free in global table
  semdealloc(idx);
  return 0;
}

uint64
sys_sem_wait(void)
{
  int idx;

  if (fetch_sem_index(0, &idx) < 0)
    return -1;

  struct semaphore *s = &semtable.sem[idx];

  acquire(&s->lock);

  if (s->valid == 0) {
    release(&s->lock);
    return -1;
  }

  // wait until count > 0 sleeping while it is 0
  while (s->count == 0) {
    // sleep on the semaphore struc releases s->lock while sleeping
    sleep(s, &s->lock);
    // when woken up s->lock is re-acquired
    if (s->valid == 0) {
      // destroyed while sleeping
      release(&s->lock);
      return -1;
    }
  }


  s->count--;
  release(&s->lock);
  return 0;
}


uint64
sys_sem_post(void)
{
  int idx;

  if (fetch_sem_index(0, &idx) < 0)
    return -1;

  struct semaphore *s = &semtable.sem[idx];

  acquire(&s->lock);

  if (s->valid == 0) {
    release(&s->lock);
    return -1;
  }

  // increment count and wake s
  s->count++;
  wakeup(s);   

  release(&s->lock);
  return 0;
}
            