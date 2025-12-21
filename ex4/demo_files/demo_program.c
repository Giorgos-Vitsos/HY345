#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int set_proc_info(unsigned long exec_time, unsigned long deadline);

void spin(void)
{
    volatile unsigned long i;
    for (i = 0; i < 1000000000UL; i++);
}

int main()
{
    pid_t p1, p2, p3;

    printf("LTF starvation demo\n");

    /* High priority LTF */
    p1 = fork();
    if (p1 == 0) {
        printf("[P1] exec=3s deadline=6s\n");
        set_proc_info(3000, 6000);
        spin();
        return 0;
    }

    /* Slightly worse T */
    p2 = fork();
    if (p2 == 0) {
        printf("[P2] exec=4s deadline=12s\n");
        set_proc_info(4000, 12000);
        spin();
        return 0;
    }

    /* Starved process */
    p3 = fork();
    if (p3 == 0) {
        printf("[P3] exec=10s deadline=100s\n");
        set_proc_info(10000, 100000);
        spin();
        return 0;
    }

    wait(NULL);
    wait(NULL);
    wait(NULL);
    return 0;
}
