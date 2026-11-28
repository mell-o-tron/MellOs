#include "assert.h"
#include "filesystems/fat.h"

#include "dynamic_mem.h"
#include "errno.h"
#include "mellos/block_device.h"
#include "mellos/fs.h"

#include "mellos/kernel/kernel.h"
#include "mellos/kernel/dentry.h"
#include "fat12_rw.h"
#include "stddef.h"
#include "string.h"
#include "kernel_stdio.h"
#include <stdint.h>


fs_type_t fat12_fs_type = {
	.name = "fat12",
	.mount = &fat12_mount,
	.unmount = &fat12_unmount,
};

super_ops_t fat12_super_ops = {
	.allocate_inode = &fat12_allocate_inode,
	.destroy_inode = &fat12_destroy_inode,
	.sync = &fat12_sync,
	.statfs = &fat12_statfs,
};

file_ops_t fat12_file_ops = {
	.read = &fat12_read,
	.write = &fat12_write,
	.readdir = fat12_readdir,
	.truncate = &fat12_truncate,
	.ioctl = &fat12_ioctl,
	.mmap = &fat12_mmap,
};

inode_ops_t fat12_inode_ops = {
	.create = &fat12_create,
	.lookup = &fat12_lookup,
	.mkdir = &fat12_mkdir,
	.link = NULL,
	.unlink = NULL,
	.symlink = NULL,
};

inode_t* fat12_allocate_inode(superblock_t* superblock) {
	// FIXME this should actually set these
	inode_t* inode = kzalloc(sizeof(inode_t));
	inode->fops = &fat12_file_ops;
	inode->ops = &fat12_inode_ops;
	inode->sb = superblock;
	inode->private = kzalloc(sizeof(fat12_inode_t));
	return inode;
}

int fat12_destroy_inode(inode_t* inode) {
	if (inode->ref_count > 0) {
		return -1;
	}
	kfree(inode->private);
	kfree(inode);
	return 0;
}

