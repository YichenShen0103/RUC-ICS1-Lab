extern const char str[];

void _start()
{
    int ans = 0;

    for (int i = 0; i < 10; i++) {
        ans += str[i];
    }

    // Exit directly using syscall
    asm volatile(
        "mov %0, %%edi\n" // First argument (exit code) in edi
        "mov $60, %%eax\n" // syscall number for exit (60)
        "syscall" // Make the syscall
        : // no outputs
        : "r"(ans) // input: our result variable
        : "eax", "edi" // clobbered registers
    );
}