/*
请注意，你的代码不能出现任何 int/short/char/float/double/auto 等局部变量/函数传参，我们仅允许使用 reg 定义的寄存器变量。
其中 reg 等价于一个 int。

你不能自己申请额外的内存，即不能使用 new/malloc，作为补偿我们传入了一段 buffer，大小为 BUFFER_SIZE = 64，你可以视情况使用。

我们的数组按照 A, B, C, buffer 的顺序在内存上连续紧密排列，且 &A = 0x30000000（这是模拟的设定，不是 A 的真实地址）

如果你需要以更自由的方式访问内存，你可以以相对 A 的方式访问，比如 A[100]，用 *(0x30000000) 是无法访问到的。

如果你有定义常量的需求（更严谨的说法是，你想定义的是汇编层面的立即数，不应该占用寄存器），请参考下面这种方式使用宏定义来完成。
*/

#include "cachelab.h"
#include <iostream>
#define A(i, j) a[(j) * m + (i)]
#define B(i, j) b[(j) * +(i)]
#define C(i, j) c[(j) * ldc + (i)]

#define m case0_m
#define n case0_n
#define p case0_p

// 我们用这个 2*2*2 的矩阵乘法来演示寄存器是怎么被分配的
void gemm_case0(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    reg tmpa[2][2];                // regs for storing integers in matrix A
    reg tmpb[2][2];                // regs for storing integers in matrix B
    reg tmpc[2][2];                // regs for storing integers in matrix C
    for (reg i = 0; i < 2; ++i) {  // loading
        for (reg j = 0; j < 2; ++j) {
            tmpc[i][j] = 0;
            tmpa[i][j] = A[i * n + j];
            tmpb[i][j] = B[i * m + j];
        }
    }
    for (reg i = 0; i < 2; ++i) {  // calculating
        for (reg j = 0; j < 2; ++j) {
            for (reg k = 0; k < 2; ++k) {
                tmpc[i][j] += tmpa[i][k] * tmpb[k][j];
            }
        }
    }
    for (reg i = 0; i < 2; ++i) {  // storing
        for (reg j = 0; j < 2; ++j) {
            C[i * p + j] = tmpc[i][j];
        }
    }
}

#undef m
#undef n
#undef p

// end of case0

#define m case1_m
#define n case1_n
#define p case1_p

// Batch processing using the special structure of the cache, exchange the priority
void gemm_case1_base(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg i = 0; i < 16; ++i) {
        reg c_line_i[16] = {0};
        for (reg j = 0; j < 16; ++j) {
            reg tmpa = A[16 * i + j];
            for (reg k = 0; k < 16; ++k) {
                reg tmpb = B[16 * j + k];
                c_line_i[k] += tmpa * tmpb;
            }
        }
        for (reg j = 0; j < 16; ++j) {
            C[i * 16 + j] = c_line_i[j];
        }
    }
}
// end of sol1; speed: 5.956, miss cache: 604, miss reg: 4608

#define BLOCK_SIZE 8

// Block optimition
// op. tmpa extracted as register array previouly
// op. change order from ijk to kji
void gemm_case1_op2(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg k = 0; k < 16; k += BLOCK_SIZE) {
        for (reg j = 0; j < 16; j += BLOCK_SIZE) {
            for (reg i = 0; i < 16; i += BLOCK_SIZE) {
                for (reg ii = i; ii < i + BLOCK_SIZE; ++ii) {
                    reg tmpc[BLOCK_SIZE];
                    reg tmpa[BLOCK_SIZE];
                    for (reg jj = j; jj < j + BLOCK_SIZE; ++jj)
                        tmpc[jj - j] = C[ii * 16 + jj];
                    for (reg kk = k; kk < k + BLOCK_SIZE; ++kk)
                        tmpa[kk - k] = A[ii * 16 + kk];
                    for (reg kk = k; kk < k + BLOCK_SIZE; ++kk) {
                        for (reg jj = j; jj < j + BLOCK_SIZE; ++jj) {
                            reg tmpb = B[kk * 16 + jj];
                            tmpc[jj - j] += tmpa[kk - k] * tmpb;
                        }
                    }
                    for (reg jj = j; jj < j + BLOCK_SIZE; ++jj)
                        C[ii * 16 + jj] = tmpc[jj - j];
                }
            }
        }
    }
}

