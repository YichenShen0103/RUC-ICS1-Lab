#ifndef __MATHLIB_H__
#define __MATHLIB_H__

// Mathematical constants
extern const double PI; // ratio of circle's circumference to its diameter
extern const double E; // base of natural logarithm

// Basic arithmetic operations
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);

// Advanced mathematical functions
int factorial(int n);
int fibonacci(int n);
int gcd(int a, int b);
int lcm(int a, int b);

// Array operations
int array_sum(const int* arr, int size);
void array_sort(int* arr, int size);

// Internal helper functions (defined in mathlib_utils.c)
int _is_prime(int n);
void _swap(int* a, int* b);
int _array_partition(int* arr, int low, int high);

#endif // __MATHLIB_H__