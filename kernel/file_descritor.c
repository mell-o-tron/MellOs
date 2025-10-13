#include <dynamic_mem.h>
#include <errno.h>
#include <mem.h>
#include <processes.h>
#include <string.h>

#include "mellos/fd.h"

#define MAX_OPEN_FILES 2048
#define MAX_OPEN_FILES_PROCESS 64
#define NULL_FILE "/dev/null"

open_file_t open_files[MAX_OPEN_FILES];
fd_t open_file_descriptors[FD_MAX_TOTAL];


fd_t *open_fd_standalone(int type, int flags, int permissions, char* path) {
    const int fdid = find_first_free_fd();
    if (fdid < 0) {
        errno = ENFILE; // system wide limit reached
        return NULL;
    }
    if (open_file_descriptors[fdid].name != NULL) {
        errno = EBUSY;
        return NULL;
    }

    open_file_descriptors[fdid].type = type;
    open_file_descriptors[fdid].flags = flags;
    switch (type) {
        case FD_TYPE_NULL:
            if (!strcmp(path, NULL_FILE)) {
                errno = EINVAL;
                return NULL;
            }
            const int32_t file_id = find_first_free_file();
            open_file_t *null_file = open_file(NULL_FILE, FD_TYPE_NULL);
            if (null_file == NULL) {
                null_file = kmalloc(sizeof(open_file_t));
                null_file->type = FD_TYPE_NULL;
                null_file->name = strdup(NULL_FILE);
                null_file->ref_count = 1;
                open_files[file_id] = *null_file;
                open_file_descriptors[fdid].resource = null_file;
            }

            open_files[fdid].type = FD_TYPE_NULL;
            open_file_descriptors[fdid].name = "null_fd";
            open_file_descriptors[fdid].resource = null_file;
            break;
        case FD_TYPE_PIPE:
            open_file_descriptors[fdid].name = "pipe_fd";
            open_file_descriptors[fdid].resource = kmalloc(sizeof(pipe_t));
            break;
        case FD_TYPE_FILE:
            open_file_descriptors[fdid].name = "file_fd";
            open_file_descriptors[fdid].resource = kmalloc(sizeof(open_file_t));
            ((open_file_t *)open_file_descriptors[fdid].resource)->permissions = permissions;
            ((open_file_t *)open_file_descriptors[fdid].resource)->name = path;
            ((open_file_t *)open_file_descriptors[fdid].resource)->ref_count += 1;
            break;
        default: return NULL;
    }
    return &open_file_descriptors[fdid];
}

int find_first_free_file() {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i].name == NULL) {
            return i;
        }
    }
    return -ENFILE;
}

int find_first_free_file_PID(uint32_t pid) {
    const process_t * process = get_process_by_pid(pid);
    
    for (int i = 0; i < MAX_OPEN_FILES_PROCESS; i++) {
        if (process->fd_table.fd_array[i].name == NULL) {
            if (i > process->fd_table.count) {
                krealloc(process->fd_table.fd_array,
                    sizeof(fd_t) * process->fd_table.count,
                    sizeof(fd_t) * (i + 1));
            }
        }
    }
    return -EMFILE;
}

int find_first_free_fd() {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_file_descriptors[i].name == NULL) {
            return i;
        }
    }
    return -1;
}

int find_first_free_fd_PID(uint32_t pid) {
    const process_t * process = get_process_by_pid(pid);
    
    for (int i = 0; i < MAX_OPEN_FILES_PROCESS; i++) {
        if (process->fd_table.fd_array[i].name == NULL) {
            if (i > process->fd_table.count) {
                krealloc(process->fd_table.fd_array,
                    sizeof(fd_t) * process->fd_table.count,
                    sizeof(fd_t) * (i + 1));
            }
            return i;
        }
    }
    return -EMFILE;
}

open_file_t *open_file(char *path, int type) {
    if (path == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (type == FD_TYPE_PIPE) {
        errno = EINVAL;
        return NULL;
    }
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_files[i].name != NULL && !strcmp(open_files[i].name, path)) {
            open_files[i].ref_count += 1;
            return &open_files[i];
        }
    }

    int file_id = find_first_free_file();
    if (file_id < 0) {
        errno = -file_id;
        return NULL;
    }

    switch (type) {
        case FD_TYPE_NULL:
            open_files[file_id].type = FD_TYPE_NULL;
            open_files[file_id].name = strdup(path);
            open_files[file_id].ref_count = 1;
            return &open_files[file_id];

        case FD_TYPE_VIRTUAL:
        case FD_TYPE_FILE:
        case FD_TYPE_DIR:
        case FD_TYPE_SOCKET:
            open_files[file_id].type = type;
            open_files[file_id].name = strdup(path);
            open_files[file_id].ref_count = 1;
            return &open_files[file_id];

        default:
            errno = EINVAL;
            return NULL;
    }
}

int is_file_in_use(open_file_t *file) {
    for (int i = 0; i < FD_MAX_TOTAL; i++) {
        if (open_file_descriptors[i].resource == file &&
            open_file_descriptors[i].type == FD_TYPE_FILE) {
            return 1;
        }
    }
    return 0;
}

int close_file(open_file_t *file) {
    if (file == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (is_file_in_use(file)) {
        file->ref_count--;
        return 0;
    }

    if (file->name != NULL) {
        kfree(file->name);
        file->name = NULL;
    }

    file->permissions = 0;
    file->ref_count = 0;

    return 0;
}


fd_t *get_file_descriptor(int fd) {
    return &open_file_descriptors[fd];
}