// BLOCKSIZE 4     8
// speed     3.67  7.29

#define blockM 16  // 16
#define blockK 16  // 16
#define blockN 16  // 16
#define innerBlockSize 8
void gemm_case1(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg iBlock = 0; iBlock < 16; iBlock += 4) {
        for (reg jBlock = 0; jBlock < 16; jBlock += 4) {
            reg tmpc[4][4];
            for (reg k = 0; k < 16; ++k) {
                reg a0 = A[(iBlock + 0) * 16 + k];
                reg a1 = A[(iBlock + 1) * 16 + k];
                reg a2 = A[(iBlock + 2) * 16 + k];
                reg a3 = A[(iBlock + 3) * 16 + k];

                reg b0 = B[k * 16 + (jBlock + 0)];
                reg b1 = B[k * 16 + (jBlock + 1)];
                reg b2 = B[k * 16 + (jBlock + 2)];
                reg b3 = B[k * 16 + (jBlock + 3)];

                tmpc[0][0] += a0 * b0;
                tmpc[0][1] += a0 * b1;
                tmpc[0][2] += a0 * b2;
                tmpc[0][3] += a0 * b3;

                tmpc[1][0] += a1 * b0;
                tmpc[1][1] += a1 * b1;
                tmpc[1][2] += a1 * b2;
                tmpc[1][3] += a1 * b3;

                tmpc[2][0] += a2 * b0;
                tmpc[2][1] += a2 * b1;
                tmpc[2][2] += a2 * b2;
                tmpc[2][3] += a2 * b3;

                tmpc[3][0] += a3 * b0;
                tmpc[3][1] += a3 * b1;
                tmpc[3][2] += a3 * b2;
                tmpc[3][3] += a3 * b3;
            }
            for (reg ii = 0; ii < 4; ++ii) {
                C[(iBlock + ii) * 16 + (jBlock + 0)] = tmpc[ii][0];
                C[(iBlock + ii) * 16 + (jBlock + 1)] = tmpc[ii][1];
                C[(iBlock + ii) * 16 + (jBlock + 2)] = tmpc[ii][2];
                C[(iBlock + ii) * 16 + (jBlock + 3)] = tmpc[ii][3];
            }
        }
    }
}

#undef m
#undef n
#undef p
#undef blockK
#undef blockM
#undef blockN
#undef innerBlockSize
#undef BLOCK_SIZE

#define m case2_m
#define n case2_n
#define p case2_p
#define BLOCK1_SIZE 16
#define BLOCK2_SIZE 8

void gemm_case2_base(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg k = 0; k < 32; k += BLOCK1_SIZE) {
        for (reg j = 0; j < 32; j += BLOCK1_SIZE) {
            for (reg i = 0; i < 32; i += BLOCK1_SIZE) {
                for (reg ii = i; ii < i + BLOCK1_SIZE; ++ii) {
                    reg tmpc[BLOCK1_SIZE];
                    for (reg jj = j; jj < j + BLOCK1_SIZE; ++jj)
                        tmpc[jj - j] = C[ii * 32 + jj];
                    for (reg kk = k; kk < k + BLOCK1_SIZE; ++kk) {
                        reg tmpa = A[ii * 32 + kk];
                        for (reg jj = j; jj < j + BLOCK1_SIZE; ++jj) {
                            reg tmpb = B[kk * 32 + jj];
                            tmpc[jj - j] += tmpa * tmpb;
                        }
                    }
                    for (reg jj = j; jj < j + BLOCK1_SIZE; ++jj)
                        C[ii * 32 + jj] = tmpc[jj - j];
                }
            }
        }
    }
}
// BS    4     8     16
// speed 2.919 4.654 5.368

#define blockM 32  // 32
#define blockK 32  // 32
#define blockN 32  // 32
#define innerBlockSize 16

