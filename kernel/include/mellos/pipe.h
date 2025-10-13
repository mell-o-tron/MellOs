#pragma once
#include "spinlock.h"

#include "mellos/fd.h"
#define PIPE_BUFFER_SIZE 1024

#define O_PIPE_BLOCKING 0x4000
#define O_PIPE_OPEN 0x8000
#define O_PIPE_BROKEN 0x10000
#define O_PIPE_QUIET 0x20000  // does not print errors, only applies when non-blocking
#define O_PIPE_NONBLOCKING 0x40000

#define IS_PIPE_OPEN(pipe) ((pipe)->flags & O_PIPE_OPEN)
#define IS_PIPE_BROKEN(pipe) ((pipe)->flags & O_PIPE_BROKEN)
#define IS_PIPE_BLOCKING(pipe) \
        (((pipe)->flags & O_PIPE_BLOCKING) ? O_PIPE_BLOCKING : ((pipe)->flags & O_PIPE_NONBLOCKING) ? O_PIPE_NONBLOCKING : 0)

typedef struct {
    cbuffer_t *buffer;
    int flags;
    char is_open;
    spinlock_t lock;
} pipe_t;


ssize_t pipe_write(fd_t *fd, const void *buf, size_t count);
ssize_t pipe_read(fd_t *fd, void *buf, size_t count);
pipe_t *open_pipe(fd_t *read_fd, fd_t *write_fd, int buffer_size);
