#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "pthread_barrier.h"
#include "psrs_internal.h"

// comparision function for qsort (citation: taken from geeks for geeks: https://www.geeksforgeeks.org/c/qsort-function-in-c/)
int compare_ints(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// helper to get current time
static double get_wall_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// SPMD main - all threads execute this
void* spmd_main(void* arg) {
    struct ThreadControlBlock* my_tcb = (struct ThreadControlBlock*)arg;
    int my_id = my_tcb->id;
    double start_time, end_time;
    
    BARRIER;  // sync all threads at start
    
    if(my_id == 0) start_time = get_wall_time();
    phase1_local_sort(my_id);
    BARRIER;
    if(my_id == 0) {
        end_time = get_wall_time();
        phase1_time = end_time - start_time;
    }
    
    if(my_id == 0) start_time = get_wall_time();
    phase2_select_pivots(my_id);
    BARRIER;
    if(my_id == 0) {
        end_time = get_wall_time();
        phase2_time = end_time - start_time;
    }
    
    if(my_id == 0) start_time = get_wall_time();
    phase3_partition(my_id);
    BARRIER;
    if(my_id == 0) {
        end_time = get_wall_time();
        phase3_time = end_time - start_time;
    }
    
    if(my_id == 0) start_time = get_wall_time();
    phase4_merge(my_id);
    BARRIER;
    if(my_id == 0) {
        end_time = get_wall_time();
        phase4_time = end_time - start_time;
    }
    
    return NULL;
}
