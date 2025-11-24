/*Giorgos Vitsos csd5369*/

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/current.h>

asmlinkage long sys_set_proc_info(int deadline,int est_runtime){

    struct task_struct *task=current;

    if(deadline <=0 || est_runtime<=0){
        return -EINVAL;
    }

    if(est_runtime>deadline*1000){
        return -EINVAL;
    }

    task->deadline=deadline;
    task->est_runtime=est_runtime;

    printk("Giorgos Vitsos csd5369 , sys_set_proc_info\n");

    return 0;
}

