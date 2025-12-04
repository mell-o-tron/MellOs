#include "ramdisk.h"
#include "dynamic_mem.h"
#include "mellos/block_device.h"
#include "mem.h"
#include "stddef.h"


ssize_t ramdisk_read(block_device_t* dev, uint64_t lba, size_t count, void* buffer) {
	const ramdisk_state* st = dev->driver_data;

	if (lba + count > st->num_blocks) {
		return -1;
	}

	const uint8_t* src = (uint8_t*)st->base + lba * st->block_size;
	const size_t bytes = count * st->block_size;

	memcpy(buffer, src, bytes);
	return (ssize_t)count;
}

ssize_t ramdisk_write(block_device_t* dev, uint64_t lba, size_t count, const void* buffer) {
	const ramdisk_state* st = dev->driver_data;

	if (lba + count > st->num_blocks) {
		return -1;
	}

	uint8_t* dst = (uint8_t*)st->base + lba * st->block_size;
	const size_t bytes = count * st->block_size;

	memcpy(dst, buffer, bytes);
	return (ssize_t)count;
}

static int ramdisk_flush(block_device_t* dev) {
	return 0; /* nothing to do */
}

static const block_device_ops ramdisk_ops = {
    .read_blocks = ramdisk_read,
    .write_blocks = ramdisk_write,
    .flush = ramdisk_flush,
};

block_device_t* ramdisk_create(const char* name, void* base, uint32_t num_blocks,
                               uint32_t block_size) {
	ramdisk_state* st = kmalloc(sizeof(ramdisk_state));
	st->base = base;
	st->num_blocks = num_blocks;
	st->block_size = block_size;

	block_device_t* dev = kmalloc(sizeof(block_device_t));
	dev->name = (char*)name;
	dev->logical_block_size = block_size;
	dev->num_blocks = num_blocks;
	dev->ops = (block_device_ops*)&ramdisk_ops;
	dev->driver_data = st;
	dev->parent = NULL;
	dev->start_lba = 0;
	dev->flags = BLOCK_DEVICE_FLAG_VIRTUAL | BLOCK_DEVICE_FLAG_DISK_DEVICE;
	dev->children = 0;


	st->driver_data = kmalloc(sizeof(disk_device_t));

	bdev_register(dev);

	return dev;
}
