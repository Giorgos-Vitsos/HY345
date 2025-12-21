/* 
 *Giorgos Vitsos csd5369
 *demo_program.c
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "proc_info_user.h"


void test_call(int deadline, int runtime, const char* description){
    long result;
    struct d_params retrieved_params;

    printf("\n------Test Case: %s ------\n", description);

    printf("Calling set_proc_info(%d s, %d ms)\n", deadline, runtime);
    result = set_proc_info(deadline, runtime);

    if(result == 0){
        printf("[SUCCESS]\n");

        printf("Calling get_proc_info()\n");
        result = get_proc_info(&retrieved_params);

        if(result == 0){
            printf("[SUCCESS]\n");
            printf("[INFO]: Deadline=%d s, Est_Runtime=%d ms\n",retrieved_params.deadline, retrieved_params.est_runtime);
        }else{
            printf("[ERROR]\n");
        }
    }else{
        printf("[ERROR] set_proc_info FAILED\n");
    }
}

int main(){

    printf("--- HY-345 Exercise 3 Demo Program ---\n");

    // Test Case 1: Valid inputs (Success)
    test_call(5, 4000, "1.Valid|5,4000");

    // Test Case 2: Invalid - Runtime Exceeds Deadline (5s = 5000ms)
    test_call(5, 5001, "2.Invalid|5001,5");

    // Test Case 3: Invalid - Negative Deadline
    test_call(-1, 1000, "3.Invalid|-1,1000");

    // Test Case 4: Invalid - Zero Runtime
    test_call(10, 0, "4.Invalid|10,0");

    // Test Case 5: Get_proc_info with NULL pointer (test for EINVAL from kernel)
    printf("\n--- Test Case: 5. get_proc_info with NULL pointer ---\n");
    long result = get_proc_info(NULL);
    if (result != 0) {
        printf("   [SUCCESS] get_proc_info(NULL) CORRECTLY FAILED. Error: %s\n", strerror(errno));
    } else {
        printf("   [ERROR] get_proc_info(NULL) DID NOT FAIL (Expected -EINVAL).\n");
    }

    return 0;
}