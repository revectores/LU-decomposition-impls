#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "util.h"
#include "lu.h"


typedef struct __worker_args_s {
    size_t n;
    size_t r;
    double** A;
    double** L;
    double** U;
    bool* done;
    pthread_cond_t* signals;
    pthread_mutex_t* mutexes;
} worker_args_s;


void* lu_worker(void* args){
    worker_args_s* worker_args = (worker_args_s*) args;
    size_t n = worker_args->n;
    size_t r = worker_args->r;
    double (*A)[n] = (void*) worker_args->A;
    double (*L)[n] = (void*) worker_args->L;
    double (*U)[n] = (void*) worker_args->U;
    bool *done = worker_args->done;
    pthread_cond_t *signals = worker_args->signals;
    pthread_mutex_t *mutexes = worker_args->mutexes;

    for (int k = 0; k < r; k++) {
        while(!done[k])
            pthread_cond_wait(&signals[k], &mutexes[r]);

        L[r][k] = U[r][k] / U[k][k];

        for (int c = k + 1; c < n; c++) {
            U[r][c] -= L[r][k] * U[k][c];
        }
    }

    done[r] = true;
    pthread_cond_broadcast(&signals[r]);

    printf("worker done: %zu\n", r);
    fflush(NULL);
    return NULL;
}


int lu_decompose(double** Ap, size_t n, double** Lp, double** Up){
    double (*A)[n] = (void*)Ap;
    double (*L)[n] = (void*)Lp;
    double (*U)[n] = (void*)Up;

    memset(L, 0, n * n * sizeof(double));
    memcpy(U, A, n * n * sizeof(double));

    pthread_t *threads = malloc(sizeof(pthread_t[n]));
    bool *done = malloc(sizeof(bool[n]));
    pthread_cond_t *signals = malloc(sizeof(pthread_cond_t[n]));
    pthread_mutex_t *mutexes = malloc(sizeof(pthread_mutex_t[n]));
    worker_args_s *worker_args_list = malloc(sizeof(worker_args_s[n]));

    for (size_t r = 0; r < n; r++){
        worker_args_list[r] = (worker_args_s) {
            n, r, Ap, Lp, Up,
            done, signals, mutexes
        };
        pthread_mutex_init(&mutexes[r], NULL);
        pthread_cond_init(&signals[r], NULL);
        done[r] = false;
        pthread_create(&threads[r], NULL, lu_worker, &worker_args_list[r]);
    }

    for (size_t r = 0; r < n; r++){
        pthread_join(threads[r], NULL);
    }

    return 0;
}
