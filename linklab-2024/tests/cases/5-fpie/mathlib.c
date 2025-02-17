#include "mathlib.h"

// Mathematical constants
const double PI = 3.14159265358979323846;
const double E = 2.71828182845904523536;

// Basic arithmetic operations
int add(int a, int b)
{
    return a + b;
}

int subtract(int a, int b)
{
    return a - b;
}

int multiply(int a, int b)
{
    return a * b;
}

int divide(int a, int b)
{
    if (b == 0)
        return 0; // Simple error handling
    return a / b;
}

// Advanced mathematical functions
int factorial(int n)
{
    if (n <= 1)
        return 1;
    return n * factorial(n - 1);
}

int fibonacci(int n)
{
    if (n <= 1)
        return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int gcd(int a, int b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

int lcm(int a, int b)
{
    return (a * b) / gcd(a, b);
}

// Array operations
int array_sum(const int* arr, int size)
{
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}

void array_sort(int* arr, int size)
{
    // Quick sort implementation
    if (size <= 1)
        return;

    // Use last element as pivot
    int pivot = arr[size - 1];
    int i = -1;

    for (int j = 0; j < size - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            _swap(&arr[i], &arr[j]);
        }
    }
    _swap(&arr[i + 1], &arr[size - 1]);

    // Recursively sort sub-arrays
    array_sort(arr, i + 1);
    array_sort(arr + i + 2, size - i - 2);
}