vfs_mount_t* fat12_mount(block_device_t* block_device, const char* mount_point, void* data) {
	char* actual_mount = mount_point == NULL ? "/" : (char*)mount_point;

	fat_mount_data_t* mount_data = (fat_mount_data_t*)data;
	superblock_t* sb = mount_data->superblock == NULL ? kzalloc(sizeof(superblock_t)) : mount_data->superblock;

	sb->bd = block_device;

	sb->block_size = sb->bd->logical_block_size;

	// i guess this could be just normal kmalloc but in case read_blocks fails
	// or something weird happens we don't want junk in the buffer
	uint8_t* fat_first_sector = kzalloc(sb->block_size * sizeof(char));
	if (!fat_first_sector) {
		kpanic_message("Failed to allocate buffer for FAT12 mount");
		return NULL;
	}
	kassert(block_device->parent);
	kassert(block_device->parent->ops);
	kassert(block_device->parent->ops->read_blocks);

	ssize_t read_return = block_device->parent->ops->read_blocks(block_device->parent, 1, 1, fat_first_sector);
	kassert_msg(read_return >= 0, "reading block device failed");

	//block_device->ops->read_blocks(block_device, 0, 1, fat_first_sector);

	fat_bfb_t* bs = (fat_bfb_t*)fat_first_sector;
	kassert(bs != NULL);
#ifdef MELLOS_DEBUG
	kprintf("start_lba: %i\n", block_device->start_lba);
	kprintf("formatter program string: ");
	for (int i = 0; i < 8; i++) {
		kprintf("%c", bs->oem_name[i] == '\0' ? 'E' : bs->oem_name[i]);
	}
	kprintf("\n");
	kprintf("sb->block_size: %u\n", sb->block_size);
#endif
	/*
	for (int i = 0; i < sb->block_size; i += sizeof(char)) {
		kprintf("%x ", fat_first_sector[i]);
		if (i % 16 == 0) {
			kprintf("\n");
		}
	}
	kprintf("\n");
	*/

	sb->fs = &fat12_fs_type;
	sb->identifier = FAT12_IDENTIFIER;

	vfs_mount_t* mnt = kzalloc(sizeof(vfs_mount_t));
	kassert(mnt != NULL);

	sb->private = kmalloc(sizeof(fat_driver_data_t));
	kassert(sb->private != NULL);
	fat_driver_data_t* driver_data = sb->private;

	//kassert(bs->fat_media_type > (uint8_t)0);
	kassert(bs->sectors_per_cluster > 0);
	kassert(bs->bytes_per_sector > 0);


	driver_data->bfb = bs;
	driver_data->total_sectors =
	    bs->total_sectors_16 == 0 ? bs->total_sectors_32 : bs->total_sectors_16;
	// todo: detect fat size before this & cast the extension to the structs for fat32
	driver_data->fat_size = bs->table_size_16;
	// ^ this is in fat_driver_data_t, so we don't need to read different fields in different
	// drivers, less almost identical code
	driver_data->root_dir_sectors =
	    ((bs->root_entry_count * 32) + (bs->bytes_per_sector - 1)) / bs->bytes_per_sector;

	driver_data->first_data_sector = bs->reserved_sector_count +
	                                 (bs->table_count * driver_data->fat_size) +
	                                 driver_data->root_dir_sectors;

	driver_data->data_sectors =
	    driver_data->total_sectors -
	    (bs->reserved_sector_count + (bs->table_count * driver_data->fat_size) +
	     driver_data->root_dir_sectors);

	driver_data->total_clusters = driver_data->data_sectors / bs->sectors_per_cluster;

	if (driver_data->total_clusters > 4084) {
		// or should it be like a corrupted filesystem thing or something?
		errno = EINVAL;
		return NULL;
	}

	char* dupd_mount = kstrdup(actual_mount);
	inode_t* root_inode = kzalloc(sizeof(inode_t));
	root_inode->fops = &fat12_file_ops;
	root_inode->sb = sb;
	root_inode->ops = &fat12_inode_ops;
	root_inode->private = kzalloc(sizeof(fat12_inode_t));
	fat12_inode_t* fat12inode = (fat12_inode_t*)root_inode->private;
	root_inode->dentry = kzalloc(sizeof(dentry_t));
	mnt->root = root_inode;

	if (mount_data->is_root) {
		kassert(dentry_init(root_inode->dentry, root_inode, dupd_mount) == 0);
		root_inode->ref_count = 1;
		fat12inode->is_root = true;
	} else {
		kassert(mnt->root != NULL);
		kassert(mnt->root->dentry != NULL);
		sb->root->dentry = dentry_alloc(mount_data->is_root ? NULL : mnt->root->dentry, dupd_mount);
	}



	sb->root = root_inode;
	kassert(root_inode->dentry != NULL);
	kassert(root_inode->dentry->name != NULL);
	root_inode->dentry->inode = sb->root;
	root_inode->dentry->refcount = 1;
	root_inode->dentry->parent = mount_data->parent;
	sb->ops = &fat12_super_ops;
	//kprintf("root_inode-dentry-name: %s\n", root_inode->dentry->name);
	//kprintf("dupd_mount: %s\n", dupd_mount);
	mnt->root = root_inode;
	mnt->sb = sb;
	kprintf("fat12 mounted at: %s\n", actual_mount);
	return mnt;
}

int fat12_unmount(vfs_mount_t* mount) {
	superblock_t* sb = mount->root->sb;
	sb->ops = NULL;
	sb->identifier = 0;
	sb->block_size = 0;
	sb->fs = NULL;
	sb->bd = NULL;
	sb->root = NULL;
	kfree(sb);
	kfree(mount);

	return 0;
}

int fat12_sync(superblock_t* sb) {
	sb->bd->ops->write_blocks(sb->bd, 0, 1, ((fat_driver_data_t*)sb->private)->bfb);
	return sb->bd->ops->flush(sb->bd);
}

int fat12_statfs(superblock_t* sb, statfs_t* st) {
	st->f_type = FAT12_IDENTIFIER;
	st->f_bsize = ((fat_driver_data_t*)sb->private)->bfb->bytes_per_sector;
	st->f_blocks = ((fat_driver_data_t*)sb->private)->total_sectors;
	st->f_bavail = ((fat_driver_data_t*)sb->private)->free_sectors;
	st->f_bfree = ((fat_driver_data_t*)sb->private)->free_sectors;
	st->f_files = ((fat_driver_data_t*)sb->private)->total_inodes;
	st->f_ffree = ((fat_driver_data_t*)sb->private)->free_inodes;

	return 0;
}

fs_type_t* fat_get_fs_type() {
	return &fat12_fs_type;
}

super_ops_t* fat_get_super_ops() {
	return &fat12_super_ops;
}

inode_ops_t* fat_get_inode_ops() {
	return &fat12_inode_ops;
}

file_ops_t* fat_get_file_ops() {
	return &fat12_file_ops;
}
