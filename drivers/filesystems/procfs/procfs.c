#include "filesystems/procfs.h"
#include "dynamic_mem.h"
#include "errno.h"
#include "mellos/fd.h"
#include "mellos/fs.h"
#include "mellos/kernel/dentry.h"
#include "mellos/kernel/mount_manager.h"
#include "kernel_stdio.h"
#include "string.h"

vfs_mount_t* procfs_mount(block_device_t* dev, const char* mount_point, void* data);
int procfs_unmount(vfs_mount_t* mount);
/**
    * Reads a directory
    */
int procfs_lookup(inode_t* dir, const char* name, inode_t** out);
/**
    * Creates a file. Use mkdir for directories.
    * This should return EINVAL if mode & S_IFDIR
    */
int procfs_create_inode(inode_t* dir, const char* name, uint32_t mode, inode_t** out);
/**
    * Creates a directory. this should return
    * EINVAL if !(mode & S_IFDIR)
    */
int procfs_mkdir(inode_t* dir, const char* name, uint32_t mode);
int procfs_unlink(inode_t* dir, const char* name);
int procfs_readlink(inode_t* vn, char* buf, size_t size);
/**
    * Creates a soft link
    */
int procfs_symlink(inode_t* dir, const char* name, const char* target);
/**
    * Creates a hard link
    */
int procfs_link(inode_t* dir, const char* name, inode_t* target);

/**
    * gets an inode relative to another inode
    */
inode_t* procfs_inode_get_relative(inode_t rel, const char* path);

/**
    * gets an inode relative to the filesystem root
    */
inode_t* procfs_inode_get_path(const char* path);

// #include "mellos/kernel/mount_manager.h"

inode_t* procfs_alloc_inode(superblock_t* sb);
int procfs_destroy_inode(inode_t* inode);
int procfs_sync(superblock_t* sb);
int procfs_statfs(superblock_t* sb, statfs_t* st);

static fs_type_t procfs_fs_type = {
    .name = "procfs", .mount = &procfs_mount, .unmount = &procfs_unmount};

fs_type_t* procfs_getfs() {
	return &procfs_fs_type;
}

super_ops_t procfs_file_ops = {
    .allocate_inode = &procfs_alloc_inode,
    .destroy_inode = &procfs_destroy_inode,
    .sync = &procfs_sync,
    .statfs = &procfs_statfs,
};

// TODO: implement all of this
inode_ops_t procfs_inode_ops = {
    //    .lookup = &procfs_lookup,
    //    .inode_get_path = &procfs_inode_get_path,
    //    .mkdir = &procfs_mkdir,

.create = &procfs_create_inode,
};

super_ops_t* procfs_get_file_ops() {
	return &procfs_file_ops;
}

vfs_mount_t* procfs_mount(block_device_t* dev, const char* mount_point, void* data) {
	superblock_t* sb = kmalloc(sizeof(superblock_t));

	const procfs_mount_data_t* mount_data = data;

	sb->bd = dev;
	sb->block_size = sb->bd->logical_block_size;

	sb->fs = &procfs_fs_type;
	sb->identifier = PROCFS_IDENTIFIER;
	sb->root = kmalloc(sizeof(inode_t));
	sb->root->sb = sb;

	vfs_mount_t* mnt = kmalloc(sizeof(vfs_mount_t));

	mnt->root = sb->root;
	mnt->sb = sb;
	sb->root->dentry = dentry_alloc(mnt->root->dentry, (char*)mount_point);
	sb->root->dentry->inode = sb->root;
	sb->root->dentry->parent = mount_data->parent_dentry;
	sb->root->ops = &procfs_inode_ops;
	sb->ops = &procfs_file_ops;
	dentry_init(mnt->root->dentry, sb->root, (char*)mount_point);
	//dentry_manager_add((char*)mount_point, sb->root->dentry);
	kprintf("procfs mounted at %s\n", mount_point);
	return mnt;
}

int procfs_unmount(vfs_mount_t* mount) {
	if (mount->root->ref_count > 0) {
		return -EBUSY;
	}
	kfree(mount->sb);
	kfree(mount->root);
	kfree(mount);
	return 0;
}

int procfs_statfs(superblock_t* sb, statfs_t* st) {
	st->f_blocks = 1;
	st->f_bfree = 1;
	st->f_ffree = 1;
	st->f_bavail = 1;
	st->f_files = 1;
	st->f_bsize = sb->block_size;
	return 0;
}

int procfs_sync(superblock_t* sb) {
	return 0;
}

int procfs_destroy_inode(inode_t* inode) {
	if (inode->ref_count > 0) {
		inode->ref_count--;
		inode->dentry->refcount--;
		return 0;
	}
	inode->ref_count--;
	inode->dentry->refcount--;
	inode->dentry->inode = NULL;
	// references to all the dentries are saved in the dcache so we can make the
	// dentry negative and delete this
	kfree(inode);
	return 0;
}

inode_t* procfs_alloc_inode(superblock_t* sb) {
	inode_t* inode = kmalloc(sizeof(inode_t));
	inode->ref_count = 1;
	// dentry gets allocated on access
	return inode;
}


int procfs_create_inode(inode_t* dir, const char* name, uint32_t mode, inode_t** out) {
	if (dir->mode & O_RDONLY) {
		return -EACCES;
	}

	if (mode & S_IFDIR) {
        return -EINVAL;
    }


	inode_t* new_inode = kzalloc(sizeof(inode_t));
	new_inode->parent = dir;
	new_inode->mode = mode;
	*out = new_inode;

    return 0;
}
