#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <assert.h>
#include "util.h"
#include "lu.h"


int lu_decompose(double** Ap, size_t n ,double** Lp, double** Up){
    double (*A)[n] = (void*)Ap;
    double (*L)[n] = (void*)Lp;
    double (*U)[n] = (void*)Up;

    memset(L, 0, n * n * sizeof(double));
    memcpy(U, A, n * n * sizeof(double));

    int (*pipes)[n][2] = malloc(sizeof(int[n][n][2]));
    int (*feedback_pipes)[2] = malloc(sizeof(int[n][2]));
    memset(pipes, 0, sizeof(int[n][n][2]));
    memset(feedback_pipes, 0, sizeof(int[n][2]));

    for (int r = 0; r < n; r++){
        pipe(feedback_pipes[r]);
        for (int c = 0; c < n; c++){
            pipe(pipes[r][c]);
        }
    }

    for (int r = 0; r < n; r++){
        int fork_code = fork();

        assert(fork_code >= 0);

        if (fork_code == 0) {
            for (int k = 0; k < r; k++){
                read(pipes[k][r][0], &L[k], sizeof(double[n]));
                read(pipes[k][r][0], &U[k], sizeof(double[n]));
                close(pipes[k][r][0]);

                L[r][k] = U[r][k] / U[k][k];

                for (int c = k+1; c < n; c++){
                    U[r][c] -= L[r][k] * U[k][c];
                }
            }

            for (int k = r+1; k < n; k++){
                write(pipes[r][k][1], &L[r], sizeof(double[n]));
                write(pipes[r][k][1], &U[r], sizeof(double[n]));
                close(pipes[r][k][1]);
            }

            write(feedback_pipes[r][1], &L[r], sizeof(double[n]));
            write(feedback_pipes[r][1], &U[r], sizeof(double[n]));
            close(feedback_pipes[r][1]);

            fflush(NULL);
            _exit(0);
        }
    }

    for (int r = 0; r < n; r++){
        read(feedback_pipes[r][0], &L[r], sizeof(double[n]));
        read(feedback_pipes[r][0], &U[r], sizeof(double[n]));
        close(feedback_pipes[r][0]);
    }

    return 0;
}
