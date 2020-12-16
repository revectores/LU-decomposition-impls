#include <stdio.h>
#include "util.h"


int min(int a, int b){
    return a < b ? a : b;
}

void print_matrix(double** M, int n){
    double (*A)[n] = (void*)M;

    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            printf("%lf ", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
