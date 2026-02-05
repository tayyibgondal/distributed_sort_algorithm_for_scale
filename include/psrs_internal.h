#ifndef PSRS_INTERNAL_H
#define PSRS_INTERNAL_H

#include <pthread.h>

// thread control block sturcture (TCB)
struct ThreadControlBlock {
    int id;
    int* local_array;  // pointer to this threads portion of data
    int local_size;
    int* samples;      // samples for phase 2
};

// Global Variables (shared across files)
extern int* global_arr;
extern int global_size;
extern int num_threads;
extern pthread_t* thread_ids;
extern struct ThreadControlBlock* TCB;
extern pthread_barrier_t barrier;

// pivots and partition stuff
extern int* pivots;
extern int num_pivots;

// Partition arrays ,each thread makes p partitions
extern int*** partitions;
extern int** partition_sizes;

// final merged arrays for each thread 
extern int** final_arrays;
extern int* final_sizes;

// phase timing (for benchmarking)
extern double phase1_time;
extern double phase2_time;
extern double phase3_time;
extern double phase4_time;

// barrier macro
#define BARRIER pthread_barrier_wait(&barrier)

// Phase functions
void phase1_local_sort(int thread_id);
void phase2_select_pivots(int thread_id);
void phase3_partition(int thread_id);
void phase4_merge(int thread_id);

// SPMD main
void* spmd_main(void* arg);

// comparision function
int compare_ints(const void* a, const void* b);

#endif
