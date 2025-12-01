#include "fat.h"

#include "dynamic_mem.h"
#include "errno.h"
#include "mellos/fs.h"
#include "mellos/kernel/mount_manager.h"

#include "mellos/kernel/kernel.h"

vfs_mount_t* fat12_mount(block_device_t* block_device, const char* mount_point, void* data) {

	superblock_t* sb = kzalloc(sizeof(superblock_t));
	const fat_mount_data_t* mount_data = data;
	sb->bd = block_device;
	sb->block_size = sb->bd->logical_block_size;

	// i guess this could be just normal kmalloc but in case read_blocks fails
	// or something weird happens we don't want junk in the buffer
	uint8_t* bd_buf = kzalloc(sb->block_size);
	if (!bd_buf) {
		kpanic_message("Failed to allocate buffer for FAT12 mount");
		return NULL;
	}
	block_device->ops->read_blocks(block_device, 0, 1, bd_buf);

	fat_bfb_t* bs = (fat_bfb_t*)bd_buf;

	sb->fs = &fat12_fs_type;
	sb->identifier = FAT12_IDENTIFIER;

	vfs_mount_t* mnt = kzalloc(sizeof(vfs_mount_t));

	sb->private = kmalloc(sizeof(fat_driver_data_t));
	fat_driver_data_t* driver_data = sb->private;
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

	sb->root = kmalloc(sizeof(inode_t));
	sb->root->sb = sb;

	mnt->sb = sb;
	sb->root->dentry = dentry_alloc(mnt->root->dentry, (char*)mount_point);
	sb->root->dentry->inode = sb->root;
	sb->root->dentry->refcount = 1;
	sb->root->dentry->parent = mount_data->parent;
	sb->ops = &fat12_super_ops;


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
