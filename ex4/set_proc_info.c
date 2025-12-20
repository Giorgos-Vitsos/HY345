/*
 *Giorgos Vitsos csd5369
 *set_proc_info.c
 */

#include <linux/sched.h>       // Πλήρης ορισμός task_struct
#include <linux/kernel.h>      // Βασικές συναρτήσεις
#include <linux/linkage.h>     // Για την asmlinkage
#include <linux/proc_info.h>   // Για τη struct d_params (αν χρειαστεί, αν και όχι άμεσα εδώ)
#include <linux/errno.h>       // Για -EINVAL
#include <asm/current.h>

asmlinkage long sys_set_proc_info(int deadline, int est_runtime){

    printk("Giorgos Vitsos csd5369, set_proc_info\n");
    struct task_struct *task=current;

    /* Check for valid arguments: deadline and est_runtime must be positive */
    if (deadline<= 0 || est_runtime<= 0){
        printk("Invalid arguments (Must be positive)\n");
        return -EINVAL;
    }

    /* Check if est_runtime (ms) can be met within deadline (s) */
    /* Conversion to long is crucial to prevent integer overflow during multiplication */
    if (deadline*1000<est_runtime){
        return -EINVAL;
    }

    /* Set the values in the current process's task_struct */
    task->deadline = deadline;
    task->est_runtime = est_runtime;

    /* Set the values for assignment4*/
    task->ltf_flag=1;
    task->time_used=0;
    task->last_sched=jiffies;

    return 0;
}