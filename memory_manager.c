#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/page.h>
#include <linux/types.h>
#include <linux/mm_types.h>

unsigned long timer_interval_ns = 10e9; // Call function every 10 seconds.
static struct hrtimer hr_timer;
struct task_struct *task;

pgd_t *pgd;
p4d_t *p4d;
pmd_t *pmd;
pud_t *pud;
pte_t *ptep, pte;

static int pid;
int ii;
unsigned long RRS = 0, SWAP = 0, WSS = 0;

// Module parameter
module_param(pid, int, S_IRUSR); 

