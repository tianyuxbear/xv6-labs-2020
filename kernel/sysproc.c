#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "fcntl.h"
#include "file.h"

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

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
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
sys_mmap(void)
{
  uint64 addr;
  int length, prot, flags, fd, offset;
  struct file * fp;

  if(argaddr(0, &addr) < 0 || argint(1, &length) < 0 || argint(2, &prot) < 0 )
    return -1;
  if(argint(3, &flags) < 0 || argint(4, &fd) < 0 || argint(5, &offset) < 0)
    return -1;

  struct proc * p = myproc();
  if(addr != 0 || offset != 0 || length <= 0)
    return -1;
  if(fd < 0 || fd >= NOFILE)
    return -1;
  if((fp = p->ofile[fd]) == 0)
    return -1;
  
  if(fp->writable == 0 && (prot & PROT_WRITE) && (flags & MAP_SHARED))
    return -1;

  uint64 va;
  va = PGROUNDUP(p->sz);
  for(int i = 0; i < NVMA; i++){
    if(p->vmas[i].addr == 0){
      p->vmas[i].addr = va;
      p->vmas[i].length = length;
      p->vmas[i].prot = prot;
      p->vmas[i].flags = flags;
      p->vmas[i].fd = fd;
      p->vmas[i].offset = offset;
      p->vmas[i].fp = fp;
      break;
    }
  }
  filedup(fp);
  p->sz = va + length;
  return va;
}

uint64
sys_munmap(void)
{
  uint64 va;
  int length;
  
  if(argaddr(0,&va) < 0 || argint(1, &length) < 0)
    return -1;

  struct proc *p = myproc();
  int i ;
  for(i = 0; i < NVMA; i++){
    if(p->vmas[i].addr && p->vmas[i].length >= length){
      if(va == p->vmas[i].addr){
        p->vmas[i].addr += length;
        p->vmas[i].length -= length;
        break;
      }
      if(va + length == p->vmas[i].addr + p->vmas[i].length){
        p->vmas[i].length -= length;
        break;
      }

    }
  }
  if(i == NVMA)
    return -1;

  if((p->vmas[i].prot & PROT_WRITE) && (p->vmas[i].flags & MAP_SHARED)){
    // if(filewrite(p->vmas[i].fp, va, length) < 0)
    //   return -1;
    filewrite(p->vmas[i].fp, va, length);
  }

  uvmunmap(p->pagetable,va,length/PGSIZE,1);

  if(p->vmas[i].length == 0){
    fileclose(p->vmas[i].fp);
    p->vmas[i].addr = 0;
  }

  return 0;
}
