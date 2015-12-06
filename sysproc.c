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
  return proc->pid;
}

// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int alloc)
{
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
  if(*pde & PTE_P){
    pgtab = (pte_t*)p2v(PTE_ADDR(*pde));
  } else {
    if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
      return 0;
    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table
    // entries, if necessary.
    *pde = v2p(pgtab) | PTE_P | PTE_W | PTE_U;
  }
  return &pgtab[PTX(va)];
}

int
sys_showmapping(void)
{
  int low;
  int high;
  if (argint(0, &low) < 0 || argint(1, &high) < 0)
    return -1;
  if (low != PGROUNDDOWN(low) || high != PGROUNDUP(high) || low > high) {
    cprintf("showmapping: Invalid address.\n");
    return -1;
  }
  pte_t *pte;
  while (low < high) {
    pte = walkpgdir(proc->pgdir, (void *)low, 0);
    cprintf("0x%x\t", low);
    if (!pte || !(*pte && (*pte)&PTE_P)) {
      cprintf("No mapping.\n");
    }
    else {
      cprintf("0x%x\t", PTE_ADDR(*pte));
      if ((*pte)&PTE_U) cprintf("permission: user\t");
      else cprintf("permission: kernel\t");
      if ((*pte)&PTE_W) cprintf("read/write\n");
      else cprintf("read only.\n");
    }
    low += PGSIZE;
  }
  return 0;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
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
    if(proc->killed){
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

int
sys_halt(void)
{
  outw(0xB004, 0x2000);
  return 0;
}

int
sys_setpriority(void) {
  int priority;
  if (argint(0, &priority) < 0)
    return -1;
  proc->priority = priority;
  return 0;
}
