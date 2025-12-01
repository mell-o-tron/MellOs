#include "linked_list.h"
#include "mellos/block_device.h"
#include "ramdisk.h"

#include "mellos/kernel/kernel_stdio.h"

#include "string.h"

#include "dynamic_mem.h"

#include "errno.h"
#include "mellos/kernel/kernel.h"

spinlock_t bdev_lock;

linked_list_t* block_devices;

#define RAMDISK_BLOCK_SIZE 4096
#define RAMDISK_BLOCK_COUNT 2560

int create_partition_device(block_device_t* parent, int index, uint64_t start_lba,
                            uint64_t num_sectors, uint8_t type) {
	partition_t* p = kmalloc(sizeof(partition_t));
	p->parent = parent;
	p->start_lba = start_lba;
	p->num_sectors = num_sectors;

	block_device_t* dev = kmalloc(sizeof(block_device_t));
	// todo: dynamic strings
	dev->name = kmalloc(25);

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
	dev->num_blocks = num_sectors;
	dev->ops = kmalloc(sizeof(block_device_ops));
	dev->flags |= BLOCK_DEVICE_FLAG_PARTITION;
	dev->parent = parent;
	dev->ops->read_blocks = &part_read;
	dev->ops->write_blocks = &part_write;
	dev->driver_data = p;
	dev->children = 0;

	bdev_register(dev);
	return 0;
}

/**
 * initializes the ramdisk, called on kernel init
 * todo: add compile options for ramdisk count
 * @return 0 for success
 */
int bdev_initialize_blockdevices() {
	spinlock_init(&bdev_lock);
	if (block_devices != NULL) {
		return -1;
	}

	spinlock_lock(&bdev_lock);
	block_devices = linked_list_create();
	spinlock_unlock(&bdev_lock);
	void* ramdisk = kmalloc(RAMDISK_BLOCK_SIZE * RAMDISK_BLOCK_COUNT);

	block_device_t* ramdisk_device =
	    ramdisk_create("ram0", ramdisk, RAMDISK_BLOCK_COUNT, RAMDISK_BLOCK_SIZE);

	create_partition_device(ramdisk_device, 0, 0, RAMDISK_BLOCK_COUNT / 2, 0);
	create_partition_device(ramdisk_device, 1, RAMDISK_BLOCK_COUNT / 2, RAMDISK_BLOCK_COUNT / 4, 0);
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
	kprintf("Registering block device %s\n", dev->name);
	kprintf("flags: %016lu\n", dev->flags);
	spinlock_lock(&bdev_lock);
	linked_list_push_back(block_devices, dev);
	spinlock_unlock(&bdev_lock);
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
	return strcmp(bdev.name, name) == 0;
}

block_device_t* get_block_device_by_name(const char* name) {
	return linked_list_get_node(block_devices, name, filter_function)->data;
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

	if (lba + count > part->num_sectors) {
		return -1; // EIO
	}

	return part->parent->ops->read_blocks(part->parent, part->start_lba + lba, count, buffer);
}

ssize_t part_write(block_device_t* dev, uint64_t lba, size_t count, const void* buffer) {
	const partition_t* part = dev->driver_data;

	if (lba + count > part->num_sectors)
		return -1;

	return part->parent->ops->write_blocks(part->parent, part->start_lba + lba, count, buffer);
}
