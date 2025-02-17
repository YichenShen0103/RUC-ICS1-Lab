#include "mathlib.h"

// Internal helper functions
int _is_prime(int n)
{
    if (n <= 1)
        return 0;
    if (n <= 3)
        return 1;
    if (n % 2 == 0 || n % 3 == 0)
        return 0;

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    }
    return 1;
}

void _swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int _array_partition(int* arr, int low, int high)
{
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            _swap(&arr[i], &arr[j]);
        }
    }
    _swap(&arr[i + 1], &arr[high]);
    return i + 1;
}