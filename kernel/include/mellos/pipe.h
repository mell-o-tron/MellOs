#pragma once
#include "mellos/fd.h"
#include "circular_queue.h"
#define PIPE_BUFFER_SIZE 1024

#define O_PIPE_BLOCKING 0x1
#define O_PIPE_OPEN 0x2
#define O_PIPE_BROKEN 0x4
#define O_PIPE_QUIET 0x8  // does not print errors, only applies when non-blocking
#define O_PIPE_NONBLOCKING 0x10

#define IS_PIPE_OPEN(pipe) ((pipe)->flags & O_PIPE_OPEN)
#define IS_PIPE_BROKEN(pipe) ((pipe)->flags & O_PIPE_BROKEN)
#define IS_PIPE_BLOCKING(pipe) \
        (((pipe)->flags & O_PIPE_BLOCKING) ? O_PIPE_BLOCKING : ((pipe)->flags & O_PIPE_NONBLOCKING) ? O_PIPE_NONBLOCKING : 0)

typedef struct {
    CircularQueue *buffer;
    int flags;
    char is_open;
    fd_t fd;
    fd_t other_fd;
} pipe_t;


pipe_t *open_pipe(int fd, int other_fd, int buffer_size);
// return how much was actually written
int write_to_pipe(pipe_t *pipe, const char *data, int size);
int write_string_to_pipe(pipe_t *pipe, char* str);
char* read_from_pipe(pipe_t *pipe, int size);