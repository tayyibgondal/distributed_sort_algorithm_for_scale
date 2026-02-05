// benchmarking code for psrs
// runs experiments and logs results to text files

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "sort.h"
#define TOTAL_RUNS 7
#define RUNS_TO_AVG 5

// struct to hold all timing info
struct RunResultPsrs {
    double total_time;
    double p1_time;
    double p2_time;
    double p3_time;
    double p4_time;
};

int* make_random_array(int size) {
    int* arr = (int*)malloc(size * sizeof(int));
    for(int i = 0; i < size; i++) {
        arr[i] = random();
    }
    return arr;
}

int is_sorted(int* arr, int n) {
    for(int i = 0; i < n-1; i++) {
        if(arr[i] > arr[i+1]) return 0;
    }
    return 1;
}

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

struct RunResultPsrs run_parallel(int n, int p) {
    double times[TOTAL_RUNS];
    double p1_times[TOTAL_RUNS];
    double p2_times[TOTAL_RUNS];
    double p3_times[TOTAL_RUNS];
    double p4_times[TOTAL_RUNS];
    
    // do 7 runs
    for(int run = 0; run < TOTAL_RUNS; run++) {
        srandom(67);
        int* arr = make_random_array(n);
        
        // reset phase times before run
        reset_phase_times();
        
        // set threads and run psrs
        set_num_threads(p);
        double start = get_time();
        psrs(arr, n);
        double end = get_time();
        
        times[run] = end - start;
        
        // get phase times
        get_phase_times(&p1_times[run], &p2_times[run], &p3_times[run], &p4_times[run]);
        
        // verify its sorted (only first run)
        if(run == 0) {
            if(!is_sorted(arr, n)) {
                printf("error :(");
            }
        }
        
        free(arr);
    }
    
    // average last 5 runs
    struct RunResultPsrs result = {0, 0, 0, 0, 0};
    for(int i = TOTAL_RUNS - RUNS_TO_AVG; i < TOTAL_RUNS; i++) {
        result.total_time += times[i];
        result.p1_time += p1_times[i];
        result.p2_time += p2_times[i];
        result.p3_time += p3_times[i];
        result.p4_time += p4_times[i];
    }
    result.total_time /= RUNS_TO_AVG;
    result.p1_time /= RUNS_TO_AVG;
    result.p2_time /= RUNS_TO_AVG;
    result.p3_time /= RUNS_TO_AVG;
    result.p4_time /= RUNS_TO_AVG;
    
    return result;
}

// run sequential qsort for comparison
double run_sequential(int n) {
    double times[TOTAL_RUNS];
    
    for(int run = 0; run < TOTAL_RUNS; run++) {
        srandom(67);
        int* arr = make_random_array(n);
        
        double start = get_time();
        // use qsort directly for sequential baseline
        qsort(arr, n, sizeof(int), compare_ints);
        double end = get_time();
        
        times[run] = end - start;
        
        free(arr);
    }
    
    // avg last 5
    double sum = 0;
    for(int i = TOTAL_RUNS - RUNS_TO_AVG; i < TOTAL_RUNS; i++) {
        sum += times[i];
    }
    return sum / RUNS_TO_AVG;
}

int main() {
    // a.
    int sizes[] = {32000000, 48000000, 64000000, 96000000, 128000000, 164000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    int threads[] = {2, 4, 8, 12, 16, 32, 64, 128};
    int num_threads_to_test = sizeof(threads) / sizeof(threads[0]);
    FILE* time_file = fopen("logs/results_time.txt", "w");
    FILE* speedup_file = fopen("logs/results_speedup.txt", "w");
    FILE* phase_file = fopen("logs/results_phases.txt", "w");
    if(!time_file || !speedup_file || !phase_file) {
        printf("Error opening output files!\n");
        return 1;
    }
    
    // b.
    // write headers
    fprintf(time_file, "n,sequential");
    fprintf(speedup_file, "n");
    for(int t = 0; t < num_threads_to_test; t++) {
        fprintf(time_file, ",p%d", threads[t]);
        fprintf(speedup_file, ",p%d", threads[t]);
    }
    fprintf(time_file, "\n");
    fprintf(speedup_file, "\n");
    fprintf(phase_file, "n,p,total,phase1,phase2,phase3,phase4\n");
    
    // c.
    // run experiments for each array size
    for(int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        printf("testing n = %d\n", n);
        
        // 1. first get sequential time
        printf(" running sequential qsort...\n");
        double seq_time = run_sequential(n);
        printf("  Sequential time: %.4f sec\n", seq_time);
        
        // store times for this size
        double* parallel_times = (double*)malloc(num_threads_to_test * sizeof(double));
        
        // 2. run for each thread count
        for(int t = 0; t < num_threads_to_test; t++) {
            int p = threads[t];
            printf("  Running PSRS with p=%d...\n", p);
            
            struct RunResultPsrs result = run_parallel(n, p);
            parallel_times[t] = result.total_time;
            
            printf("  Time: %.4f sec, Speedup: %.2fx\n", 
                   result.total_time, seq_time / result.total_time);
            printf("    Phase breakdown: P1=%.4f P2=%.4f P3=%.4f P4=%.4f\n",
                   result.p1_time, result.p2_time, result.p3_time, result.p4_time);
            
            // write phase breakdown to file
            fprintf(phase_file, "%d,%d,%.4f,%.4f,%.4f,%.4f,%.4f\n",
                    n, p, result.total_time, 
                    result.p1_time, result.p2_time, result.p3_time, result.p4_time);
        }
        
        // 3. write log fiels
        fprintf(time_file, "%d,%.6f", n, seq_time);
        for(int t = 0; t < num_threads_to_test; t++) {
            fprintf(time_file, ",%.6f", parallel_times[t]);
        }
        fprintf(time_file, "\n");
        fprintf(speedup_file, "%d", n);
        for(int t = 0; t < num_threads_to_test; t++) {
            fprintf(speedup_file, ",%.4f", seq_time / parallel_times[t]);
        }
        fprintf(speedup_file, "\n");
        
        free(parallel_times);
        printf("\n");
    }
    
    fclose(time_file);
    fclose(speedup_file);
    fclose(phase_file);
    
    printf("Done! Resutsl saved");
    
    return 0;
}
