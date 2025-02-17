void _start()
{
    asm volatile(
        "mov $100, %edi\n" // exit code 100
        "mov $60, %eax\n" // syscall number for exit
        "syscall");
}