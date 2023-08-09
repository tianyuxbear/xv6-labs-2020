// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.
struct {
  struct spinlock reflock;
  int refcount[MAXPAGES];
} pageref;

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&pageref.reflock,"pageref");
  for(int i = 0; i < MAXPAGES; i++)
    pageref.refcount[i] = 1;
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  int refc = sub_refc((uint64)pa);
  if(refc < 0){
    printf("refc: %d pa: %p\n",refc, pa);
    panic("kfree");
  }
  else if(refc > 0){
    return;
  }

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){ 
    kmem.freelist = r->next;
    pageref.refcount[PA2IND((uint64)r)] = 1;
  }
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  
  return (void*)r;
}

int get_refc(uint64 pa){
  return pageref.refcount[PA2IND(pa)];
}
int add_refc(uint64 pa){
  int refc;
  acquire(&pageref.reflock);
  pageref.refcount[PA2IND(pa)]++;
  refc = pageref.refcount[PA2IND(pa)];
  release(&pageref.reflock);
  return refc;
}
int sub_refc(uint64 pa){
  int refc;
  acquire(&pageref.reflock);
  pageref.refcount[PA2IND(pa)]--;
  refc = pageref.refcount[PA2IND(pa)];
  release(&pageref.reflock);
  return refc;
}