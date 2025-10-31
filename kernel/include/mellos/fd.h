#pragma once
#define FD_MAX_PER_PROCESS 100
#define FD_MAX_TOTAL 4096

/// Open mode flags
#define O_RDONLY       0x0000  // Read-only
#define O_WRONLY       0x0001  // Write-only
#define O_RDWR         0x0002  // Read-Write

/// Modifier flags
#define O_CREAT        0x0100  // Create if file does not exist
#define O_EXCL         0x0200  // Exclusive use (with O_CREAT)
#define O_APPEND       0x0400  // Append to file
#define O_NONBLOCK     0x0800  // Non-blocking mode
#define O_SYNC         0x1000  // Synchronous writes
#define O_NOFOLLOW     0x2000  // Do not follow symbolic links

/// fd permissions
#define FD_PERM_READ    0x100  // File read permission
#define FD_PERM_WRITE   0x200  // File write permission
#define FD_PERM_EXECUTE 0x400  // File execute permission

/// status
#define FD_CLOSED   0x800   // File descriptor is closed
#define FD_OPEN     0x1000  // File descriptor is actively open
#define FD_LOCKED   0x2000  // File descriptor is locked (exclusive access)

/// types
#define FD_TYPE_FILE    0b1  // Regular file
#define FD_TYPE_DIR     0b10  // Directory
#define FD_TYPE_PIPE    0b100  // Pipe
#define FD_TYPE_SOCKET  0b1000 // Socket
#define FD_TYPE_VIRTUAL 0b10000 // ramdisk stuff
#define FD_TYPE_NULL    0b100000 // Null device

/// flags
#define FD_EOF   0x01  // Reached end of file
#define FD_ERROR 0x02  // An error has occurred


#define MAX_OPEN_FILES 2048
#define MAX_OPEN_FILES_PROCESS 64
#define NULL_FILE "/dev/null"

// 20 bytes
typedef struct {
    char *name;
    int flags;
    int status;
    int type;
    void *resource; // ptr to the actual resource we are using this for e.g., pipe_t
} fd_t;

// 2 bytes
typedef struct {
    unsigned int count;
    fd_t *fd_array;
} fd_table_t;

// 20 bytes
typedef struct {
    char *name;
    int permissions;
    int flags;
    int ref_count;
    int type;
} open_file_t;


fd_t *open_fd_standalone(int type, int flags, int permissions, char* path);
fd_t *get_file_descriptor(int fd);
int find_first_free_fd_PID(unsigned int process);
int find_first_free_fd();
int find_first_free_file_PID(unsigned int process);
int find_first_free_file();