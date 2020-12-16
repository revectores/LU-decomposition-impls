#include <stdlib.h>
#include "lu.h"


int lu_decompose(double** Ap, size_t n, double** Lp, double** Up){
    double (*A)[n] = (void*) Ap;
    double (*L)[n] = (void*) Lp;
    double (*U)[n] = (void*) Up;

    memset(L, 0, n * n * sizeof(double));
    memcpy(U, A, n * n * sizeof(double));

    for (int k = 0; k < n-1; k++){
        for (int i = k+1; i < n; i++){
            L[i][k] = U[i][k] / U[k][k];
        }
        for (int j = k+1; j < n; j++){
            for (int i = k+1; i < n; i++){
                U[i][j] = U[i][j] - L[i][k] * U[k][j];
            }
        }
    }

    return 0;
}
