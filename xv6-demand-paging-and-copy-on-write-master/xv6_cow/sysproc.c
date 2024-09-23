#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int sys_fork(void)
{
  return fork();
}

int sys_exit(void)
{
  exit();
  return 0; // not reached
}

int sys_wait(void)
{
  return wait();
}

int sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getpid(void)
{
  return myproc()->pid;
}

int sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

int sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
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
int sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Adding functionality to date system call
int sys_date(void)
{
  struct rtcdate *r;

  // Taking Argument pointer of rtcdate
  if (argptr(0, (void *)&r, sizeof(r)) < 0)
    return -1; // If the argument is invalid

  // Using cmostime() to store the data in pointer
  cmostime(r);
  return 0; // Return 0 if successful
}

int sys_pgtPrint(void)
{
  // Entry address for page directory
  pde_t *pgdir_entry = myproc()->pgdir;

  for (int i = 0; i < 1024; i++)
  {
    // Checking if entry in page directory is valid and accessible to user
    if ((pgdir_entry[i] & PTE_P) && (pgdir_entry[i] & PTE_U))
    {
      // Entry address to corresponding page table of page directory entry
      pte_t *pgtab_entry = (pte_t *)P2V(PTE_ADDR(pgdir_entry[i]));

      for (int j = 0; j < 1024; j++)
        // Checking is entry in page table is valid and user accessible
        if ((pgtab_entry[j] & PTE_P) && (pgtab_entry[j] & PTE_U))
          // Printing logical and physical addresses
          cprintf("Pgdir entry num:%d,Pgt entry number: %d, Virtual address: 0x%x, Physical address: 0x%x, W-bit: %d\n", i, i * 1024 + j, i * 1024 * PGSIZE + j * PGSIZE, PTE_ADDR(pgtab_entry[j]), ((pgtab_entry[j] & PTE_W) / 2));
    }
  }

  return 0;
}
