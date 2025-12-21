/* * Giorgos Vitsos csd5369
 * demo_ass4.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "proc_info_user.h" // Periechei ta set_proc_info, get_proc_info

// Sinartisi gia na "kaiei" tin CPU
void do_heavy_work() {
    volatile unsigned long long i;
    for (i = 0; i < 0xFFFFFFFFFFFFFFFFULL; i++) {
        // Spinning... 
        // O scheduler prepei na mas skotosei otan teleiosei o xronos mas.
    }
}

int main() {
    pid_t p1, p2, p3;
    
    // Pairnoume to trechon "now" se milliseconds gia sosta deadlines
    // Simiosi: To syscall sou perimenei apolito xrono (now + duration)
    unsigned long now = (unsigned long)time(NULL) * 1000;

    printf("--- HY-345 Assignment 4 LTF Scheduler Demo ---\n");
    printf("[PARENT]: Current time reference: %lu ms\n", now);

    // --- TEST CASE 1: Normal LTF Execution (P1) ---
    p1 = fork();
    if (p1 == 0) {
        int d1 = now + 15000; // Deadline se 15 defterolepta
        int e1 = 5000;        // Xreiazetai 5 defterolepta CPU
        printf("[CHILD P1]: PID=%d | Goal: Run for 5s, Deadline in 15s\n", getpid());
        if (set_proc_info(d1, e1) != 0) {
            perror("P1 set_proc_info failed");
            exit(1);
        }
        do_heavy_work();
        exit(0);
    }

    // --- TEST CASE 2: Preemption (P2) ---
    // To P2 tha exei mikrotero T argotera kai prepei na "klepsei" tin CPU
    p2 = fork();
    if (p2 == 0) {
        usleep(500000); // Perimene ligo gia na ksekinisei proto to P1
        int d2 = now + 12000; // Deadline pio noris apo to P1
        int e2 = 3000;        // Xreiazetai ligotero xrono
        printf("[CHILD P2]: PID=%d | Goal: Run for 3s, Deadline in 12s\n", getpid());
        set_proc_info(d2, e2);
        do_heavy_work();
        exit(0);
    }

    // --- TEST CASE 3: Infeasible / Doomed (P3) ---
    // E=10s, D=5s -> T < 1. Prepei na skotothei amesos!
    p3 = fork();
    if (p3 == 0) {
        usleep(1000000); 
        int d3 = now + 5000; 
        int e3 = 10000; 
        printf("[CHILD P3]: PID=%d | Goal: DOOMED (E > D). Should be killed instantly.\n", getpid());
        set_proc_info(d3, e3);
        do_heavy_work();
        exit(0);
    }

    printf("[PARENT]: Waiting for children to be terminated by LTF Scheduler...\n");

    // Perimenoume ta paidia. Epidi o scheduler tha ta kanei SIGKILL, 
    // to wait() tha epistrepsei kanonika.
    wait(NULL);
    wait(NULL);
    wait(NULL);

    printf("\n[PARENT]: All test processes finished/killed.\n");
    printf("[ADVICE]: Check 'dmesg' or '/var/log/messages' to see LTF decisions.\n");

    return 0;
}