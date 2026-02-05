#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "pthread_barrier.h"
#include "psrs_internal.h"
#include "sort.h"

// phase timing variables (for benchmarking breakdown)
double phase1_time = 0.0;
double phase2_time = 0.0;
double phase3_time = 0.0;
double phase4_time = 0.0;

//global vasrs for psrs
int* global_arr = NULL;
int global_size = 0;
int num_threads = 4;  
pthread_t* thread_ids = NULL;
struct ThreadControlBlock* TCB = NULL;
pthread_barrier_t barrier;

// pivots and partition (phase 2 stuff)
int* pivots = NULL;
int num_pivots = 0;

// partition arrays (phase 3 stuff)
// note: i put these out of tcb strct as in phase 3, all threds need to access other thread partions. so this is not 'strictly' thread related
int*** partitions = NULL;  // partitions[thread][partition][elements]
int** partition_sizes = NULL;  // sizes of each partition
// final merged arrays for each thread  (phase 4 stuff)
int** final_arrays = NULL; // 2d cz threads, partitions
int* final_sizes = NULL; // size of all rows in final arrasys

// Main PSRS function 
int* psrs(int* arr, int sizeofarray) {
    // setup the barrier
    pthread_barrier_init(&barrier, NULL, num_threads);
    global_arr = arr;
    global_size = sizeofarray;
    
    // allocate thread control blocks and IDs
    TCB = (struct ThreadControlBlock*)malloc(num_threads * sizeof(struct ThreadControlBlock));
    thread_ids = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    // allocate partition arrays
    partitions = (int***)malloc(num_threads * sizeof(int**));
    partition_sizes = (int**)malloc(num_threads * sizeof(int*));
    // allocate final output arrays
    final_arrays = (int**)malloc(num_threads * sizeof(int*));
    final_sizes = (int*)malloc(num_threads * sizeof(int));
    
    //start threads 1 to p-1 (main thread will be thread 0)
    for(int i = 1; i < num_threads; i++) {
        TCB[i].id = i;
        pthread_create(&thread_ids[i], NULL, spmd_main, (void*)&TCB[i]);
    }
    
    // main thread acts as thread 0
    TCB[0].id = 0;
    spmd_main((void*)&TCB[0]);
    
    // wait for all other threads to complete
    for(int i = 1; i < num_threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }
    
    // copy final sorted data back into original array
    int position = 0;
    for(int t = 0; t < num_threads; t++) {
        for(int i = 0; i < final_sizes[t]; i++) {
            arr[position++] = final_arrays[t][i];
        }
    }
    
    // free all memory (cleanup)
    for(int t = 0; t < num_threads; t++) {
        if(TCB[t].samples) free(TCB[t].samples);
        for(int p = 0; p < num_threads; p++) {
            if(partitions[t][p]) free(partitions[t][p]);
        }
        if(partitions[t]) free(partitions[t]);
        if(partition_sizes[t]) free(partition_sizes[t]);
        if(final_arrays[t]) free(final_arrays[t]);
    }
    
    free(TCB);
    free(thread_ids);
    free(partitions);
    free(partition_sizes);
    free(final_arrays);
    free(final_sizes);
    if(pivots) free(pivots);
    pthread_barrier_destroy(&barrier);
    
    return arr;
}

// function to set number of threads (callable from main)
void set_num_threads(int p) {
    num_threads = p;
}

// function to get phase times (for benchmarking)
void get_phase_times(double* p1, double* p2, double* p3, double* p4) {
    *p1 = phase1_time;
    *p2 = phase2_time;
    *p3 = phase3_time;
    *p4 = phase4_time;
}

// reset phase times before each run
void reset_phase_times() {
    phase1_time = 0.0;
    phase2_time = 0.0;
    phase3_time = 0.0;
    phase4_time = 0.0;
}
