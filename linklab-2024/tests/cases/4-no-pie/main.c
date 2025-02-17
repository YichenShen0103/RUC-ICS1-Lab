#include "minilibc.h"

// 全局变量，用于测试绝对寻址
int global_var = 42;

// 外部函数声明
int get_value(void);
void print_value(int);

int main()
{
    // 调用外部函数，测试相对寻址（函数调用）
    int value = get_value();

    // 访问全局变量，测试绝对寻址
    value += global_var;

    // 再次调用外部函数
    print_value(value);

    return value;
}