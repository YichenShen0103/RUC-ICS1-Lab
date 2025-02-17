#pragma once
#include "common.h"
#include "matrix.h"

void gemm_case0(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer);
void gemm_case1(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer);
void gemm_case2(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer);
void gemm_case3(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer);

void gemm_case0_baseline(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer);
void gemm_case1_baseline(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer);
void gemm_case2_baseline(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer);
void gemm_case3_baseline(ptr_reg A, ptr_reg B, ptr_reg C, ptr_reg buffer);