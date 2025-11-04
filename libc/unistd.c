#include "unistd.h"
#include "stdint.h"
#include "stddef.h"

long syscall0(long number) {
    long ret;
    register long b asm("ebx") = 0;
    register long c asm("ecx") = 0;
    register long d asm("edx") = 0;
    register long S asm("esi") = 0;
    register long D asm("edi") = 0;
    asm volatile (
        "int $0x80\n"
        : "=a"(ret)
        : "a"(number), "b"(b), "c"(c), "d"(d), "S"(S), "D"(D)
        : "memory"
    );
    return ret;
}

long syscall1(long number, long arg1) {
    long ret;
    register long c asm("ecx") = 0;
    register long d asm("edx") = 0;
    register long S asm("esi") = 0;
    register long D asm("edi") = 0;
    asm volatile (
        "int $0x80\n"
        : "=a"(ret)
        : "a"(number), "b"(arg1), "c"(c), "d"(d), "S"(S), "D"(D)
        : "memory"
    );
    return ret;
}

long syscall2(long number, long arg1, long arg2) {
    long ret;
    register long d asm("edx") = 0;
    register long S asm("esi") = 0;
    register long D asm("edi") = 0;
    asm volatile (
        "int $0x80\n"
        : "=a"(ret)
        : "a"(number), "b"(arg1), "c"(arg2), "d"(d), "S"(S), "D"(D)
        : "memory"
    );
    return ret;
}

long syscall3(long number, long arg1, long arg2, long arg3) {
    long ret;
    register long S asm("esi") = 0;
    register long D asm("edi") = 0;
    asm volatile (
        "int $0x80\n"
        : "=a"(ret)
        : "a"(number), "b"(arg1), "c"(arg2), "d"(arg3), "S"(S), "D"(D)
        : "memory"
    );
    return ret;
}

long syscall4(long number, long arg1, long arg2, long arg3, long arg4) {
    long ret;
    register long D asm("edi") = 0;
    asm volatile (
        "int $0x80\n"
        : "=a"(ret)
        : "a"(number), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4), "D"(D)
        : "memory"
    );
    return ret;
}

long syscall5(long number, long arg1, long arg2, long arg3, long arg4, long arg5) {
    long ret;
    asm volatile (
        "int $0x80\n"
        : "=a"(ret)
        : "a"(number), "b"(arg1), "c"(arg2), "d"(arg3), "S"(arg4), "D"(arg5)
        : "memory"
    );
    return ret;
}

int syscall_write(int fd, const char *buf, size_t count) {
    return (int)syscall3(SYS_WRITE, (long)fd, (long)(uintptr_t)buf, (long)count);
}

int syscall_read(int fd, char *buf, size_t count) {
    return (int)syscall3(SYS_READ, (long)fd, (long)(uintptr_t)buf, (long)count);
}

int syscall_exit(int status) {
    return (int)syscall1(SYS_EXIT, (long)status);
}

int syscall_fork() {
    return (int)syscall0(SYS_FORK);
}

int syscall_getpid() {
    return (int)syscall0(SYS_GETPID);
}

void* syscall_mmap(unsigned long size) {
    long addr = syscall2(SYS_MMAP, 0L, (long)size);
    if (addr == 0) return (void*)0;
    return (void*)(uintptr_t)addr;
}

int syscall_munmap(void* base, unsigned long size) {
    return (int)syscall2(SYS_MUNMAP, (long)(uintptr_t)base, (long)size);
}

int syscall_mprotect(void* base, unsigned long size, int prot) {
    return (int)syscall3(SYS_MPROTECT, (long)(uintptr_t)base, (long)size, (long)prot);
}

int syscall_malloc(size_t size) {
    return (int)(uintptr_t)syscall_mmap(size);
}

int syscall_free(void *ptr) {
    // size unknown: kernel will look up region by base
    return (int)syscall3(SYS_MMAP, 1L, (long)(uintptr_t)ptr, 0L);
}