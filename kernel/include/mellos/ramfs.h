#pragma once
#include "fs.h"
#include "mellos/fd.h"
#include "stddef.h"
#include "stdbool.h"

int ramfs_create_dir(inode_t* dir, const char* name, uint32_t mode, inode_t** out);

/**
 * Reads a directory
 */
int ramfs_lookup(inode_t* dir, const char* name, inode_t** out);
int ramfs_mkdir(inode_t* dir, const char* name, uint32_t mode);
int ramfs_unlink(inode_t* dir, const char* name);
int ramfs_readlink(inode_t* vn, char* buf, size_t size);
/**
 * Creates a soft link
 */
int ramfs_symlink(inode_t* dir, const char* name, const char* target);
/**
 * no op
 * @param sb superblock
 * @return always 0
 */
int ramfs_sync(superblock_t* sb);
/**
 * get fs stats
 * @param sb superblock
 * @param st stats to write
 * @return success
 */
int ramfs_statfs(superblock_t* sb, statfs_t* st);
/**
 * Creates a hard link
 */
int ramfs_link(inode_t* dir, const char* name, inode_t* target);

int ramfs_init(void);

dentry_t* ramfs_get_root_dentry(void);

ssize_t ramfs_read(file_t* f, void* buf, size_t size, uint64_t offset);
ssize_t ramfs_write(file_t* f, const void* buf, size_t size, uint64_t offset);
int ramfs_readdir(file_t* f, void* dirent_out);
int ramfs_ioctl(file_t* f, unsigned long cmd, void* arg);
int ramfs_mmap(file_t* f, void* addr, size_t length, int prot, int flags);

file_t* ramfs_open_file_handle(char* path, int type);
int ramfs_set_file_mode(file_t* file, int mode);
file_t* ramfs_get_file(const char* path);
void ramfs_list_dir(inode_t* path);
void read_file(inode_t* path, size_t size);

struct ramfs_dir_entry {
	char* name;
	uint32_t mode;
	inode_t* vnode;
};

struct ramfs_dir {
	size_t count;
	size_t capacity;
	struct ramfs_dir_entry* entries;
};

struct ramfs_file {
	size_t count;
	size_t capacity;
	char* buffer;
};

struct ramfs_symlink {
	bool hard;
	char* target;
};