#include "minilibc.h"

// 外部全局变量声明
extern int global_var;

// 返回一个固定值
int get_value(void)
{
    return 58; // 58 + 42 = 100
}

// 打印值
void print_value(int x)
{
    printf("%d\n", x);
}