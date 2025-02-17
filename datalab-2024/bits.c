/* WARNING: Do not include any other libraries here,
 * otherwise you will get an error while running test.py
 * You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 *
 * Using printf will interfere with our script capturing the execution results.
 * At this point, you can only test correctness with ./btest.
 * After confirming everything is correct in ./btest, remove the printf
 * and run the complete tests with test.py.
 */

/*
 * bitXor - x ^ y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 7
 *   Difficulty: 1
 */
int bitXor(int x, int y) {
    return ~(x & y) & ~(~x & ~y);
}

/*
 * samesign - Determines if two integers have the same sign.
 *   0 is not positive, nor negative
 *   Example: samesign(0, 1) = 0, samesign(0, 0) = 1
 *            samesign(-4, -5) = 1, samesign(-4, 5) = 0
 *   Legal ops: >> << ! ^ && if else &
 *   Max ops: 12
 *   Difficulty: 2
 *
 * Parameters:
 *   x - The first integer.
 *   y - The second integer.
 *
 * Returns:
 *   1 if x and y have the same sign , 0 otherwise.
 */
int samesign(int x, int y) {
    if (x && !y) return 0;
    if (y && !x) return 0;    // Dealing with one of x and y equals 0 while the other doesn't
    return !((x ^ y) & 0x80000000);    // Other conditions can be solved by unified expression
}

/*
 * logtwo - Calculate the base-2 logarithm of a positive integer using bit
 *   shifting. (Think about bitCount)
 *   Note: You may assume that v > 0
 *   Example: logtwo(32) = 5
 *   Legal ops: > < >> << |
 *   Max ops: 25
 *   Difficulty: 4
 */
int logtwo(int v) {
    int ans = 0;

    int tmp = (v > 0x0000ffff) << 4;    // Judge if it is necessary to handle the left
    ans |= tmp;    // Use 'or' to emulate '+'
    v = v >> tmp;    // Right shifting to prepare for next operation 

    tmp = (v > 0x000000ff) << 3;
    ans |= tmp;
    v = v >> tmp;

    tmp = (v > 0x0000000f) << 2;
    ans |= tmp;
    v = v >> tmp;

    tmp = (v > 0x00000003) << 1;
    ans |= tmp;
    v = v >> tmp;

    ans |= v > 0x00000001;

    return ans;
}

/*
 *  byteSwap - swaps the nth byte and the mth byte
 *    Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *              byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *    Note: You may assume that 0 <= n <= 3, 0 <= m <= 3
 *    Legal ops: ! ~ & ^ | + << >>
 *    Max ops: 17
 *    Difficulty: 2
 */
int byteSwap(int x, int n, int m) {
    n = n << 3;    // Construct the mask of nth byte
    m = m << 3;

    int diff = ((x >> m) ^ (x >> n)) & 0x000000ff;    // Transmit the difference of the two bytes

    return x ^ ((diff << m) | (diff << n));    // (x ^ y) ^ x = y
}

/*
 * reverse - Reverse the bit order of a 32-bit unsigned integer.
 *   Example: reverse(0xFFFF0000) = 0x0000FFFF reverse(0x80000000)=0x1 reverse(0xA0000000)=0x5
 *   Note: You may assume that an unsigned integer is 32 bits long.
 *   Legal ops: << | & - + >> for while ! ~ (You can define unsigned in this function)
 *   Max ops: 30
 *   Difficulty: 3
 */
unsigned reverse(unsigned v) {
    unsigned slice_left = (v & 0xaaaaaaaa) >> 1;    // Group every two adjacent bits together and swap
    unsigned slice_right = (v & 0x55555555) << 1;
    v = slice_left | slice_right;

    slice_left = (v & 0xcccccccc) >> 2;
    slice_right = (v & 0x33333333) << 2;
    v = slice_left | slice_right;

    slice_left = (v & 0xf0f0f0f0) >> 4;
    slice_right = (v & 0x0f0f0f0f) << 4;
    v = slice_left | slice_right;

    slice_left = (v & 0xff00ff00) >> 8;
    slice_right = (v & 0x00ff00ff) << 8;
    v = slice_left | slice_right;

    slice_left = (v & 0xffff0000) >> 16;
    slice_right = (v & 0x0000ffff) << 16;

    return slice_left | slice_right;
}

/*
 * logicalShift - shift x to the right by n, using a logical shift
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Note: You can assume that 0 <= n <= 31
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Difficulty: 3
 */
