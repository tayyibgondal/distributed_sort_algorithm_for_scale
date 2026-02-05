#include <stdio.h>
#include <stdlib.h>

int comparison(const void* a, const void * b) {
    int value_a, value_b;
    value_a = *(int*)a;
    value_b = *(int*)b;
    return value_a - value_b;
}

int* sort(int*arr, int sizeofarray) {
    // takes in array as pointer
    // returns array pointer
    int oneElementSize = sizeof(arr[0]);
    qsort(arr, sizeofarray, oneElementSize, comparison);
    return arr;
 
}
