#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/page.h>
#include <linux/types.h>
#include <linux/mm_types.h>
#include <linux/jiffies.h>

static unsigned long timer_interval_ns = 10e9; // Call function every 10 seconds.
static struct hrtimer hr_timer;
static int exit = 0;

struct task_struct *task;

pgd_t *pgd;
p4d_t *p4d;
pmd_t *pmd;
pud_t *pud;
pte_t *ptep, pte;

static int pid = 1;
unsigned long RSS = 0, SWAP = 0, WSS = 0;

// Module parameter
module_param(pid, int, S_IRUSR); 

int ptep_test_and_clear_young(struct vm_area_struct *vma, unsigned long addr, pte_t *ptep)
{
    int ret = 0;
    if (pte_young(*ptep))
        ret = test_and_clear_bit(_PAGE_BIT_ACCESSED, (unsigned long *) &ptep->pte);
    return ret;
}



