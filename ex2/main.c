#include "restaurant.h"

int main(int argc, char *argv[]) {

    if(argc!=4){//error check

        printf("Wrong usage!");
        return 1;

    }

    int num_tables=atoi(argv[1]);
    int num_groups=atoi(argv[2]);
    int table_capacity=atoi(argv[3]);

    srand(time(NULL));

    init_restaurant(num_tables, num_groups, table_capacity);//we open the restaurant

    pthread_t waiter;//we hire a waiter
    pthread_create(&waiter,NULL,waiter_function,NULL);

    pthread_t *group_threads=(pthread_t*)malloc(num_groups * sizeof(pthread_t));//we create groups
    int seen[num_groups];//to avoid duplicates

    for(int i=0; i<num_groups; i++){

        seen[i]=0;

    }

    for(int i=0; i<num_groups; i++){//for all groups

        int id;

        do{//we get a unique random id (for random arrival)

            id=rand()%num_groups;

        }while(seen[id]==1);

        seen[id]=1;
        int random_time=rand()%3+1;//random arrival time
        sleep(random_time); // Stagger group arrivals
        printf("[GROUP %d] Arrived with %d people (after %d sec)\n",id,restaurant.groups[id].size,random_time);
        pthread_create(&group_threads[i],NULL,group_function,(void*)(long)id);

    }

    
    for(int i= 0; i<num_groups; i++){//we wait for all to finish eating

        pthread_join(group_threads[i], NULL);

    }

    destroy_restaurant();//clean up
    free(group_threads);

    return 0;
}
