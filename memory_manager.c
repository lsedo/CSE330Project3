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

u64 start_time;

static unsigned long timer_interval_ns = 10e9; // Call function every 10 seconds.
static struct hrtimer hr_timer;
static int exit = 0;

struct task_struct *task;

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

int findPte(struct task_struct *task)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pmd_t *pmd;
    pud_t *pud;
    pte_t *ptep, pte;
    
    return 0;
}

static enum hrtimer_restart no_restart_callback(struct hrtimer *timer)
{
    exit++;
    ktime_t currtime, interval;
    currtime = ktime_get();
    interval = ktime_set(0, timer_interval_ns);
    hrtimer_forward(timer, currtime, interval);
    findPte(task);
    printk("%d", exit);
    return HRTIMER_RESTART;
}

int memory_manager_init(void)
{
    return 0;
}

void memory_manager_exit(void)
{
    
}

module_init(memory_manager_init);
module_exit(memory_manager_exit);
MODULE_LICENSE("GPL");
