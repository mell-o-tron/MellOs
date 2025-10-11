#include "mellos/pipe.h"

#include <dynamic_mem.h>
#include "circular_queue.h"

#include "../global/include/errno.h"

pipe_t *open_pipe(int fd1, int fd2, int buffer_size) {
    pipe_t *pipe = kmalloc(sizeof(pipe_t));

    if (pipe == NULL) {
        // todo: signal for out of memory
        return NULL;
    }

    pipe->buffer = cqueue_init(buffer_size);
    pipe->fd = *get_file_descriptor(fd1);
    pipe->other_fd = *get_file_descriptor(fd2);
    pipe->is_open = true;
    pipe->flags |= O_PIPE_BLOCKING; // pipes block by default

    return pipe;
}

int write_string_to_pipe(pipe_t *pipe, char* str, const uint32_t from_pid) {
    int i = 0;
    while (str[i] != '\0') {
        write_to_pipe(pipe, str + i, 1, from_pid);
        i++;
    }
    return i;
}

int write_to_pipe(pipe_t *pipe, const char *data, const uint32_t size, const uint32_t from_pid) {
    if (IS_PIPE_BROKEN(pipe)) {
        // todo: send SIGPIPE to proc
        cqueue_destroy(pipe->buffer);
        return -EPIPE;
    }
    if (!IS_PIPE_OPEN(pipe)) {
        kfree(pipe);
        return -EBADF;
    }
    for (int i = 0; i < size; i++) {
        if (cqueue_get_free_space(pipe->buffer) == 0) {
            if (IS_PIPE_BLOCKING(pipe)) {
                // todo: tell scheduler to continue to the next program/thread
            }
        }
        if (IS_PIPE_BLOCKING(pipe)) {
            cqueue_enqueue(pipe->buffer, (void*)data+i);
        }
    }

    return 0;
}