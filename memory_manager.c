#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/sched/mm.h>
#include <linux/sched/signal.h>
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
    struct mm_struct *mm;
    struct vm_area_struct *vma;
    unsigned long size, address;
    pgd_t *pgd;
    p4d_t *p4d;
    pmd_t *pmd;
    pud_t *pud;
    pte_t *ptep, pte;
    
    for_each_process(task)
    {
        if (task->pid == pid && task != NULL) 
        {
            mm = task->mm;
            vma = mm->mmap;
            // Loop through each vma
            while( vma != NULL ){
                // Check each page
                for( address = vma->vm_start; address <= vma->vm_end; address += PAGE_SIZE ){
                    pgd = pgd_offset(mm, address);
                    if( pgd_none(*pgd) || pgd_bad(*pgd) )
                        return 0;
                    
                    p4d = p4d_offset(pgd, address);
                    if( p4d_none(*p4d) || p4d_bad(*p4d) )
                        return 0;
                    
                    pud = pud_offset(p4d, address);
                    if( pud_none(*pud) || pud_bad(*pud) )
                        return 0;
                    
                    pmd = pmd_offset(pud, address);
                    if( pmd_none(*pmd) || pmd_bad(*pmd) )
                        return 0;
                    
                    ptep = pte_offset_map(pmd, address);
                    if( !ptep )
                        return 0;
                    pte = *ptep;
                    
                    if(pte_present(pte))
                        RSS++;
                    else
                        SWAP++;
                }
                vma = vma->vm_next;
            }
            
        }
    }
    
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
    ktime_t ktime;
    ktime = ktime_set(0, timer_interval_ns);
    hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    hr_timer.function = &no_restart_callback;
    hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
    return 0;
}

void memory_manager_exit(void)
{
    int ret = hrtimer_cancel(&hr_timer);
}

module_init(memory_manager_init);
module_exit(memory_manager_exit);
MODULE_LICENSE("GPL");