void gemm_case2_try(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg k = 0; k < 32; k += blockK) {
        for (reg i = 0; i < 32; i += blockM) {
            for (reg j = 0; j < 32; j += blockN) {
                for (reg kk = k; kk < k + std::min(blockK, 32 - k); kk += innerBlockSize) {
                    for (reg jj = j; jj < j + std::min(blockN, 32 - j); jj += innerBlockSize) {
                        for (reg ii = i; ii < i + std::min(blockM, 32 - i); ii += innerBlockSize) {
                            for (reg iii = ii; iii < ii + std::min(innerBlockSize, i + std::min(blockM, 32 - i) - ii); ++iii) {
                                reg tmpc[innerBlockSize];
                                // reg tmpa[innerBlockSize];
                                for (reg jjj = jj; jjj < jj + std::min(innerBlockSize, j + std::min(blockN, 32 - j) - jj); ++jjj)
                                    tmpc[jjj - jj] = C[iii * p + jjj];
                                // for (reg kkk = kk; kkk < kk + std::min(innerBlockSize, k + std::min(blockK, 37 - k) - kk); ++kkk) {
                                // tmpa[kkk - kk] = A[iii * n + kkk];
                                // }
                                for (reg kkk = kk; kkk < kk + std::min(innerBlockSize, k + std::min(blockK, 32 - k) - kk); ++kkk) {
                                    reg tmpa = A[iii * n + kkk];
                                    for (reg jjj = jj; jjj < jj + std::min(innerBlockSize, j + std::min(blockN, 32 - j) - jj); ++jjj) {
                                        reg tmpb = B[kkk * p + jjj];
                                        tmpc[jjj - jj] += tmpa * tmpb;
                                    }
                                }
                                for (reg jjj = jj; jjj < jj + std::min(innerBlockSize, j + std::min(blockN, 32 - j) - jj); ++jjj) {
                                    C[iii * p + jjj] = tmpc[jjj - jj];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void gemm_case2(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg iBlock = 0; iBlock < 32; iBlock += 4) {
        for (reg jBlock = 0; jBlock < 32; jBlock += 4) {
            reg tmpc[4][4];
            for (reg k = 0; k < 32; ++k) {
                reg a0 = A[(iBlock + 0) * 32 + k];
                reg a1 = A[(iBlock + 1) * 32 + k];
                reg a2 = A[(iBlock + 2) * 32 + k];
                reg a3 = A[(iBlock + 3) * 32 + k];

                reg b0 = B[k * 32 + (jBlock + 0)];
                reg b1 = B[k * 32 + (jBlock + 1)];
                reg b2 = B[k * 32 + (jBlock + 2)];
                reg b3 = B[k * 32 + (jBlock + 3)];

                tmpc[0][0] += a0 * b0;
                tmpc[0][1] += a0 * b1;
                tmpc[0][2] += a0 * b2;
                tmpc[0][3] += a0 * b3;

                tmpc[1][0] += a1 * b0;
                tmpc[1][1] += a1 * b1;
                tmpc[1][2] += a1 * b2;
                tmpc[1][3] += a1 * b3;

                tmpc[2][0] += a2 * b0;
                tmpc[2][1] += a2 * b1;
                tmpc[2][2] += a2 * b2;
                tmpc[2][3] += a2 * b3;

                tmpc[3][0] += a3 * b0;
                tmpc[3][1] += a3 * b1;
                tmpc[3][2] += a3 * b2;
                tmpc[3][3] += a3 * b3;
            }
            for (reg ii = 0; ii < 4; ++ii) {
                C[(iBlock + ii) * 32 + (jBlock + 0)] = tmpc[ii][0];
                C[(iBlock + ii) * 32 + (jBlock + 1)] = tmpc[ii][1];
                C[(iBlock + ii) * 32 + (jBlock + 2)] = tmpc[ii][2];
                C[(iBlock + ii) * 32 + (jBlock + 3)] = tmpc[ii][3];
            }
        }
    }
}

#undef m
#undef n
#undef p
#undef blockK
#undef blockM
#undef blockN
#undef innerBlockSize
#undef BLOCK_SIZE

#define m case3_m
#define n case3_n
#define p case3_p
#define BLOCK_SIZE 20

void gemm_case3_sol(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg i = 0; i < m; i += BLOCK_SIZE) {
        for (reg j = 0; j < p; j += BLOCK_SIZE) {
            for (reg k = 0; k < n; k += BLOCK_SIZE) {
                for (reg ii = i; ii < std::min(i + BLOCK_SIZE, m); ++ii) {
                    reg tmpc[BLOCK_SIZE];
                    for (reg jj = j; jj < std::min(j + BLOCK_SIZE, p); ++jj)
                        tmpc[jj - j] = C[ii * p + jj];
                    for (reg kk = k; kk < std::min(k + BLOCK_SIZE, n); ++kk) {
                        reg tmpa = A[ii * n + kk];
                        for (reg jj = j; jj < std::min(j + BLOCK_SIZE, p); ++jj) {
                            reg tmpb = B[kk * p + jj];
                            tmpc[jj - j] += tmpa * tmpb;
                        }
                    }
                    for (reg jj = j; jj < std::min(j + BLOCK_SIZE, p); ++jj)
                        C[ii * p + jj] = tmpc[jj - j];
                }
            }
        }
    }
}

#undef BLOCK_SIZE

#define blockM 24  // 31
#define blockK 16  // 37
#define blockN 16  // 31
#define innerBlockSize 16

void gemm_case3_try(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg k = 0; k < 37; k += blockK) {
        for (reg i = 0; i < 31; i += blockM) {
            for (reg j = 0; j < 31; j += blockN) {
                for (reg kk = k; kk < k + std::min(blockK, 37 - k); kk += innerBlockSize) {
                    for (reg jj = j; jj < j + std::min(blockN, 31 - j); jj += innerBlockSize) {
                        for (reg ii = i; ii < i + std::min(blockM, 31 - i); ii += innerBlockSize) {
                            for (reg iii = ii; iii < ii + std::min(innerBlockSize, i + std::min(blockM, 31 - i) - ii); ++iii) {
                                reg tmpc[innerBlockSize];
                                // reg tmpa[innerBlockSize];
                                for (reg jjj = jj; jjj < jj + std::min(innerBlockSize, j + std::min(blockN, 31 - j) - jj); ++jjj)
                                    tmpc[jjj - jj] = C[iii * p + jjj];
                                // for (reg kkk = kk; kkk < kk + std::min(innerBlockSize, k + std::min(blockK, 37 - k) - kk); ++kkk) {
                                // tmpa[kkk - kk] = A[iii * n + kkk];
                                // }
                                for (reg kkk = kk; kkk < kk + std::min(innerBlockSize, k + std::min(blockK, 37 - k) - kk); ++kkk) {
                                    reg tmpa = A[iii * n + kkk];
                                    for (reg jjj = jj; jjj < jj + std::min(innerBlockSize, j + std::min(blockN, 31 - j) - jj); ++jjj) {
                                        reg tmpb = B[kkk * p + jjj];
                                        tmpc[jjj - jj] += tmpa * tmpb;
                                    }
                                }
                                for (reg jjj = jj; jjj < jj + std::min(innerBlockSize, j + std::min(blockN, 31 - j) - jj); ++jjj) {
                                    C[iii * p + jjj] = tmpc[jjj - jj];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

#define BLOCK_SIZE 4
#define iLimit 28
#define jLimit 28

// op1. 对剩余的单独处理的部分从朴素的变成优化列处理
// op2. 单独处理的优化
void gemm_case3_prev(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg iBlock = 0; iBlock < iLimit; iBlock += BLOCK_SIZE) {
        for (reg jBlock = 0; jBlock < jLimit; jBlock += BLOCK_SIZE) {
            reg tmpc[4][4];
            for (reg k = 0; k < 37; ++k) {
                reg a0 = A[(iBlock + 0) * 37 + k];
                reg a1 = A[(iBlock + 1) * 37 + k];
                reg a2 = A[(iBlock + 2) * 37 + k];
                reg a3 = A[(iBlock + 3) * 37 + k];

                reg b0 = B[k * 31 + (jBlock + 0)];
                reg b1 = B[k * 31 + (jBlock + 1)];
                reg b2 = B[k * 31 + (jBlock + 2)];
                reg b3 = B[k * 31 + (jBlock + 3)];

                tmpc[0][0] += a0 * b0;
                tmpc[0][1] += a0 * b1;
                tmpc[0][2] += a0 * b2;
                tmpc[0][3] += a0 * b3;

                tmpc[1][0] += a1 * b0;
                tmpc[1][1] += a1 * b1;
                tmpc[1][2] += a1 * b2;
                tmpc[1][3] += a1 * b3;

                tmpc[2][0] += a2 * b0;
                tmpc[2][1] += a2 * b1;
                tmpc[2][2] += a2 * b2;
                tmpc[2][3] += a2 * b3;

                tmpc[3][0] += a3 * b0;
                tmpc[3][1] += a3 * b1;
                tmpc[3][2] += a3 * b2;
                tmpc[3][3] += a3 * b3;
            }
            for (reg ii = 0; ii < 4; ++ii) {
                C[(iBlock + ii) * 31 + (jBlock + 0)] = tmpc[ii][0];
                C[(iBlock + ii) * 31 + (jBlock + 1)] = tmpc[ii][1];
                C[(iBlock + ii) * 31 + (jBlock + 2)] = tmpc[ii][2];
                C[(iBlock + ii) * 31 + (jBlock + 3)] = tmpc[ii][3];
            }
        }
    }

    // 剩余行
    /*     for (reg i = iLimit; i < 31; ++i) {
            for (reg j = 0; j < 28; ++j) {
                reg tmpc = 0;
                for (reg k = 0; k < 37; ++k) {
                    reg tmpa = A[i * 37 + k];
                    reg tmpb = B[k * 31 + j];
                    tmpc += tmpa * tmpb;
                }
                C[i * 31 + j] = tmpc;
            }
        } */
    // 剩余列
    /*     for (reg i = 0; i < 31; ++i) {
            reg tmpc[3] = {0};
            for (reg j = 0; j < 37; ++j) {
                reg tmpa = A[i * 37 + j];
                for (reg k = 0; k < 3; ++k) {
                    reg tmpb = B[j * 31 + k + 28];
                    tmpc[k] += tmpa * tmpb;
                }
            }
            for (reg j = 0; j < 3; ++j) {
                C[i * 31 + j + 28] = tmpc[j];
            }
        } */
    for (reg jBlock = 0; jBlock < jLimit; jBlock += BLOCK_SIZE) {
        reg tmpc[3][7];
        for (reg k = 0; k < 37; ++k) {
            reg a0 = A[(iLimit + 0) * 37 + k];
            reg a1 = A[(iLimit + 1) * 37 + k];
            reg a2 = A[(iLimit + 2) * 37 + k];

            reg tmpb = B[k * 31 + (jBlock + 0)];
            reg b2 = B[k * 31 + (jBlock + 2)];

            tmpc[0][0] += a0 * tmpb;
            tmpc[1][0] += a1 * tmpb;
            tmpc[2][0] += a2 * tmpb;
            tmpb = B[k * 31 + (jBlock + 1)];
            tmpc[0][1] += a0 * tmpb;
            tmpc[1][1] += a1 * tmpb;
            tmpc[2][1] += a2 * tmpb;
            tmpb = B[k * 31 + (jBlock + 2)];
            tmpc[0][2] += a0 * tmpb;
            tmpc[1][2] += a1 * tmpb;
            tmpc[2][2] += a2 * tmpb;
            tmpb = B[k * 31 + jBlock + 3];
            tmpc[0][3] += a0 * tmpb;
            tmpc[1][3] += a1 * tmpb;
            tmpc[2][3] += a2 * tmpb;
            tmpb = B[k * 31 + (jBlock + 4)];
            tmpc[0][4] += a0 * tmpb;
            tmpc[1][4] += a1 * tmpb;
            tmpc[2][4] += a2 * tmpb;
            tmpb = B[k * 31 + (jBlock + 5)];
            tmpc[0][5] += a0 * tmpb;
            tmpc[1][5] += a1 * tmpb;
            tmpc[2][5] += a2 * tmpb;
            tmpb = B[k * 31 + (jBlock + 6)];
            tmpc[0][6] += a0 * tmpb;
            tmpc[1][6] += a1 * tmpb;
            tmpc[2][6] += a2 * tmpb;
        }
        for (reg i = 0; i < 3; ++i) {
            for (reg j = 0; j < 7; ++j)
                C[(iLimit + i) * 31 + (jBlock + j)] = tmpc[i][j];
        }
    }
    for (reg iBlock = 0; iBlock < iLimit; iBlock += BLOCK_SIZE) {
        reg tmpc[4][3];
        for (reg k = 0; k < 37; ++k) {
            reg a0 = A[(iBlock + 0) * 37 + k];
            reg a1 = A[(iBlock + 1) * 37 + k];
            reg a2 = A[(iBlock + 2) * 37 + k];
            reg a3 = A[(iBlock + 3) * 37 + k];

            reg b0 = B[k * 31 + (jLimit + 0)];
            reg b1 = B[k * 31 + (jLimit + 1)];
            reg b2 = B[k * 31 + (jLimit + 2)];

            tmpc[0][0] += a0 * b0;
            tmpc[0][1] += a0 * b1;
            tmpc[0][2] += a0 * b2;

            tmpc[1][0] += a1 * b0;
            tmpc[1][1] += a1 * b1;
            tmpc[1][2] += a1 * b2;

            tmpc[2][0] += a2 * b0;
            tmpc[2][1] += a2 * b1;
            tmpc[2][2] += a2 * b2;

            tmpc[3][0] += a3 * b0;
            tmpc[3][1] += a3 * b1;
            tmpc[3][2] += a3 * b2;
        }
        for (reg i = 0; i < 4; ++i) {
            for (reg j = 0; j < 3; ++j)
                C[(iBlock + i) * 31 + (jLimit + j)] = tmpc[i][j];
        }
    }
    reg tmpc[3][3];
    for (reg k = 0; k < 37; ++k) {
        reg a0 = A[(iLimit + 0) * 37 + k];
        reg a1 = A[(iLimit + 1) * 37 + k];
        reg a2 = A[(iLimit + 2) * 37 + k];

        reg b0 = B[k * 31 + (jLimit + 0)];
        reg b1 = B[k * 31 + (jLimit + 1)];
        reg b2 = B[k * 31 + (jLimit + 2)];

        tmpc[0][0] += a0 * b0;
        tmpc[0][1] += a0 * b1;
        tmpc[0][2] += a0 * b2;

        tmpc[1][0] += a1 * b0;
        tmpc[1][1] += a1 * b1;
        tmpc[1][2] += a1 * b2;

        tmpc[2][0] += a2 * b0;
        tmpc[2][1] += a2 * b1;
        tmpc[2][2] += a2 * b2;
    }
    for (reg i = 0; i < 3; ++i) {
        for (reg j = 0; j < 3; ++j)
            C[(iLimit + i) * 31 + (jLimit + j)] = tmpc[i][j];
    }
}

#undef m
#undef n
#undef p

// 16 32 16 --- 16 4.35
// 24 32 16 --- 16 4.360
// 24 16 16 --- 16 4.364

void gemm_case3(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer) {
    for (reg iBlock = 0; iBlock < iLimit; iBlock += BLOCK_SIZE) {
        for (reg jBlock = 0; jBlock < jLimit; jBlock += BLOCK_SIZE) {
            reg tmpc[4][4];
            for (reg k = 0; k < 37; ++k) {
                reg a0 = A[(iBlock + 0) * 37 + k];
                reg a1 = A[(iBlock + 1) * 37 + k];
                reg a2 = A[(iBlock + 2) * 37 + k];
                reg a3 = A[(iBlock + 3) * 37 + k];

                reg b0 = B[k * 31 + (jBlock + 0)];
                reg b1 = B[k * 31 + (jBlock + 1)];
                reg b2 = B[k * 31 + (jBlock + 2)];
                reg b3 = B[k * 31 + (jBlock + 3)];

                tmpc[0][0] += a0 * b0;
                tmpc[0][1] += a0 * b1;
                tmpc[0][2] += a0 * b2;
                tmpc[0][3] += a0 * b3;

                tmpc[1][0] += a1 * b0;
                tmpc[1][1] += a1 * b1;
                tmpc[1][2] += a1 * b2;
                tmpc[1][3] += a1 * b3;

                tmpc[2][0] += a2 * b0;
                tmpc[2][1] += a2 * b1;
                tmpc[2][2] += a2 * b2;
                tmpc[2][3] += a2 * b3;

                tmpc[3][0] += a3 * b0;
                tmpc[3][1] += a3 * b1;
                tmpc[3][2] += a3 * b2;
                tmpc[3][3] += a3 * b3;
            }
            for (reg ii = 0; ii < 4; ++ii) {
                C[(iBlock + ii) * 31 + (jBlock + 0)] = tmpc[ii][0];
                C[(iBlock + ii) * 31 + (jBlock + 1)] = tmpc[ii][1];
                C[(iBlock + ii) * 31 + (jBlock + 2)] = tmpc[ii][2];
                C[(iBlock + ii) * 31 + (jBlock + 3)] = tmpc[ii][3];
            }
        }
    }

    for (reg jBlock = 0; jBlock < 30; jBlock += 6) {
        reg tmpc[3][6];
        for (reg k = 0; k < 37; ++k) {
            reg a0 = A[(iLimit + 0) * 37 + k];
            reg a1 = A[(iLimit + 1) * 37 + k];
            reg a2 = A[(iLimit + 2) * 37 + k];

            reg b0 = B[k * 31 + (jBlock + 0)];
            reg b1 = B[k * 31 + (jBlock + 1)];
            reg b2 = B[k * 31 + (jBlock + 2)];
            reg b3 = B[k * 31 + (jBlock + 3)];
            reg b4 = B[k * 31 + (jBlock + 4)];

            tmpc[0][0] += a0 * b0;
            tmpc[0][1] += a0 * b1;
            tmpc[0][2] += a0 * b2;
            tmpc[0][3] += a0 * b3;
            tmpc[0][4] += a0 * b4;

            tmpc[1][0] += a1 * b0;
            tmpc[1][1] += a1 * b1;
            tmpc[1][2] += a1 * b2;
            tmpc[1][3] += a1 * b3;
            tmpc[1][4] += a1 * b4;

            tmpc[2][0] += a2 * b0;
            tmpc[2][1] += a2 * b1;
            tmpc[2][2] += a2 * b2;
            tmpc[2][3] += a2 * b3;
            tmpc[2][4] += a2 * b4;
            b4 = B[k * 31 + (jBlock + 5)];
            tmpc[0][5] += a0 * b4;
            tmpc[1][5] += a1 * b4;
            tmpc[2][5] += a2 * b4;
        }
        for (reg i = 0; i < 3; ++i) {
            for (reg j = 0; j < 6; ++j)
                C[(iLimit + i) * 31 + (jBlock + j)] = tmpc[i][j];
        }
    }
    for (reg iBlock = 0; iBlock < 30; iBlock += 6) {
        reg tmpc[6][3];
        for (reg k = 0; k < 37; ++k) {
            reg a0 = A[(iBlock + 0) * 37 + k];
            reg a1 = A[(iBlock + 1) * 37 + k];
            reg a2 = A[(iBlock + 2) * 37 + k];
            reg a3 = A[(iBlock + 3) * 37 + k];
            reg a4 = A[(iBlock + 4) * 37 + k];

            reg b0 = B[k * 31 + (jLimit + 0)];
            reg b1 = B[k * 31 + (jLimit + 1)];
            reg b2 = B[k * 31 + (jLimit + 2)];

            tmpc[0][0] += a0 * b0;
            tmpc[0][1] += a0 * b1;
            tmpc[0][2] += a0 * b2;

            tmpc[1][0] += a1 * b0;
            tmpc[1][1] += a1 * b1;
            tmpc[1][2] += a1 * b2;

            tmpc[2][0] += a2 * b0;
            tmpc[2][1] += a2 * b1;
            tmpc[2][2] += a2 * b2;

            tmpc[3][0] += a3 * b0;
            tmpc[3][1] += a3 * b1;
            tmpc[3][2] += a3 * b2;

            tmpc[4][0] += a4 * b0;
            tmpc[4][1] += a4 * b1;
            tmpc[4][2] += a4 * b2;

            a4 = A[(iBlock + 5) * 37 + k];
            tmpc[5][0] += a4 * b0;
            tmpc[5][1] += a4 * b1;
            tmpc[5][2] += a4 * b2;
        }
        for (reg i = 0; i < 6; ++i) {
            for (reg j = 0; j < 3; ++j)
                C[(iBlock + i) * 31 + (jLimit + j)] = tmpc[i][j];
        }
    }
    reg tmpc;
    for (reg k = 0; k < 37; ++k) {
        reg a0 = A[30 * 37 + k];
        reg b0 = B[k * 31 + 30];
        tmpc += a0 * b0;
    }
    C[30 * 31 + 30] = tmpc;
}

#undef BLOCK_SIZE