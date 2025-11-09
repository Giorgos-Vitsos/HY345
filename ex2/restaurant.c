#include "restaurant.h"

Restaurant restaurant;

void init_restaurant(int num_tables, int num_groups, int table_capacity){

    restaurant.num_tables=num_tables;
    restaurant.tables=(Table*)malloc(num_tables * sizeof(Table));

    for(int i=0; i<num_tables; i++){

        restaurant.tables[i].table_capacity=table_capacity;
        restaurant.tables[i].current_customers=0;
        restaurant.tables[i].table_id=i;

    }

    restaurant.num_groups=num_groups;
    restaurant.groups = (Group*)malloc(num_groups * sizeof(Group));

    for(int i=0; i<num_groups; i++){

        int random_size=rand()%(table_capacity)+1;
        restaurant.groups[i].size=random_size;
        restaurant.groups[i].group_id=i;
        restaurant.groups[i].table_id=-1;

        sem_init(&restaurant.groups[i].g_sem, 0, 0);

    }

    restaurant.waiting_groups = (int*)malloc(num_groups * sizeof(int));
    for(int i=0; i<num_groups; i++){

        restaurant.waiting_groups[i]=-1;

    }
    sem_init(&restaurant.w_sem, 0, 0);
    pthread_mutex_init(&restaurant.mutex, NULL);
    restaurant.remaining_groups=num_groups;
}

void destroy_restaurant() {
    // Implementation of restaurant cleanup
}

void *group_function(void *ptr){

    int id=*(int*) ptr;
    int time=1; //for testing purposes
    Group* group=&restaurant.groups[id];//we get the currect group based on the id

    printf("[GROUP %d] Arrived with %d people (after %d sec)\n",group->group_id,group->size,time);//group arrives

    //waiting
    pthread_mutex_lock(&restaurant.mutex);
    restaurant.waiting_groups[group->group_id]=group->group_id;//we add it to the queue
    sem_post(&restaurant.w_sem);//we tell the waiter to wake up
    pthread_mutex_unlock(&restaurant.mutex);

    sem_wait(&group->g_sem);//the group waits

    printf("[GROUP %d] Seated at Table %d with %d people\n",group->group_id,group->table_id,group->size);

    //eating
    int sleeping_time=rand()%(15-5+1)+5;
    printf("[GROUP %d] Eating (%d people) for %d seconds...\n",group->group_id,group->size,sleeping_time);
    sleep(sleeping_time);

    //leaving
    pthread_mutex_lock(&restaurant.mutex);
    free_table(group);//we free the table
    pthread_mutex_unlock(&restaurant.mutex);

    return NULL;
}

void free_table(Group* group){

    int table_id=group->table_id;
    restaurant.tables[table_id].current_customers=restaurant.tables[table_id].current_customers-group->size;//we remove the group from the table
    group->table_id=-1;
    restaurant.remaining_groups--;//we remove the group from the restaurant
    printf("[GROUP %d] Left Table %d (%d/%d occupied)\n",group->group_id,table_id,restaurant.tables[table_id].current_customers,restaurant.tables[table_id].table_capacity);
    sem_post(&restaurant.w_sem);

}

void *waiter_function(void *ptr){
    while(1){

        sem_wait(&restaurant.w_sem);
        
        if(restaurant.remaining_groups==0){

            break;

        }

        
        pthread_mutex_lock(&restaurant.mutex);

        int found=0;
        for(int i=0; i<restaurant.num_groups; i++){

            if(restaurant.waiting_groups[i]==-1){

                continue;

            }
            Group* group=&restaurant.groups[i];
            for(int j=0; j<restaurant.num_tables; j++){

                Table* table=&restaurant.tables[j];
                if(table->table_capacity-table->current_customers>=group->size){

                    seat_at_table(group,table);
                    found=1;
                    break;
                }
            }
            if(found==1){

                sem_post(&group->g_sem);
                break;
            }
        }

        pthread_mutex_unlock(&restaurant.mutex);
        
    }
    return NULL;

}

void seat_at_table(Group* group,Table* table){

    table->current_customers=table->current_customers+group->size;
    group->table_id=table->table_id;
    restaurant.waiting_groups[group->group_id]=-1;


}
