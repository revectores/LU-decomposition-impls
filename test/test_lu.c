#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "lu.h"
#include "util.h"
#include "timing.h"


ssize_t read_matrix_from(FILE* fp, double*** Mp){
    int n;
    int rc;
    char* line = NULL;
    size_t len = 0;

    if ((rc = getline(&line, &len, fp)) != -1){
        n = atoi(line);
    }

    double (*M)[n] = malloc(sizeof(double[n][n]));
    *Mp = (double**)M;

    int r = 0;
    while((rc = getline(&line, &len, fp)) != -1){
        char* a;
        int c = 0;

        M[r][c++] = atof(strtok(line, ","));

        while((a = (strtok(NULL, ",")))){
            // printf("r = %d, c = %d\n", r, c);
            M[r][c++] = atof(a);
        }
        r++;
    }

    return n;
}


void test_lu_decompose(){
    char* matrix_filename = "input_10.txt";

    FILE* fp = fopen(matrix_filename, "r");
    if (!fp) {
        fprintf(stderr, "File %s does not exist\n", matrix_filename);
        return;
    }

    double** Ap;
    int n = read_matrix_from(fp, &Ap);

    double (*A)[n] = (void*) Ap;
    double (*L)[n] = malloc(sizeof(double[n][n]));
    double (*U)[n] = malloc(sizeof(double[n][n]));
    double start_clock;
    double fin_clock;

    print_matrix((double**)A, n);

    start_clock = now();
    lu_decompose((double**)A, n, (double**)L, (double**)U);
    fin_clock = now();
    printf("%lf\n", fin_clock - start_clock);
}


int main(){
    test_lu_decompose();
    return 0;
}
