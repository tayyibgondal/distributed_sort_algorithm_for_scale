// citation: https://www.geeksforgeeks.org/dsa/merge-k-sorted-arrays/ (for some part of logic in phase 4)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pthread_barrier.h"
#include "psrs_internal.h"

// phase 1: each thread sorts its local portion
void phase1_local_sort(int thread_id) {
    // calculate which part of array belongs to this thread
    int chunk_size = global_size / num_threads;  // 10 elems per thread=30 size/3 threads
    int start = thread_id * chunk_size; // could be 0, 10, 20 for this mind example
    int end; // could be 30, 10, 20 (this is inflated by 1)
    // last thread gets any remaining elements
    if(thread_id == num_threads - 1) {
        end = global_size;
    } else {
        end = start + chunk_size;
    }
    int local_n = end - start;
    
    // sort my chunk using quicksort
    qsort(&global_arr[start], local_n, sizeof(int), compare_ints);
    
    // save pointer and size to TCB
    TCB[thread_id].local_array = &global_arr[start];
    TCB[thread_id].local_size = local_n;
}

//phase 2, pick pivots to partition data
void phase2_select_pivots(int thread_id) {
    // a. each thread takes p samples from its sorted portion
    int local_n = TCB[thread_id].local_size;
    TCB[thread_id].samples = (int*)malloc(num_threads * sizeof(int));
    // (regular sampling)
    for(int i = 0; i < num_threads; i++) {
        // formula for regular sampling: divide array into equal parts
        int sample_index = (i * local_n) / num_threads;  // its values will be 0 , 3, 9  for p1 (local sample idices for all threads)
        // make sure we dont go out of bounds
        if(sample_index >= local_n) {
            sample_index = local_n - 1;
        }
        TCB[thread_id].samples[i] = TCB[thread_id].local_array[sample_index];
    }
    
    BARRIER; 
    
    // b. master does pivot selection
    if(thread_id == 0) {
        // 1.gather all the samples from all threads
        int total_samples = num_threads * num_threads;
        int* all_samples = (int*)malloc(total_samples * sizeof(int));
        int index = 0;
        for(int t = 0; t < num_threads; t++) {
            for(int s = 0; s < num_threads; s++) {
                all_samples[index] = TCB[t].samples[s];
                index++;
            }
        }
        
        // 2.sort all samples together
        qsort(all_samples, total_samples, sizeof(int), compare_ints);
        
        // 3.choose p-1 pivots (evenly spaced)
        num_pivots = num_threads - 1;
        pivots = (int*)malloc(num_pivots * sizeof(int));
        
        for(int i = 0; i < num_pivots; i++) {
            // skip 'num_threads' samples between each pivot
            int position = (i + 1) * num_threads;
            
            // make sure we don't go past the end (this logic sometimes picks one more pivot at the end i.e., last sample, that might not be very helpful. but as in psrs paper, if oversampling is not of order n/p, its fine)
            if(position >= total_samples) {
                position = total_samples - 1;
            }
            
            // pick the pivot value at that position
            pivots[i] = all_samples[position];
        }
        
        free(all_samples);
    }
    
    BARRIER;  // wait for master to finish
}

// phase3, partition the data according to pivots
void phase3_partition(int thread_id) {
    int local_n = TCB[thread_id].local_size;
    int* local_data = TCB[thread_id].local_array;
    
    // a. allocate p partitions for this thread
    partitions[thread_id] = (int**)malloc(num_threads * sizeof(int*));
    partition_sizes[thread_id] = (int*)malloc(num_threads * sizeof(int));
    // alocate memory for each partition 
    // worst case all elements go into 1 partition so allocate local_n for each ...
    for(int p = 0; p < num_threads; p++) {
        partitions[thread_id][p] = (int*)malloc(local_n * sizeof(int));
        partition_sizes[thread_id][p] = 0;
    }
    
    // b. partition the sorted data using the pivots (for each thread)
    for(int i = 0; i < local_n; i++) {
        int value = local_data[i];
        int partition_num = 0;
        // figure out which partition this element goes into
        for(int p = 0; p < num_pivots; p++) {
            if(value > pivots[p]) {
                partition_num++;
            } else {
                break;  // found the right partition
            }
        }
        // put value into correct partition
        int idx = partition_sizes[thread_id][partition_num];
        partitions[thread_id][partition_num][idx] = value;
        partition_sizes[thread_id][partition_num]++;
    }
    
    BARRIER;
}

// phase 4, each thread merges partitions assigned to it
void phase4_merge(int thread_id) {
    // thread i gets partition i from all p threads and merges them
    // calcuate total elements this thread will handle
    int total_size = 0;
    for(int t = 0; t < num_threads; t++) {
        total_size += partition_sizes[t][thread_id];
    } 
    final_sizes[thread_id] = total_size;
    final_arrays[thread_id] = (int*)malloc(total_size * sizeof(int));
    


    // merge all the partitions together   (i took help from this source. citation: https://www.geeksforgeeks.org/dsa/merge-k-sorted-arrays/)
    // we need an array to track our position in each partition
    int* position = (int*)calloc(num_threads, sizeof(int));
    // keep adding elements to output until we've merged everything
    for(int i = 0; i < total_size; i++) {
        // step 1: find the smallest element among all partitions
        int min_value = 0;
        int from_which_thread = -1;
        
        // look at each thread's partition
        for(int t = 0; t < num_threads; t++) {
            // check if this partition still has elements
            int size = partition_sizes[t][thread_id];
            int pos = position[t];
            
            if(pos < size) {
                // get the current element from this partition
                int current_element = partitions[t][thread_id][pos];
                
                // is this the first element we're looking at?
                if(from_which_thread == -1) {
                    min_value = current_element;
                    from_which_thread = t;
                }
                // or is this element smaller than what we found before?
                else if(current_element < min_value) {
                    min_value = current_element;
                    from_which_thread = t;
                }
            }
        }
        // step 2: add the smallest element to output
        if(from_which_thread != -1) {
            final_arrays[thread_id][i] = min_value;
            position[from_which_thread]++;  // move forward in that partition
        }
    }
    
    free(position);
    
    BARRIER;
}
