    /*
    *Giorgos Vitsos csd5369
    *get_proc_info.c
    */

    #include <linux/kernel.h>
    #include <linux/sched.h>
    #include <linux/uaccess.h>
    #include <linux/proc_info.h>

    asmlinkage long sys_get_proc_info(struct d_params __user *params){

        struct d_params k_params;
        struct task_struct *task=current;

        printk("Giorgos Vitsos csd5369, get_proc_info\n");

        //check for null
        if(params== NULL){
            printk("Params pointer is NULL\n");
            return -EINVAL;
        }

        //we copy the values
        k_params.deadline = task->deadline;
        k_params.est_runtime = task->est_runtime;

        if(copy_to_user(params, &k_params, sizeof(struct d_params))){
            return -EFAULT;
        }

        return 0;
    }