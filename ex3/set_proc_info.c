/*
 *Giorgos Vitsos csd5369
 *set_proc_info.c
 */

#include <linux/sched.h>       
#include <linux/kernel.h>      
#include <linux/linkage.h>     
#include <linux/proc_info.h>   
#include <linux/errno.h>       
#include <asm/current.h>

asmlinkage long sys_set_proc_info(int deadline, int est_runtime){

    printk("Giorgos Vitsos csd5369, set_proc_info\n");

    struct task_struct *task=current;

    //if negative
    if (deadline<= 0 || est_runtime<= 0){
        printk("Invalid arguments (Must be positive)\n");
        return -EINVAL;
    }

    //if not enough time
    if (deadline*1000<est_runtime){
        return -EINVAL;
    }

    //set
    task->deadline = deadline;
    task->est_runtime = est_runtime;

    return 0;
}