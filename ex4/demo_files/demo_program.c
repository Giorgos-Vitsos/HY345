/* Giorgos Vitsos csd5369
 * demo_program.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "proc_info_user.h"

void do_spinning(int id, unsigned long iterations) {
    volatile unsigned long i;
    printf("[Child %d] Starting spinning...\n", id);
    for (i = 0; i < iterations; i++) {
        //simultates work
    }
    printf("[Child %d] Finished work normally.\n", id);
}

int main() {
    printf("--- LTF Multi-Process Priority Test ---\n");

    //Child 1
    if (fork() == 0) {
        //Low Priority
        int deadline = 20; 
        int runtime = 8000; 
        set_proc_info(deadline, runtime);
        printf("[Child 1] PID: %d, D: %ds, E: %dms\n", getpid(), deadline, runtime);
        
        do_spinning(1, 4000000000ULL);
        exit(0);
    }

    sleep(1);

    //Child 2
    if (fork() == 0) {
        //High peirity
        int deadline = 20; 
        int runtime = 2000; 
        set_proc_info(deadline, runtime);
        printf("[Child 2] PID: %d, D: %ds, E: %dms\n", getpid(), deadline, runtime);
        
        do_spinning(2, 4000000000ULL);
        exit(0);
    }

    wait(NULL);
    wait(NULL);
    printf("--- Main Demo Finished ---\n");
    return 0;
}