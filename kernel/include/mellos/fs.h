#pragma once
#include "block_device.h"
#include "statfs.h"

#define S_IFMT 00170000  /* File type mask */
#define S_IFSOCK 0140000 /* Socket */
#define S_IFLNK 0120000  /* Symbolic link */
#define S_IFREG 0100000  /* Regular file */
#define S_IFBLK 0060000  /* Block device */
#define S_IFDIR 0040000  /* Directory */
#define S_IFCHR 0020000  /* Character device */
#define S_IFIFO 0010000  /* FIFO/pipe */

#define S_ISUID 0004000 /* Set UID bit */
#define S_ISGID 0002000 /* Set GID bit */
#define S_ISVTX 0001000 /* Sticky bit */

#define S_IRUSR 0000400 /* User has read permission */
#define S_IWUSR 0000200 /* User has write permission */
#define S_IXUSR 0000100 /* User has execute permission */

#define S_IRGRP 0000040 /* Group has read permission */
#define S_IWGRP 0000020 /* Group has write permission */
#define S_IXGRP 0000010 /* Group has execute permission */

#define S_IROTH 0000004 /* Others have read permission */
#define S_IWOTH 0000002 /* Others have write permission */
#define S_IXOTH 0000001 /* Others have execute permission */

#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

#define RAMFS_IDENTIFIER 230432
#define RAMFS_BLOCK_SIZE 4096
// todo: this is just for demo
#define RAMFS_MAX_BLOCKS 1024

typedef struct inode inode_t;
typedef struct file file_t;
typedef struct superblock superblock_t;

/**
 * Represents a mounted filesystem
 */
typedef struct mount {
	/**
	 * Represents the root inode of the specific mount (root directory)
	 */
	inode_t* root;
	bool mounted;
	superblock_t* sb;
} vfs_mount_t;

/**
 * Filesystem type descriptor
 * Registered globally: each filesystem implementation provides one.
 */
typedef struct {
	/* e.g., ext2, ramfs, ... */
	const char* name;

	/**
	 * This is expected to allocate resources.
	 * RamFS is the only driver that *does not*
	 * take the mount inode. You will get NULL
	 * back and errno indicating invalid arguments
	 * if you try to give it a non-NULL one.
	 *
	 * @return Mount point of the newly mounted
	 * block device.
	 */
	vfs_mount_t* (*mount)(block_device_t* dev, const char* mount_point, void* data);
	/**
	 * This is expected to free the resources allocated by mount.
	 */
	int (*unmount)(vfs_mount_t* sb);
} fs_type_t;

/**
 * Superblock operations (1/ mounted instance)
 */
typedef struct {

	/**
	 * Initializes an inode. Can be left as NULL
	 */
	inode_t* (*allocate_inode)(superblock_t* sb);
	/**
	 * Destroys an inode. Required only if allocate_inode is not NULL.
	 */
	void (*destroy_inode)(inode_t* inode);
	int (*sync)(superblock_t* sb);
	/**
	 * Since filesystem sectors / blocks are not always the same size as
	 * the block device blocks, this needs to return the data directly
	 * from the driver, this is usually stored in the filesystem
	 * header (see example in FAT driver),
	 * so do not use sb->bd->*
	 */
	int (*statfs)(superblock_t* sb, statfs_t* st);
} super_ops_t;

/**
 * VNode operations (operations on directory entries / file nodes)
 */
typedef struct {
	/**
	 * Reads a directory
	 */
	int (*lookup)(inode_t* dir, const char* name, inode_t** out);
	/**
	 * Creates a file. Use mkdir for directories.
	 * This should return EINVAL if mode & S_IFDIR
	 */
	int (*create)(inode_t* dir, const char* name, uint32_t mode, inode_t** out);
	/**
	 * Creates a directory. this should return
	 * EINVAL if !(mode & S_IFDIR)
	 */
	int (*mkdir)(inode_t* dir, const char* name, uint32_t mode);
	int (*unlink)(inode_t* dir, const char* name);
	int (*readlink)(inode_t* vn, char* buf, size_t size);
	/**
	 * Creates a soft link
	 */
	int (*symlink)(inode_t* dir, const char* name, const char* target);
	/**
	 * Creates a hard link
	 */
	int (*link)(inode_t* dir, const char* name, inode_t* target);

	/**
	 * gets an inode relative to another inode
	 */
	inode_t* (*inode_get_relative)(inode_t rel, const char* path);

	/**
	 * gets an inode relative to the filesystem root
	 */
	inode_t* (*inode_get_path)(const char* path);
} inode_ops_t;

