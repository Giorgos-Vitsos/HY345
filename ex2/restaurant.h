#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct{

    int table_capacity;
    int current_customers;

} Table;

typedef struct{

    int size;
    int group_id;
    int table_id;
    sem_t g_sem; 

} Group;

typedef struct{

    Table* tables;
    int num_tables;

    Group* groups;
    int num_groups;
    
    int* waiting_groups;
    pthread_mutex_t mutex;
    sem_t w_sem;
    int remaining_groups;

} Restaurant;

void init_restaurant(int num_tables,int num_groups, int table_capacity);
void destroy_restaurant();

void *waiter_function(void *ptr);
void *group_function(void *ptr);
void free_table(Group* group);