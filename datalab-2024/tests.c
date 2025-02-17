/* Testing Code */

#include <math.h>
#include <stdio.h>
#include <limits.h>
/* Routines used by floation point test code */

/* Convert from bit level representation to floating point number */
float u2f(unsigned u) {
    union {
        unsigned u;
        float f;
    } a;
    a.u = u;
    return a.f;
}

/* Convert from floating point number to bit-level representation */
unsigned f2u(float f) {
    union {
        unsigned u;
        float f;
    } a;
    a.f = f;
    return a.u;
}

int test_bitXor(int x, int y) {
    return x ^ y;
}

int test_logtwo(int v) {
    if (v <= 1)
        return 0;  // 32-bit value to find the log2 of
    const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
    const unsigned int S[] = {1, 2, 4, 8, 16};
    int i;
    unsigned int r = 0;       // result of log2(v) will go here
    for (i = 4; i >= 0; i--)  // unroll for speed...
    {
        if (v & b[i]) {
            v >>= S[i];
            r |= S[i];
        }
    }
    return r;
}
int test_byteSwap(int x, int n, int m) {
    /* little endiamachine */
    /* least significant byte stored first */
    unsigned int nmask, mmask;
    switch (n) {
        case 0:
            nmask = x & 0xFF;
            x &= 0xFFFFFF00;
            break;
        case 1:
            nmask = (x & 0xFF00) >> 8;
            x &= 0xFFFF00FF;
            break;
        case 2:
            nmask = (x & 0xFF0000) >> 16;
            x &= 0xFF00FFFF;
            break;
        default:
            nmask = ((unsigned int)(x & 0xFF000000)) >> 24;
            x &= 0x00FFFFFF;
            break;
    }
    switch (m) {
        case 0:
            mmask = x & 0xFF;
            x &= 0xFFFFFF00;
            break;
        case 1:
            mmask = (x & 0xFF00) >> 8;
            x &= 0xFFFF00FF;
            break;
        case 2:
            mmask = (x & 0xFF0000) >> 16;
            x &= 0xFF00FFFF;
            break;
        default:
            mmask = ((unsigned int)(x & 0xFF000000)) >> 24;
            x &= 0x00FFFFFF;
            break;
    }
    nmask <<= 8 * m;
    mmask <<= 8 * n;
    return x | nmask | mmask;
}
int test_samesign(int x, int y) {
    if (x < 0 && y < 0) {
        return 1;
    } else if (x > 0 && y > 0) {
        return 1;
    } else if (x == y && x == 0) {
        return 1;
    } else {
        return 0;
    }
}
int test_logicalShift(int x, int n) {
    unsigned u = (unsigned)x;
    unsigned shifted = u >> n;
    return (int)shifted;
}
unsigned test_reverse(unsigned v) {
    static const unsigned char BitReverseTable256[256] =
        {
#define R2(n) n, n + 2 * 64, n + 1 * 64, n + 3 * 64
#define R4(n) R2(n), R2(n + 2 * 16), R2(n + 1 * 16), R2(n + 3 * 16)
#define R6(n) R4(n), R4(n + 2 * 4), R4(n + 1 * 4), R4(n + 3 * 4)
            R6(0), R6(2), R6(1), R6(3)};
    unsigned int c;
    c = (BitReverseTable256[v & 0xff] << 24) |
        (BitReverseTable256[(v >> 8) & 0xff] << 16) |
        (BitReverseTable256[(v >> 16) & 0xff] << 8) |
        (BitReverseTable256[(v >> 24) & 0xff]);
    return c;
}
unsigned test_float_i2f(int x) {
    float f = (float)x;
    return f2u(f);
}
int test_float64_f2i(unsigned uf1, unsigned uf2) {
      unsigned sign = uf2 >> 31;
      unsigned exp = (uf2 >> 20) & 0x7FF;
      unsigned val = 0x80000000u | ((uf2 & 0xFFFFF) << 11) | (uf1 >> 21);
      if (exp < 1023) {
        return 0;
      }
      if (exp > 1054)
        return 0x80000000u;
      val = val >> (1054 - exp);
      if (sign) {
        return val > 0x80000000u ? 0x80000000u : -val;
      } else {
        return val > 0x7FFFFFFF ? 0x80000000u : val;
      }
}
int test_leftBitCount(int x) {
    int result = 0;
    int i;
    for (i = 31; i >= 0; i--) {
        int bit = (x >> i) & 0x1;
        if (!bit)
            break;
        result++;
    }
    return result;
}
unsigned test_floatScale2(unsigned uf) {
    float f = u2f(uf);
    float tf = 2 * f;
    if (isnan(f))
        return uf;
    else
        return f2u(tf);
}
unsigned test_floatPower2(int x) {
    float result = 1.0;
    float p2 = 2.0;
    int recip = (x < 0);
    /* treat tmin specially */
    if ((unsigned)x == 0x80000000) {
        return 0;
    }
    if (recip) {
        x = -x;
        p2 = 0.5;
    }
    while (x > 0) {
        if (x & 0x1)
            result = result * p2;
        p2 = p2 * p2;
        x >>= 1;
    }
    return f2u(result);
}
