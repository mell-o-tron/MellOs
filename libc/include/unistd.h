#pragma once

#include <stddef.h>
#include <stdint.h>

#define SYS_EXIT 1
#define SYS_FORK 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_GETPID 7
#define SYS_MMAP 8
#define SYS_MUNMAP 9
#define SYS_MPROTECT 10
#define SYS_MEMORY 8 // legacy alias to SYS_MMAP

// Generic syscall helpers with different argument counts (use long-sized args)
long syscall0(long number);
long syscall1(long number, long arg1);
long syscall2(long number, long arg1, long arg2);
long syscall3(long number, long arg1, long arg2, long arg3);
long syscall4(long number, long arg1, long arg2, long arg3, long arg4);
long syscall5(long number, long arg1, long arg2, long arg3, long arg4, long arg5);

int syscall_exit(int status);
int syscall_write(int fd, const char *buf, size_t count);
int syscall_read(int fd, char *buf, size_t count);

int syscall_fork();
int syscall_getpid();

// Memory syscalls
void* syscall_mmap(unsigned long size);
int syscall_munmap(void* base, unsigned long size);
int syscall_mprotect(void* base, unsigned long size, int prot);

// Legacy helpers
int syscall_malloc(unsigned long size);
int syscall_free(void *ptr);

