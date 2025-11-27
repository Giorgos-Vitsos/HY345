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

    test_call(5, 4000, "1.Valid|5,4000");
    test_call(5, 5001, "2.Invalid|5001,5");
    test_call(-1, 1000, "3.Invalid|-1,1000");
    test_call(10, 0, "4.Invalid|10,0");

    return 0;
}