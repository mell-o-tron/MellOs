#pragma once

#define SYS_EXIT 1
#define SYS_FORK 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_GETPID 7
#define SYS_MEMORY 8

static int syscall(int number, ...);

int syscall_exit(int status);
int syscall_write(int fd, const char *buf, long unsigned int count);
int syscall_read(int fd, char *buf, long unsigned int count);


int syscall_fork();
int syscall_getpid();

