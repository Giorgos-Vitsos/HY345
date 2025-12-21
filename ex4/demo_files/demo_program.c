#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "proc_info_user.h"
#include <unistd.h>

void spin(unsigned long count)
{
    volatile unsigned long i;
    for (i = 0; i < count; i++);
}

int main() {
    pid_t p1, p2, p3;

    printf("--- LTF Demo ---\n");

    // Short job, tight deadline
    p1 = fork();
    if (p1 == 0) {
        printf("[P1] exec=1s, deadline=2s\n");
        set_proc_info(1000, 2000); // exec=1s, deadline=2s
        spin(500000000UL);
        return 0;
    }

    // Medium job, medium deadline
    p2 = fork();
    if (p2 == 0) {
        printf("[P2] exec=2s, deadline=5s\n");
        set_proc_info(2000, 5000);
        spin(1000000000UL);
        return 0;
    }

    // Long job, long deadline (should be preempted)
    p3 = fork();
    if (p3 == 0) {
        printf("[P3] exec=5s, deadline=10s\n");
        set_proc_info(5000, 10000);
        spin(3000000000UL);
        return 0;
    }

    wait(NULL);
    wait(NULL);
    wait(NULL);

    printf("--- LTF Demo Finished ---\n");
    return 0;
}
