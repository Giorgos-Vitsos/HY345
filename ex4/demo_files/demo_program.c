/* Giorgos Vitsos csd5369
 * demo_program.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "proc_info_user.h"

void do_heavy_spinning(int id) {

    volatile unsigned long i;
    printf("[Child %d, PID %d] starts...\n", id, getpid());

    for (i = 0; i < 8000000000ULL; i++) {
        //simulate work
    }

    printf("[Child %d] end...\n", id);
}

int main() {
    printf("--- LTF Test ---\n");

    //Child1 low pritority
    if (fork() == 0) {
        set_proc_info(30, 10000); 
        do_heavy_spinning(1);
        exit(0);
    }

    sleep(1); 

    //Child2 high priority
    if (fork() == 0) {
        set_proc_info(30, 3000);
        do_heavy_spinning(2);
        exit(0);
    }

    sleep(1);

    //Child 3 doomed
    if (fork() == 0) {
        set_proc_info(1, 5000); 
        do_heavy_spinning(3);
        exit(0);
    }


    for(int i=0; i<3; i++){
        wait(NULL);
    } 
    
    printf("--- Demo Finished ---\n");
    return 0;
}