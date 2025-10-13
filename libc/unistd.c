#include "unistd.h"
#include "stdint.h"
#include "stddef.h"

static inline int syscall(int number, ...) {
    int ret;
    va_list args;
    va_start(args, number);

    asm volatile (
        "movl %1, %%eax\n"
        "movl %2, %%ebx\n" 
        "movl %3, %%ecx\n"
        "int $0x80"
        : "=a"(ret)
        : "r"(number), "r"(va_arg(args, int)), "r"(va_arg(args, int))
        : "%ebx", "%ecx"
    );

    va_end(args);
    return ret;
}

int syscall_write(int fd, const char *buf, size_t count) {
    return syscall(SYS_WRITE, fd, buf, count);
}

int syscall_read(int fd, char *buf, size_t count) {
    return syscall(SYS_READ, fd, buf, count);
}

int syscall_exit(int status) {
    return syscall(SYS_EXIT, status);
}

int syscall_fork() {
    return syscall(SYS_FORK);
}

int syscall_getpid() {
    return syscall(SYS_GETPID);
}

int syscall_malloc(size_t size) {
    return syscall(SYS_MEMORY, 0, size);
}

int syscall_free(void *ptr) {
    return syscall(SYS_MEMORY, 1, ptr);
}