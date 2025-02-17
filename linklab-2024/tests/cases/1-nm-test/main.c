// Global variables (strong)
int global_var = 42;
char global_array[] = "Hello";

// Weak global variable
__attribute__((weak)) int weak_var = 100;

// Local function (static)
static int local_func(int x)
{
    return x + 1;
}

// Global function
int global_func(void)
{
    return local_func(global_var);
}

// Weak function
__attribute__((weak)) void weak_func(void)
{
    weak_var = 200;
}

int main()
{
    return global_func();
}