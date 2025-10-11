#include "dynamic_mem.h"
#include "../global/include/errno.h"

#include "mellos/fd.h"

fd_t *fd_table[FD_MAX_TOTAL];

int fd_count = 0;

// todo: permissions

int init_file_descriptors() {
    if (open_file_descriptor(FD_TYPE_TERM, O_RDONLY, FD_PERM_READ) != 0) return -ENOMEM;
    if (open_file_descriptor(FD_TYPE_TERM, O_WRONLY, FD_PERM_WRITE) != 1) return -ENOMEM;
    if (open_file_descriptor(FD_TYPE_TERM, O_WRONLY, FD_PERM_WRITE) != 2) return -ENOMEM;
    return 0;
}

fd_t *get_file_descriptor(int fd) {
    fd_t *file = fd_table[fd];
    return file;
}

int open_file_descriptor(short type, int flags, int permissions) {
    for (int i = 0; i < FD_MAX_TOTAL; i++) {
        if (fd_table[i] == NULL) {
            fd_table[i] = (fd_t*)kmalloc(sizeof(fd_t));
            if (fd_table[i] == NULL) {
                return -ENOMEM;
            }
            fd_table[i]->type = type;
            fd_table[i]->flags = flags;
            fd_table[i]->permissions = permissions;
            fd_count++;
            return i;
        }
    }
    return -ENFILE; // global fd limit reached
}

void close_file_descriptor(int fd) {
    if (fd < 0 || fd >= FD_MAX_TOTAL || fd_table[fd] == NULL) return;
    kfree(fd_table[fd]);
    fd_count--;
}
