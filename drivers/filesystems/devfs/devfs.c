#include "filesystems/devfs.h"
#include "dynamic_mem.h"
#include "linked_list.h"
#include "mellos/block_device.h"
#include "mellos/fs.h"
#include "mellos/kernel/kernel.h"
#include "mellos/kernel/kernel_stdio.h"
#include "mellos/kernel/mount_manager.h"

bool devfs_initialized = false;
// /dev has so few files on the first layer that we cah just store them in a shitty way
// and we have dentries which will most likely hold these all the time.
linked_list_t* files;

static const fs_type_t devfs_fs_type = {.mount = &devfs_mount, .unmount = NULL, .name = "devfs"};

void init_devfs() {
	files = linked_list_create();
	vfs_mount_t* dev_mount = mount(NULL, NULL, NULL);
	if (dev_mount == NULL) {
		kfprintf(kstderr, "Failed to mount devfs!\n");
		return;
	}
	dev_mount->sb = kmalloc(sizeof(superblock_t));
	dev_mount->sb->root = kmalloc(sizeof(inode_t));
	dev_mount->sb->root->dentry->inode = dev_mount->sb->root;
	dev_mount->sb->root->dentry->name = "/dev";
	dev_mount->sb->fs = (fs_type_t*) &devfs_fs_type;
	register_fs(dev_mount);
}

void devfs_register_bdev(block_device_t* bdev) {
	
}

 
//mount_t* (*mount)(superblock_t* sb, block_device_t* dev, void* data);
// do we need this or should we just mask the filesystem in case of containers?
vfs_mount_t* devfs_mount(block_device_t* bdev, const char* mount_point, void* data) {
	vfs_mount_t* mnt = kmalloc(sizeof(vfs_mount_t));

	if (mnt == NULL) {
		kpanic_message("Unable to allocate ram for devfs.");
		return NULL;
	}

	superblock_t* sb = kmalloc(sizeof(superblock_t));

	mnt->root = sb->root;
	mnt->sb = sb;
	mnt->root->sb->bd = bdev;
	return mnt;
}


