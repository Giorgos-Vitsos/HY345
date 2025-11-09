#include "restaurant.h"

int main(int argc, char* argv[]){

    if(argc !=4){

        printf("Wrong usage.\n");
        return 1;

    }

    int num_tables=atoi(argv[1]);
    int num_groups=atoi(argv[2]);
    int table_capacity=atoi(argv[3]);

    init_restaurant(num_tables,num_groups,table_capacity);

    destroy_restaurant();
    return 0;
}

