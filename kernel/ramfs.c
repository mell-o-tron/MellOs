#include "mellos/ramfs.h"

#include "errno.h"
#include "stddef.h"
#include "string.h"


extern open_file_t open_files[];

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

    const int file_id = find_first_free_file();
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

int set_file_mode(open_file_t *file, int mode) {
    // todo: more error checking
    if (file == NULL) {
        errno = EINVAL;
        return -1;
    }
    file->permissions = mode;
    return 0;
}
