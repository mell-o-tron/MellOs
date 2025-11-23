#include "linked_list.h"
#include "mellos/block_device.h"
#include "ramdisk.h"
#include "stdlib.h"

#include "mellos/kernel/kernel_stdio.h"

#include "mellos/fs.h"
#include "string.h"

#include <dynamic_mem.h>

linked_list_t* block_devices;

#define RAMDISK_BLOCK_SIZE 4096
#define RAMDISK_BLOCK_COUNT 2560
/**
 * initializes the ramdisk, called on kernel init
 * @return 0 for success
 */
int bdev_initialize_blockdevices() {
	if (block_devices != NULL) {
		return -1;
	}

	block_devices = linked_list_create();
	void* ramdisk = kmalloc(RAMDISK_BLOCK_SIZE * RAMDISK_BLOCK_COUNT);
	ramdisk_create("ram0", ramdisk, RAMDISK_BLOCK_COUNT, RAMDISK_BLOCK_SIZE);
	return 0;
}

/**
 * Registers a block device to the kernel
 * @param dev Block device to register
 * @return 0 for success
 */
int bdev_register(block_device* dev) {
	if (dev == NULL) {
		return -1;
	}

	kprintf("Registering block device %s\n", dev->name);
	kprintf("flags: %016lu\n", dev->flags);
	linked_list_push_back(block_devices, dev);
	return 0;
}

bool filter_function(list_node_t* node, void* name) {
	if (block_devices == NULL) {
		return false;
	}
	if (node->data == NULL) {
		return false;
	}
	block_device_t bdev = *(block_device*)node->data;
	return strcmp(bdev.name, name) == 0;
}

block_device* get_block_device_by_name(const char* name) {
	return linked_list_get_node(block_devices, name, filter_function)->data;
}

// todo: unmount all related filesystems
void block_devices_destroy() {
	linked_list_destroy(block_devices);
}

linked_list_t* get_block_devices() {
	return block_devices;
}