#include "assert.h"
#include "disk.h"
#include "filesystems/fat.h"
#include "linked_list.h"
#include "mellos/block_device.h"
#include "mellos/fd.h"
#include "mellos/fs.h"
#include "mellos/kernel/mount_manager.h"
#include "ramdisk.h"

#include "kernel_stdio.h"

#include "dynamic_mem.h"

#include "errno.h"
#include "mellos/kernel/kernel.h"
#include "stddef.h"
#include "string.h"

volatile int32_t bdev_lock = 0;

linked_list_t* block_devices;

#define RAMDISK_BLOCK_SIZE 4096
#define RAMDISK_BLOCK_COUNT 2560

extern vfs_mount_t* root_mnt;

int create_partition_device(block_device_t* parent, int index, uint64_t start_lba,
                            uint64_t num_sectors, uint8_t type) {
	partition_t* p = kzalloc(sizeof(partition_t));
	p->parent = parent;
	p->start_lba = start_lba;
	p->num_sectors = num_sectors;

	block_device_t* dev = kzalloc(sizeof(block_device_t));
	// todo: dynamic strings
	dev->name = kzalloc(25);
	p->parent = dev;

	if (parent->driver_data == NULL) {
		kpanic_message("create_partition_device: no driver data");
		return -1;
	}

	if (parent->flags & BLOCK_DEVICE_FLAG_DISK_DEVICE &&
	    parent->flags & BLOCK_DEVICE_FLAG_VIRTUAL) {
		disk_device_t* disk = ((ramdisk_state*)parent->driver_data)->driver_data;
		if (parent->children > index) {
			// we have already allocted space for it
			if (linked_list_get(disk->partitions, index) == NULL) {

			} else {
				kfree(dev->name);
				kfree(dev);
				return -1;
			}
		} else {
		}
	}

	ksnprintf(dev->name, 25, "%sp%d", parent->name, index);
	dev->logical_block_size = parent->logical_block_size;
	dev->start_lba = p->start_lba;
	dev->num_blocks = num_sectors;
	dev->ops = kzalloc(sizeof(block_device_ops));
	dev->flags |= BLOCK_DEVICE_FLAG_PARTITION;
	dev->parent = parent;
	dev->ops->read_blocks = &part_read;
	dev->ops->write_blocks = &part_write;
	dev->driver_data = p;
	dev->children = 0;

	bdev_register(dev);
	return 0;
}

block_device_t* register_disk(disk_info_t* diskinfo) {
	if (diskinfo->drive == PATA_CHS) {
		kprintf("chs is unsupported\n");
		return NULL;
	}
	block_device_t* block_device = kzalloc(sizeof(block_device_t));
	disk_device_t* disk_device = kzalloc(sizeof(disk_device_t));
	lba_data_t* lbadata = (lba_data_t*)diskinfo->data;
	block_device->driver_data = disk_device;
	disk_device->disk_info = diskinfo;
	block_device->flags |= BLOCK_DEVICE_FLAG_DISK_DEVICE;
	if (diskinfo->id == DRIVE_SELECT_MASTER) {
		block_device->name = "hd0";
	} else {
		block_device->name = "hd1";
	}

	block_device->logical_block_size = diskinfo->sector_size;
	block_device->num_blocks = lbadata->sector_count;

	block_device->ops = kzalloc(sizeof(block_device_ops));
	block_device->ops->read_blocks = &atapi_read;
	block_device->ops->write_blocks = &atapi_write;
	block_device->ops->flush = &atapi_flush;
	bdev_register(block_device);

	return block_device;
}

/**
 * initializes the ramdisk, called on kernel init
 * todo: add compile options for ramdisk count
 * @return 0 for success
 */
