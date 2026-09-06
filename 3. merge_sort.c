

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


static void merge(int *arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));
    if (!L || !R) {
        fprintf(stderr, "Fatal: malloc failed in merge\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}


static void merge_sort(int *arr, int left, int right) {
    if (left >= right) return; 
    int mid = left + (right - left) / 2;
    merge_sort(arr, left, mid);
    merge_sort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

static void print_array(const int *arr, int n) {
    printf("[ ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("]\n");
}

int main(void) {
    
    int sample[] = {38, 27, 43, 3, 9, 82, 10};
    int n = (int)(sizeof(sample) / sizeof(sample[0]));

    printf("Original array:\n");
    print_array(sample, n);

    merge_sort(sample, 0, n - 1);

    printf("Sorted array:\n");
    print_array(sample, n);

    
    int big_n = 20000;
    int *big = malloc(big_n * sizeof(int));
    if (!big) return EXIT_FAILURE;

    srand((unsigned)time(NULL));
    for (int i = 0; i < big_n; i++) big[i] = rand() % 1000000;

    clock_t start = clock();
    merge_sort(big, 0, big_n - 1);
    clock_t end = clock();

    
    int sorted_ok = 1;
    for (int i = 1; i < big_n; i++) {
        if (big[i - 1] > big[i]) { sorted_ok = 0; break; }
    }

    printf("\nSorted %d random integers in %.4f seconds. Verified sorted: %s\n",
           big_n, (double)(end - start) / CLOCKS_PER_SEC,
           sorted_ok ? "yes" : "NO (bug!)");

    free(big);
    return 0;
}