typedef struct dentry_ops dentry_ops_t;
/**
 * Cache of a dentry or file in the filesystem.
 */
typedef struct dentry {
	/**
	 * The parent directory entry, NULL if this is the root directory.
	 */
	struct dentry* parent;
	/**
	 * The inode, NULL if this dentry has been accessed but the file does not exist!
	 */
	inode_t* inode;
	/**
	 * Stores the name of a directory entry or file in the filesystem.
	 * Includes the path up to the superblock root.
	 *
	 * For example, if you have a separate filesystem in your home directory, you would
	 * do /Desktop/important.txt
	 *
	 * use rebuild_path to get string up to root
	 */
	char* name;

	dentry_ops_t* dops;

	/**
	 * How many processes are currently accessing this dentry. Also add if the filesystem is
	 * traversing it.
	 * If it hits 0 this dentry may be removed freed.
	 */
	uint32_t refcount;
} dentry_t;

struct dentry_ops {
	/**
	 * Creates a new dentry.
	 */
	int (*dentry_init)(dentry_t* dentry, inode_t* inode);

	dentry_t* (*dentry_alloc)(dentry_t* parent, char* name);
	/**
	 * Deletes/frees the dentry
	 */
	int (*dentry_delete)(dentry_t* dentry);
};

/**
 * File operations (per open file handle)
 */
typedef struct {
	ssize_t (*read)(file_t* f, void* buf, size_t size, uint64_t offset);
	ssize_t (*write)(file_t* f, const void* buf, size_t size, uint64_t offset);
	size_t (*readdir)(file_t* f, void* dirent_out);
	/**
	 * Change the size of a file, either by extending it or cutting it's tail off.
	 */
	size_t (*truncate)(inode_t* vn, uint64_t new_size);
	size_t (*ioctl)(file_t* f, unsigned long cmd, void* arg);
	size_t (*mmap)(file_t* f, void* addr, size_t length, int prot, int flags);
} file_ops_t;

/**
 * Kernel superblock structure (per mounted filesystem)
 */
struct superblock {
	fs_type_t* fs;      /* filesystem type descriptor */
	block_device_t* bd; /* Mounted block device */
	super_ops_t* ops;     /* superblock operations */
	inode_t* root;      /* root inode of this mount */
	uint32_t identifier;
	uint32_t block_size;
	uint64_t total_blocks;
	uint64_t flags;

	void* private; /* filesystem-specific data */
};

/**
 * Kernel INode structure (represents a directory entry or file)
 */
typedef struct inode {
	superblock_t* sb;
	inode_ops_t* ops;
	file_ops_t* fops;
	inode_t* parent;
	/**
	 * Dentry gets allocated on access, use get_or_create_dentry for getting it safely
	 * maybe this should be a function which calls that?
	 */
	dentry_t* dentry;

	uint32_t mode; /* permissions and type */
	uint64_t size; /* logical size */

	uint32_t ref_count;

	void* private; /* filesystem specific inode/znode/etc */
} inode_t;

/**
 * Kernel file structure (per open handle)
 */
struct file {
	inode_t* inode;
	/**
	 * Different for different file types
	 */
	file_ops_t* ops;

	uint64_t position;
	void* private; /* FS-specific per-open data */
};

/**
 * Rebuild the inode path as a string
 * goes up from the inode by following the parent
 * so it may or may not use a hard link,
 * it depends on the filesystem that created the
 * node.
 *
 * @param inode The node to end to
 * @param buffer The buffer to write to
 * @return success
 */
int rebuild_path(inode_t* inode, char** buffer);

/**
 * Gets an inode for a file from a unix style path.
 *
 * @param inode of the directory for which to do relative search
 * @param path Path to the file that we are trying to retrieve the inode for.
 * @return Inode for the file defined by path
 */
inode_t* get_inode_from_path_relative(inode_t* inode, const char* path);

/**
 * Shortcut to use get_inode_from_path_relative with root.
 *
 * @param path Path to the file that we are trying to retrieve the inode for.
 * @return Inode for the file defined by path
 */
inode_t* get_inode_from_path(const char* path);

bool free_dentry(dentry_t* dentry);