int bdev_initialize_blockdevices() {
	if (block_devices != NULL) {
		return -1;
	}

	SpinLock(&bdev_lock);
	block_devices = linked_list_create();
	SpinUnlock(&bdev_lock);

	disk_info_t* diskinfo = kzalloc(sizeof(disk_info_t));
	read_disk_info(DRIVE_SELECT_MASTER, diskinfo);

	if (!diskinfo) {
		kpanic_message("Error getting disk info from primary drive");
	}

	mbr_t* mbr = kzalloc(sizeof(mbr_t));
	if (read_sector(diskinfo, 0, 1, (uint16_t*)mbr) != 0) {
		kpanic_message("Error reading first sector of primary drive");
	}
	//kprintf("%u",mbr->boot_code);
	block_device_t* primarybd = register_disk(diskinfo);
	for (int i = 0; i < 4; i++) {
		mbr_partition_entry_t* entry = &(mbr->entries[i]);
		if (entry->type == 0 || entry->lba_count == 0) {
			kprintf("discarding partition, it seems to be unused: \nlba_start: %u\nlba_count: %u\ntype: %u\n", entry->lba_start, entry->lba_count, entry->type);
			continue;
		}
		create_partition_device(primarybd, i, entry->lba_start, entry->lba_count, entry->type);
	}

	block_device_t* root_bd = get_block_device_by_name("hd0p1");
	superblock_t* root_sb = kzalloc(sizeof(superblock_t));

	kassert_msg(root_bd != NULL, "root_bd is NULL");
	kassert_msg(root_sb != NULL, "root_sb is NULL");

	root_sb->bd = root_bd;
	root_sb->ops = fat_get_super_ops();
	root_sb->fs = fat_get_fs_type();
	root_mnt = mount(root_sb, root_bd, "/");
	kfree(mbr);

	kassert_msg(root_mnt != NULL, "root_mnt is NULL");

	return 0;
}

/**
 * Registers a block device to the kernel
 * @param dev Block device to register
 * @return 0 for success
 */
int bdev_register(block_device_t* dev) {
	if (dev == NULL) {
		return -1;
	}

	if (dev->driver_data == NULL) {
		return -EINVAL;
	}
	if (dev->flags == 0) {
		return -EINVAL;
	}
	if (dev->name == NULL) {
		return -EINVAL;
	}
	if (dev->ops == NULL) {
        return -EINVAL;
	}
#ifdef MELLOS_DEBUG
	kprintf("Registering block device %s\n", dev->name);
	kprintf("flags: %016lu\n", dev->flags);
#endif
	SpinLock(&bdev_lock);
	linked_list_push_back(block_devices, dev);
	SpinUnlock(&bdev_lock);
	return 0;
}

bool filter_function(list_node_t* node, void* name) {
	if (block_devices == NULL) {
		return false;
	}
	if (node->data == NULL) {
		return false;
	}
	block_device_t bdev = *(block_device_t*)node->data;
	return kstrcmp(bdev.name, name) == 0;
}

block_device_t* get_block_device_by_name(char* name) {
	list_node_t* node = linked_list_get_node(block_devices, name, filter_function);
	if (!node) {
		return NULL;
	}
	return node->data;
}

// todo: unmount all related filesystems
void block_devices_destroy() {
	linked_list_destroy(block_devices);
}

void scan_mbr_partitions(block_device_t* ram0) {
	mbr_t mbr;
	if (ram0->ops->read_blocks(ram0, 0, 1, &mbr) != 0) {
		return;
	}

	if (mbr.signature != 0xAA55) {
		return; // no MBR
	}

	for (int i = 0; i < 4; i++) {
		mbr_partition_entry_t* p = &mbr.entries[i];
		if (p->type == 0 || p->lba_count == 0)
			continue; // empty

		uint64_t start = p->lba_start;
		uint64_t count = p->lba_count;

		// create /dev/ram0p(i+1) backed by (start, start+count)
		create_partition_device(ram0, i + 1, start, count, p->type);
	}
}
/**
 * get all the block devices
 * @return list of block_device_t
 */
linked_list_t* get_block_devices() {
	return block_devices;
}

ssize_t part_read(block_device_t* dev, uint64_t lba, size_t count, void* buffer) {
	const partition_t* part = dev->driver_data;
	kassert(part != NULL)
	kassert(buffer != NULL);
	kassert(count > 0);
	if (lba + count > lba + part->num_sectors) {
		return -1; // EIO
	}

	return dev->parent->ops->read_blocks(part->parent, lba, count, buffer);
}

ssize_t part_write(block_device_t* dev, uint64_t lba, size_t count, const void* buffer) {
	const partition_t* part = dev->driver_data;
	kassert(buffer != NULL);
	kassert(count > 0);
	if (lba + count > part->num_sectors)
		return -1;

	return part->parent->ops->write_blocks(part->parent, lba, count, buffer);
}
