#include "mathlib.h"
#include "minilibc.h"

int main()
{
    // Test various math functions
    printf("PI = %d\n", (int)(PI * 1000)); // Print as fixed point
    printf("E = %d\n", (int)(E * 1000)); // Print as fixed point

    // Test basic arithmetic
    int a = 42, b = 10;
    printf("add(%d, %d) = %d\n", a, b, add(a, b));
    printf("subtract(%d, %d) = %d\n", a, b, subtract(a, b));
    printf("multiply(%d, %d) = %d\n", a, b, multiply(a, b));
    printf("divide(%d, %d) = %d\n", a, b, divide(a, b));

    // Test advanced math
    printf("factorial(5) = %d\n", factorial(5));
    printf("fibonacci(7) = %d\n", fibonacci(7));
    printf("gcd(48, 18) = %d\n", gcd(48, 18));
    printf("lcm(48, 18) = %d\n", lcm(48, 18));

    // Test array operations
    int arr[] = { 1, 5, 3, 4, 2 };
    printf("array_sum = %d\n", array_sum(arr, 5));
    array_sort(arr, 5);
    printf("array_sorted = ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}