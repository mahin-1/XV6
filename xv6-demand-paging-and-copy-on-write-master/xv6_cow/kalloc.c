// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run
{
  struct run *next;
};

// added an array refcount which keeps count of each process associated with physical address
struct
{
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
  uint ref_count[PHYSTOP >> PGSHIFT];
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void freerange(void *vstart, void *vend)
{
  char *p;
  p = (char *)PGROUNDUP((uint)vstart);
  for (; p + PGSIZE <= (char *)vend; p += PGSIZE)
  {
    kmem.ref_count[V2P(p) >> PGSHIFT] = 0;
    kfree(p);
  }
}
// PAGEBREAK: 21
//  Free the page of physical memory pointed at by v,
//  which normally should have been returned by a
//  call to kalloc().  (The exception is when
//  initializing the allocator; see kinit above.)
void kfree(char *v)
{
  struct run *r;

  if ((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  if (kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run *)v;
  uint index = V2P(v) >> PGSHIFT;
  // Reduces reference count of v
  if (kmem.ref_count[index] > 0)
    kmem.ref_count[index]--;

  if (kmem.ref_count[index] == 0)
  {
    // Fill with junk to catch dangling refs.
    memset(v, 1, PGSIZE);
    r->next = kmem.freelist;
    kmem.freelist = r;
  }
  if (kmem.use_lock)
    release(&kmem.lock);
  return;
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char *
kalloc(void)
{
  struct run *r;

  if (kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;

  if (r)
    kmem.freelist = r->next;
  if (kmem.use_lock)
    release(&kmem.lock);
  return (char *)r;
}

// decreasing reference count of pa frame
void dec_ref_count(uint pa)
{
  // invalid decrement
  if (pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("decrease_reference_count");

  acquire(&kmem.lock);
  kmem.ref_count[pa >> PGSHIFT]--;
  release(&kmem.lock);
}

// increasing reference count of pa frame
void inc_ref_count(uint pa)
{
  // invalid increment
  if (pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("increase_reference_count");

  acquire(&kmem.lock);
  kmem.ref_count[pa >> PGSHIFT]++;
  release(&kmem.lock);
}
// Geeting reference count of physical address pa
uint ref_count(uint pa)
{
  // invalid access
  if (pa < (uint)V2P(end) || pa >= PHYSTOP)
    panic("get_reference_count");
  uint count;

  acquire(&kmem.lock);
  count = kmem.ref_count[pa >> PGSHIFT];
  release(&kmem.lock);
  // cprintf("in kalloc count:%d\n", count);
  return count;
}