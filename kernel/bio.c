// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKET 13
struct hashbucket{
  struct spinlock lock;
  struct buf head;
};

struct {
  struct buf buf[NBUF];
  struct hashbucket buckets[NBUCKET];
} bcache;

char bcachename[10];

void
binit(void)
{
  struct buf *b;

  for(int i = 0; i < NBUCKET; i++){
    memset(bcachename, 0, sizeof bcachename);
    snprintf(bcachename,sizeof bcachename,"bache%02d", i);
    initlock(&bcache.buckets[i].lock, bcachename);
    bcache.buckets[i].head.prev = &bcache.buckets[i].head;
    bcache.buckets[i].head.next = &bcache.buckets[i].head;
  }

  for(b = bcache.buf; b < bcache.buf + NBUF; b++){
    int bid = (b - bcache.buf) % NBUCKET;
    b->next = bcache.buckets[bid].head.next;
    b->prev = &bcache.buckets[bid].head;
    initsleeplock(&b->lock, "buffer");
    bcache.buckets[bid].head.next->prev = b;
    bcache.buckets[bid].head.next = b;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  int bid = blockno % NBUCKET;
  acquire(&bcache.buckets[bid].lock);

  for(b = bcache.buckets[bid].head.next; b != &bcache.buckets[bid].head; b = b->next){
       if(b->dev == dev && b->blockno == blockno){
          b->refcnt++;


          acquire(&tickslock);
          b->timestamp = ticks;
          release(&tickslock);

          release(&bcache.buckets[bid].lock);
          acquiresleep(&b->lock);
          return b;
       }
  }

  struct buf * t;
  b = 0;
  // 从当前桶中开始查找
  for(int i = 0; i < NBUCKET; i++){
    int nbid = (bid + i) % NBUCKET;

    if(nbid != bid){
      if(holding(&bcache.buckets[nbid].lock))
        continue;
      
      acquire(&bcache.buckets[nbid].lock);
    }
    // 找到桶中refcnt == 0的buf中time_stamp最小的
    for(t = bcache.buckets[nbid].head.next; t != &bcache.buckets[nbid].head; t = t->next){
      if(t->refcnt == 0 && (b == 0 || t->timestamp < b->timestamp ))
        b = t;
    }
    if(!b)
    {
      if(nbid != bid)
        release(&bcache.buckets[nbid].lock);
      continue;
    }
    
    if(nbid != bid){
      b->next->prev = b->prev;
      b->prev->next = b->next;
      release(&bcache.buckets[nbid].lock);

      b->next = bcache.buckets[bid].head.next;
      bcache.buckets[bid].head.next->prev = b;
      b->prev = &bcache.buckets[bid].head;
      bcache.buckets[bid].head.next = b;
    }

    b->dev = dev;
    b->blockno = blockno;
    b->valid = 0;
    b->refcnt = 1;

    acquire(&tickslock);
    b->timestamp = ticks;
    release(&tickslock);
    
    release(&bcache.buckets[bid].lock);
    acquiresleep(&b->lock);
    return b;
  }

  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  int bid = b->blockno % NBUCKET;

  acquire(&bcache.buckets[bid].lock);
  b->refcnt--;

  acquire(&tickslock);
  b->timestamp = ticks;
  release(&tickslock);

  release(&bcache.buckets[bid].lock);
}

void
bpin(struct buf *b) {
  int bid = b->blockno % NBUCKET;
  acquire(&bcache.buckets[bid].lock);
  b->refcnt++;
  release(&bcache.buckets[bid].lock);
}

void
bunpin(struct buf *b) {
  int bid = b->blockno % NBUCKET;
  acquire(&bcache.buckets[bid].lock);
  b->refcnt--;
  release(&bcache.buckets[bid].lock);
}


