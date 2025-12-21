/* 
 *Giorgos Vitsos csd5369
 *proc_info_user.h
*/

#ifndef _PROC_INFO_USER_H
#define _PROC_INFO_USER_H

#include <unistd.h>
#include <sys/syscall.h>

#define NR_set_proc_info 341
#define NR_get_proc_info 342

struct d_params {
    int deadline;     
    int est_runtime;  
};

static inline long set_proc_info(int deadline, int est_runtime) {
    return syscall(NR_set_proc_info, deadline, est_runtime);
}

static inline long get_proc_info(struct d_params *params) {
    return syscall(NR_get_proc_info, params);
}

#endif 