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
    size_t c;
    double** A;
    double** L;
    double** U;
    bool** done;
    pthread_cond_t** signals;
    pthread_mutex_t** mutexes;
} worker_args_s;


void* lu_worker(void* args){
    worker_args_s* worker_args = (worker_args_s*) args;
    size_t n = worker_args->n;
    size_t r = worker_args->r;
    size_t c = worker_args->c;
    double (*A)[n] = (void*) worker_args->A;
    double (*L)[n] = (void*) worker_args->L;
    double (*U)[n] = (void*) worker_args->U;
    bool (*done)[n] = (void*) worker_args->done;
    pthread_cond_t (*signals)[n] = (void*) worker_args->signals;
    pthread_mutex_t (*mutexes)[n] = (void*) worker_args->mutexes;

    // Approach 1: condition variable
    for (int k = 0; k < min(r, c) - 1; k++) {
        while (!done[k][c])
            pthread_cond_wait(&signals[k][c], &mutexes[r][c]);
        while (!done[r][k])
            pthread_cond_wait(&signals[r][k], &mutexes[r][c]);

        U[r][c] -= L[r][k] * U[k][c];
    }

    // Approach 2: Spining
    // for (int k = 0; k < min(r, c) - 1; k++) {
    //     if (done[k][c] && done[r][k]){
    //         U[r][c] -= L[r][k] * U[k][c];
    //     }
    // }

    if (r <= c){
        done[r][c] = true;
        pthread_cond_broadcast(&signals[r][c]);
    } else {
        while (!done[c][c]){
            pthread_cond_wait(&signals[c][c], &mutexes[r][c]);
        }

        L[r][c] = U[r][c] / U[c][c];

        done[r][c] = true;
        pthread_cond_broadcast(&signals[r][c]);
    }

    return NULL;
    // printf("worker done: %zu %zu\n", r, c);
}


int lu_decompose(double** Ap, size_t n, double** Lp, double** Up){
    double (*A)[n] = (void*)Ap;
    double (*L)[n] = (void*)Lp;
    double (*U)[n] = (void*)Up;

    memset(L, 0, n * n * sizeof(double));
    memcpy(U, A, n * n * sizeof(double));

    pthread_t (*threads)[n] = malloc(sizeof(pthread_t[n][n]));
    bool (*done)[n] = malloc(sizeof(bool[n][n]));
    pthread_cond_t (*signals)[n] = malloc(sizeof(pthread_cond_t[n][n]));
    pthread_mutex_t (*mutexes)[n] = malloc(sizeof(pthread_mutex_t[n][n]));
    worker_args_s (*worker_args_matrix)[n] = malloc(sizeof(worker_args_matrix[n][n]));

    for (size_t r = 0; r < n; r++) {
        for (size_t c = 0; c < n; c++) {
            worker_args_matrix[r][c] = (worker_args_s) {
                n, r, c, Ap, Lp, Up,
                (void*)done,
                (void*)signals,
                (void*)mutexes
            };
        };
    }

    for (size_t r = 0; r < n; r++){
        for (size_t c = 0; c < n; c++){
            pthread_mutex_init(&mutexes[r][c], NULL);
            // pthread_mutex_lock(&mutexes[r][c]);
            pthread_cond_init(&signals[r][c], NULL);

            done[r][c] = false;
        }
    }

    for (size_t r = 0; r < n; r++){
        for (size_t c = 0; c < n; c++){
            // printf("master: %zu %zu\n", worker_args.r, worker_args.c);
            // printf("%d %d\n", r, c);
            pthread_create(&threads[r][c], NULL, lu_worker, &worker_args_matrix[r][c]);
        }
    }

    for (size_t r = 0; r < n; r++){
        for (size_t c = 0; c < n; c++){
            pthread_join(threads[r][c], NULL);
        }
    }

    return 0;
}
