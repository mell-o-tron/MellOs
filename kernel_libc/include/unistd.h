#pragma once

#include "stdint.h"

// remember to update the userspace unistd if you update the syscalls
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

#define SYS_MMAP_MAP 0x01

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

