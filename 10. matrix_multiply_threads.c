#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define ROWS_A 300
#define COLS_A 300
#define ROWS_B COLS_A
#define COLS_B 300
#define NUM_THREADS 4

static double A[ROWS_A][COLS_A];
static double B[ROWS_B][COLS_B];
static double C_threaded[ROWS_A][COLS_B];
static double C_serial[ROWS_A][COLS_B];

typedef struct {
    int thread_id;
    int start_row; 
    int end_row;   
} ThreadArgs;


static void *multiply_rows(void *arg) {
    ThreadArgs *targs = (ThreadArgs *)arg;

    for (int i = targs->start_row; i < targs->end_row; i++) {
        for (int j = 0; j < COLS_B; j++) {
            double sum = 0.0;
            for (int k = 0; k < COLS_A; k++) {
                sum += A[i][k] * B[k][j];
            }
            C_threaded[i][j] = sum;
        }
    }

    printf("Thread %d finished rows [%d, %d)\n",
           targs->thread_id, targs->start_row, targs->end_row);
    return NULL;
}

static void fill_random(double mat[][COLS_A], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mat[i][j] = (double)(rand() % 10);
        }
    }
}

static void multiply_serial(void) {
    for (int i = 0; i < ROWS_A; i++) {
        for (int j = 0; j < COLS_B; j++) {
            double sum = 0.0;
            for (int k = 0; k < COLS_A; k++) {
                sum += A[i][k] * B[k][j];
            }
            C_serial[i][j] = sum;
        }
    }
}

static int results_match(void) {
    for (int i = 0; i < ROWS_A; i++) {
        for (int j = 0; j < COLS_B; j++) {
            if (C_threaded[i][j] != C_serial[i][j]) return 0;
        }
    }
    return 1;
}

int main(void) {
    srand(42); 

    
    fill_random(A, ROWS_A, COLS_A);
    fill_random((double (*)[COLS_A])B, ROWS_B, COLS_B);

    pthread_t threads[NUM_THREADS];
    ThreadArgs args[NUM_THREADS];

    int rows_per_thread = ROWS_A / NUM_THREADS;
    int remainder = ROWS_A % NUM_THREADS;

    
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    int current_row = 0;
    for (int t = 0; t < NUM_THREADS; t++) {
        int extra = (t < remainder) ? 1 : 0; 
        args[t].thread_id = t;
        args[t].start_row = current_row;
        args[t].end_row = current_row + rows_per_thread + extra;
        current_row = args[t].end_row;

        if (pthread_create(&threads[t], NULL, multiply_rows, &args[t]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", t);
            return EXIT_FAILURE;
        }
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double threaded_seconds = (t_end.tv_sec - t_start.tv_sec) +
                               (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    multiply_serial();
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double serial_seconds = (t_end.tv_sec - t_start.tv_sec) +
                             (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    printf("\nMatrix size: %dx%d * %dx%d, %d threads\n",
           ROWS_A, COLS_A, ROWS_B, COLS_B, NUM_THREADS);
    printf("Threaded time: %.4f s\n", threaded_seconds);
    printf("Serial time:   %.4f s\n", serial_seconds);
    printf("Speedup:       %.2fx\n", serial_seconds / threaded_seconds);
    printf("Results match: %s\n", results_match() ? "yes" : "NO (bug!)");

    return 0;
}
