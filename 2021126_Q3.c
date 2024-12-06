#include <omp.h>
#include <stdio.h>

int count_occurrences(int *arr, int size, int val) {
    int occurrences = 0;

    #pragma omp parallel for reduction(+:occurrences)
    for (int i = 0; i < size; i++) {
        if (arr[i] == val)
            occurrences += 1;
    }

    return occurrences;
}

// Example usage
int main() {
    int arr[] = {1, 2, 3, 2, 2, 4, 2, 5};
    int size = sizeof(arr) / sizeof(arr[0]);
    int val_to_count = 2;

    int count = count_occurrences(arr, size, val_to_count);
    printf("Number of times %d appears: %d\n", val_to_count, count);

    return 0;
}