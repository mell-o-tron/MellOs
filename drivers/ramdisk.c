#include "dynamic_mem.h"
#include "mellos/block_device.h"
#include "mem.h"
#include "stddef.h"
#include "stdint.h"
#include "ramdisk.h"

typedef struct {
	void* base;
	uint64_t num_blocks;
	uint32_t block_size;
} ramdisk_state;

static ssize_t ramdisk_read(block_device* dev, uint64_t lba, size_t count, void* buffer) {
	ramdisk_state* st = dev->driver_data;

	if (lba + count > st->num_blocks) {
		return -1;
	}

	uint8_t* src = (uint8_t*)st->base + lba * st->block_size;
	size_t bytes = count * st->block_size;

	memcpy(buffer, src, bytes);
	return (ssize_t)count;
}

static ssize_t ramdisk_write(block_device* dev, uint64_t lba, size_t count, const void* buffer) {
	ramdisk_state* st = dev->driver_data;

	if (lba + count > st->num_blocks) {
		return -1;
	}

	uint8_t* dst = (uint8_t*)st->base + lba * st->block_size;
	size_t bytes = count * st->block_size;

	memcpy(dst, buffer, bytes);
	return (ssize_t)count;
}

static int ramdisk_flush(block_device* dev) {
	return 0; /* nothing to do */
}

static const block_device_ops ramdisk_ops = {
    .read_blocks = ramdisk_read,
    .write_blocks = ramdisk_write,
    .flush = ramdisk_flush,
};

block_device* ramdisk_create(const char* name, void* base, uint64_t num_blocks, uint32_t block_size) {
	ramdisk_state* st = kmalloc(sizeof(*st));
	st->base = base;
	st->num_blocks = num_blocks;
	st->block_size = block_size;

	block_device* dev = kmalloc(sizeof(*dev));
	dev->driver_data = kmalloc(num_blocks * block_size);
	dev->name = name;
	dev->logical_block_size = block_size;
	dev->num_blocks = num_blocks;
	dev->ops = &ramdisk_ops;
	dev->driver_data = st;
	dev->parent = NULL;
	dev->start_lba = 0;
	dev->flags = 0;

	bdev_register(dev);

	return dev;
}
