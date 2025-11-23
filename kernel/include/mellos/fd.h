#pragma once
#define FD_MAX_PER_PROCESS 100
#define FD_MAX_TOTAL 4096

/// Open mode flags
#define O_RDONLY 0x0000 // Read-only
#define O_WRONLY 0x0001 // Write-only
#define O_RDWR 0x0002   // Read-Write

/// Modifier flags
#define O_CREAT 0x0100    // Create if file does not exist
#define O_EXCL 0x0200     // Exclusive use (with O_CREAT)
#define O_APPEND 0x0400   // Append to file
#define O_NONBLOCK 0x0800 // Non-blocking mode
#define O_SYNC 0x1000     // Synchronous writes
#define O_NOFOLLOW 0x2000 // Do not follow symbolic links

/// fd permissions
#define FD_PERM_READ 0x100    // File read permission
#define FD_PERM_WRITE 0x200   // File write permission
#define FD_PERM_EXECUTE 0x400 // File execute permission

/// status
#define FD_CLOSED 0x800  // File descriptor is closed
#define FD_OPEN 0x1000   // File descriptor is actively open
#define FD_LOCKED 0x2000 // File descriptor is locked (exclusive access)

typedef enum {
	FD_TYPE_FILE,
	FD_TYPE_DIR,
	FD_TYPE_PIPE,
	FD_TYPE_SOCKET,
	FD_TYPE_DEVICE,
	FD_TYPE_CHAR,
	FD_TYPE_NULL,
} fd_type_t;

/// flags
#define FD_EOF 0x01   // Reached end of file
#define FD_ERROR 0x02 // An error has occurred

#define MAX_OPEN_FILES 2048
#define MAX_OPEN_FILES_PROCESS 64
#define NULL_FILE "/dev/null"

/**
 * @brief File descriptor structure.
 *
 * This structure represents a file descriptor, which is a reference to an open file or other
 * resource. It contains information about the file descriptor's name, flags, status, type, and
 * private data.
 */
typedef struct {
	char* name;
	int flags;
	int status;
	fd_type_t type;
	/**
	 * @brief A pointer to private data associated with a file descriptor.
	 *
	 * This variable stores a pointer to data that is specific to a particular file descriptor.
	 * The data pointed to by this variable is dependent on the fd_type_t and
	 * can also include structures or other resources necessary for managing the descriptor's
	 * behavior.
	 *
	 * In the case of a pipe file descriptor, `private_data` points to a `pipe_t` structure which
	 * includes a circular buffer, state flags, and other information describing the pipe's state
	 * and behavior.
	 */
	void* private_data;
} fd_t;

// 2 bytes
typedef struct {
	unsigned int count;
	fd_t* fd_array;
} fd_table_t;

// process includes this alr
typedef struct process process_t;
typedef struct mount mount_t;

fd_t* open_fd_standalone(fd_type_t type, mount_t* mount, process_t* process, int flags,
                         int permissions, char* path);
fd_t* get_file_descriptor(int fd);
int find_first_free_fd_PID(unsigned int process);
int find_first_free_fd();
int find_first_free_file_PID(unsigned int process);