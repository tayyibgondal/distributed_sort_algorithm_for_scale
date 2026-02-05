#ifndef SORT_H
#define SORT_H

int* sort(int*arr, int sizeofarray);
int* psrs(int* arr, int sizeofarray);
void set_num_threads(int p);
int compare_ints(const void* a, const void* b);

// phase timing functions (for benchmarking)
void get_phase_times(double* p1, double* p2, double* p3, double* p4);
void reset_phase_times();

#endif