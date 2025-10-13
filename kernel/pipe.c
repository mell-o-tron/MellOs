#include "mellos/pipe.h"

#include <dynamic_mem.h>

#include "errno.h"

pipe_t *open_pipe(fd_t *read_fd, fd_t *write_fd, int buffer_size) {
    pipe_t *pipe = kmalloc(sizeof(pipe_t));

    if (pipe == NULL) {
        // todo: signal for out of memory
        return NULL;
    }

    pipe->buffer = kmalloc(sizeof(cbuffer_t));
    pipe->buffer->size = buffer_size;
    pipe->buffer->array = kmalloc(buffer_size);
    pipe->is_open = true;
    pipe->flags |= O_PIPE_BLOCKING; // pipes block by default

    read_fd->type = FD_TYPE_PIPE;
    read_fd->resource = pipe;
    write_fd->type = FD_TYPE_PIPE;
    write_fd->resource = pipe;

    read_fd->flags |= O_RDONLY;
    write_fd->flags |= O_WRONLY;

    return pipe;
}

ssize_t pipe_write(fd_t *fd, const void *buf, size_t count) {
    if (!fd || fd->type != FD_TYPE_PIPE) {
        return -EINVAL;
    }
    if (!(fd->flags & O_WRONLY)) {
        return -EACCES;
    }



    pipe_t *pipe = (pipe_t *)fd->resource;

    spinlock_lock(&pipe->lock);

    ssize_t bytes_written = 0;
    for (size_t i = 0; i < count; i++) {
        if (&pipe->buffer->top == &pipe->buffer->bot) {
            break;
        }
        add_to_cbuffer(pipe->buffer, ((char*)buf)[i], false);
        bytes_written++;
    }

    spinlock_unlock(&pipe->lock);

    return bytes_written;
}

ssize_t pipe_read(fd_t *fd, void *buf, size_t count) {
    if (!fd || fd->type != FD_TYPE_PIPE) {
        return -EINVAL;
    }
    if (!(fd->flags & O_RDONLY)) {
        return -EACCES;
    }

    pipe_t *pipe = (pipe_t *)fd->resource;

    spinlock_lock(&pipe->lock);

    ssize_t bytes_read = 0;
    for (size_t i = 0; i < count; i++) {
        if (&pipe->buffer->top == &pipe->buffer->bot) {
            goto cleanup;
        }
        ((char*)buf)[i] = get_from_cbuffer(pipe->buffer);
        bytes_read++;
    }

cleanup:

    spinlock_unlock(&pipe->lock);

    return bytes_read;
}