#pragma once
#define FD_MAX_PER_PROCESS 100
#define FD_MAX_TOTAL 4096

// Open mode flags
#define O_RDONLY       0x0000  // Read-only
#define O_WRONLY       0x0001  // Write-only
#define O_RDWR         0x0002  // Read-Write

// Modifier flags
#define O_CREAT        0x0100  // Create if file does not exist
#define O_EXCL         0x0200  // Exclusive use (with O_CREAT)
#define O_APPEND       0x0400  // Append to file
#define O_NONBLOCK     0x0800  // Non-blocking mode
#define O_SYNC         0x1000  // Synchronous writes
#define O_NOFOLLOW     0x2000  // Do not follow symbolic links


#define FD_PERM_READ    0x100  // File read permission
#define FD_PERM_WRITE   0x200  // File write permission
#define FD_PERM_EXECUTE 0x400  // File execute permission

/// status
#define FD_CLOSED   0x800   // File descriptor is closed
#define FD_OPEN     0x1000  // File descriptor is actively open
#define FD_LOCKED   0x2000  // File descriptor is locked (exclusive access)

/// types
#define FD_TYPE_FILE  0x01  // Regular file
#define FD_TYPE_DIR   0x02  // Directory
#define FD_TYPE_PIPE  0x04  // Pipe
#define FD_TYPE_SOCKET 0x08 // Socket
#define FD_TYPE_TERM 0x16   // stdin, out, err

/// flags
#define FD_EOF   0x01  // Reached end of file
#define FD_ERROR 0x02  // An error has occurred


typedef struct {
    // this may be -1 if this has not been initialized (added to fd_table) yet
    int file_descriptor;
    short sectors;
    int flags;
    int permissions;
    int status;
    short type;
} fd_t;

void close_file_descriptor(int fd);
int open_file_descriptor(short type, int flags, int permissions);
fd_t *get_file_descriptor(int fd);
int init_file_descriptors();