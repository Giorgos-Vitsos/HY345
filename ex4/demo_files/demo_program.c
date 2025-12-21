#include <stdio.h>
#include <unistd.h>
#include "proc_info_user.h"

int main() {
    long result;
    struct d_params retrieved_params;

    printf("--- Step 8 LTF Test Demo ---\n");

    // Register a long-running LTF task
    result = set_proc_info(5, 1000); // 5 s deadline, 1000 ms estimated runtime
    if (result != 0) {
        printf("[ERROR] set_proc_info failed\n");
        return 1;
    }

    // Busy loop to simulate CPU usage
    for (volatile long i = 0; i < 1000000000; i++);

    // Optional: check parameters after busy work
    result = get_proc_info(&retrieved_params);
    if (result == 0) {
        printf("[INFO]: Deadline=%d s, Est_Runtime=%d ms\n",
               retrieved_params.deadline, retrieved_params.est_runtime);
    } else {
        printf("[ERROR] get_proc_info failed\n");
    }

    return 0;
}