int logicalShift(int x, int n) {
    return (x >> n) & (((0x7fffffff >> n) << 1) | 1);    // Use mask to offset the 1s generated automatically
}

/*
 * leftBitCount - returns count of number of consective 1's in left-hand (most) end of word.
 *   Examples: leftBitCount(-1) = 32, leftBitCount(0xFFF0F0F0) = 12,
 *             leftBitCount(0xFE00FF0F) = 7
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 50
 *   Difficulty: 4
 */
int leftBitCount(int x) {
    x &= x >> 1;
    x &= x >> 2;
    x &= x >> 4;
    x &= x >> 8;
    x &= x >> 16;    //Set all bits to 0 except for the leftmost consecutive 1s

    x += ~((x >> 1) & 0x55555555) + 1;
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0f0f0f0f;
    x += x >> 8;
    x += x >> 16;    // Group and count the number of 1s

    return x & 0x0000003f;
}

/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but it is to be interpreted as
 *   the bit-level representation of a single-precision floating point values.
 *   Legal ops: if else while for & | ~ + - >> << < > ! ==
 *   Max ops: 30
 *   Difficulty: 4
 */
unsigned float_i2f(int x) {
    if (!x) return x;    // Prevent infinite loop caused by 0

    unsigned sign = x & 0x80000000;
    if (sign) x = -x;    // Avoid logical confusion caused by signed numbers

    unsigned frac = x, expo = 0x4f800000, leftMost = 0;
    while (!leftMost) {
      leftMost = frac & 0x80000000;
      expo -= 0x00800000;
      frac <<= 1;    // Ensure the number is in the form we need
    }

    unsigned rounding = (frac & 0x000001ff) > 0x00000100;
    if ((frac & 0x000003ff) == 0x00000300) rounding = 1;
    frac = (frac >> 9) + rounding;    // Little tricks: ignore the possible overflow to simplify operation

    return sign | (expo + frac);    // Use '+' instead of '|' to deal with the overflow condition
}

/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: & >> << | if > < >= <= ! ~ else + ==
 *   Max ops: 30
 *   Difficulty: 4
 */
unsigned floatScale2(unsigned uf) {
    unsigned expoAndfrac = uf & 0x7fffffff;    // Group expo and frac since frac will not be operate singly
    unsigned expo = uf & 0x7f800000;

    if (expo == 0x7f800000) return uf;    // INF and NaN Condition
    if (expo == 0x7f000000) expoAndfrac = 0x7f800000;    // Condition very close to overflow
    else if (expo == 0) expoAndfrac <<= 1;    // Denormalized condition 
    else expoAndfrac += 0x00800000;    // Normalized condition

    return (uf & 0x80000000) | expoAndfrac; 
}

/*
 * float64_f2i - Convert a 64-bit IEEE 754 floating-point number to a 32-bit signed integer.
 *   The conversion rounds towards zero.
 *   Note: Assumes IEEE 754 representation and standard two's complement integer format.
 *   Parameters:
 *     uf1 - The lower 32 bits of the 64-bit floating-point number.
 *     uf2 - The higher 32 bits of the 64-bit floating-point number.
 *   Returns:
 *     The converted integer value, or 0x80000000 on overflow, or 0 on underflow.
 *   Legal ops: >> << | & ~ ! + - > < >= <= if else
 *   Max ops: 60
 *   Difficulty: 3
 */
int float64_f2i(unsigned uf1, unsigned uf2) {
    int rightShift = 1053 - ((uf2 & 0x7ff00000) >> 20);    // Use rightShift instead of exp to simplify operation
    if (rightShift > 30) return 0;    // Cases whose rightShift is too large
    if (rightShift < 0) return 0x80000000;    // Cases whose rightShift is too small

    int val = (0x40000000 | ((uf2 & 0x000fffff) << 10) | (uf1 >> 22))>> rightShift;    // val is at most 31 bits
    if (uf2 & 0x80000000) return -val;    // Dealing with signed number
    return val;
}

/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: < > <= >= << >> + - & | ~ ! if else &&
 *   Max ops: 30
 *   Difficulty: 4
 */
unsigned floatPower2(int x) {
    if (x >= 128) return 0x7f800000;  // Overflow conditions
    if (x <= -150) return 0;    // Too small for even denormalized condition
    if (x >= -126) return (x + 127) << 23;    // Normalized conditions
    return 0x00000001 << (149 + x);    // Denormalized conditions
}
