#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


//추가
int
sys_forknexec(void)
{
    char *path, *args[MAXARG];
    int i;
    uint uargv, uarg;

    if (argstr(0, &path) < 0 || argint(1, (int *)&uargv) < 0)
    {
        return -1;
    }
    memset(args, 0, sizeof(args));
    for (i = 0;; i++)
    {
        if (i >= NELEM(args))
            return -1;
        if (fetchint(uargv + 4 * i, (int *)&uarg) < 0)
            return -1;
        if (uarg == 0)
        {
            args[i] = 0;
            break;
        }
        if (fetchstr(uarg, &args[i]) < 0)
            return -1;
    }
    return forknexec((const char *)path, (const char **)args);
}


int
sys_set_proc_priority(void){
  int pid, priority;
  if (argint(0, &pid) < 0 || argint(1, &priority) < 0){
    return -1;
  }
  return set_proc_priority(pid, priority);
}
  
int
sys_get_proc_priority(void){
  int pid;
  if(argint(0, &pid) < 0){
    return -1;
  }
  return get_proc_priority(pid);
}
