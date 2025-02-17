#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define TMin INT_MIN
#define TMax INT_MAX

#include "btest.h"
#include "bits.h"

test_rec test_set[] = {

    {"bitXor", (funct_t)bitXor, (funct_t)test_bitXor, 2, "$", 7, 1, {{TMin, TMax}, {TMin, TMax}, {TMin, TMax}}},
    {"byteSwap", (funct_t)byteSwap, (funct_t)test_byteSwap, 3, "$", 17, 2, {{TMin, TMax}, {0, 3}, {0, 3}}},
    {"logicalShift", (funct_t)logicalShift, (funct_t)test_logicalShift, 2, "$", 20, 3, {{TMin, TMax}, {0, 31}, {TMin, TMax}}},
    {"float_i2f", (funct_t)float_i2f, (funct_t)test_float_i2f, 1, "$", 30, 4, {{1, 1}, {1, 1}, {1, 1}}},
    {"leftBitCount", (funct_t)leftBitCount, (funct_t)test_leftBitCount, 1, "$", 40, 4, {{TMin, TMax}, {TMin, TMax}, {TMin, TMax}}},
    {"floatPower2", (funct_t)floatPower2, (funct_t)test_floatPower2, 1, "$", 30, 4, {{1, 1}, {1, 1}, {1, 1}}},
    {"floatScale2", (funct_t)floatScale2, (funct_t)test_floatScale2, 1, "$", 30, 4, {{1, 1}, {1, 1}, {1, 1}}},
    {"reverse", (funct_t)reverse, (funct_t)test_reverse, 1, "$", 30, 3, {{TMin, TMax}, {TMin, TMax}, {TMin, TMax}}},
    {"samesign", (funct_t)samesign, (funct_t)test_samesign, 2, "$", 12, 2, {{TMin, TMax}, {TMin, TMax}, {TMin, TMax}}},
    {"logtwo", (funct_t)logtwo, (funct_t)test_logtwo, 1, "$", 25, 4, {{1, TMax}, {TMin, TMax}, {TMin, TMax}}},
    {"float64_f2i", (funct_t)float64_f2i, (funct_t)test_float64_f2i, 2, "$", 40, 3, {{1, 1}, {1, 1}, {1, 1}}},
    {"", NULL, NULL, 0, "", 0, 0, {{0, 0}, {0, 0}, {0, 0}}}};
