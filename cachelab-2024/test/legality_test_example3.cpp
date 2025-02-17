#include "cachelab.h"

#define m case0_m
#define n case0_n
#define p case0_p

void gemm_case0(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    reg a[m * n];
    reg b[n * p];
    int* x;
    for (reg i = 0; i < m; ++i) {
        for (reg j = 0; j < n; ++j) {
            a[i * n + j] = A[i * n + j];
        }
    }
    for (reg i = 0; i < n; ++i) {
        for (reg j = 0; j < p; ++j) {
            b[i * p + j] = B[i * p + j];
        }
    }
    for (reg i = 0; i < m; ++i) {
        for (reg j = 0; j < p; ++j) {
            reg tmpc = 0;
            for (reg k = 0; k < n; ++k) {
                reg tmpa = a[i * n + k];
                reg tmpb = b[k * p + j];
                tmpc += tmpa * tmpb;
            }
            C[i * p + j] = tmpc;
        }
    }
}

#undef m
#undef n
#undef p