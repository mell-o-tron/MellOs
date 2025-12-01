#pragma once
#include "linked_list.h"
#include "mellos/block_device.h"
#include "mellos/fs.h"

/**
 * Kernel INode structure (represents a directory entry or file)
 */
typedef struct inode inode_t;
struct file;
/**
 * Kernel file structure (per open handle)
 */
typedef struct file file_t;

void init_vfs(void);
void uninitialize_mount_manager(void);
vfs_mount_t* mount(superblock_t* sb, block_device_t* bdev, const char* target);
void unmount(vfs_mount_t* fs);
bool are_mounts_initialized(void);
vfs_mount_t* get_root_mount(void);
vfs_mount_t* get_proc_mount(void);

void register_fs(vfs_mount_t* fs);
int register_filesystem(fs_type_t* fs);
void init_fs_registry(void);
void destroy_fs_registry(void);

linked_list_t* get_mounts(void);
linked_list_t* get_registered_filesystems(void);

int dentry_init(dentry_t* dentry, inode_t* inode);
int dentry_delete(dentry_t* dentry);
dentry_t* dentry_alloc(dentry_t* parent, char* name);
vfs_mount_t* get_mount_for_bd(block_device_t* bd);
vfs_mount_t* get_mount_for_sb(superblock_t* sb);