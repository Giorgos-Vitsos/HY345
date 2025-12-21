/* 
 * Giorgos Vitsos csd5369
 * demo_step8.c
 */

#include <stdio.h>
#include "proc_info_user.h"
#include <unistd.h>   // for sleep
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("--- HY-345 Step 8 Demo Program ---\n");

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Child 1
        set_proc_info(5, 2000);  // 5s deadline, 2000ms est
        while (1); // busy loop to consume CPU
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Child 2
        set_proc_info(5, 1000);  // 5s deadline, 1000ms est
        while (1); // busy loop
    }

    pid_t pid3 = fork();
    if (pid3 == 0) {
        // Child 3
        set_proc_info(5, 3000);  // 5s deadline, 3000ms est
        while (1); // busy loop
    }

    // Parent waits a bit to let LTF scheduling happen
    sleep(3);

    // Kill children
    kill(pid1, 9);
    kill(pid2, 9);
    kill(pid3, 9);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);

    printf("--- Demo Finished ---\n");
    return 0;
}
