#pragma once
#include "stddef.h"
#include "stdint.h"

#include "linked_list.h"

typedef struct block_device block_device_t;

ssize_t part_read(block_device_t* dev, uint64_t lba, size_t count, void* buffer);

ssize_t part_write(block_device_t* dev, uint64_t lba, size_t count, const void* buffer);

#define BLOCK_DEVICE_FLAG_READ_ONLY 0x01
#define BLOCK_DEVICE_FLAG_UNINITIALIZED 0x08
#define BLOCK_DEVICE_FLAG_VIRTUAL 0x10
#define BLOCK_DEVICE_FLAG_PARTITION 0x20
#define BLOCK_DEVICE_FLAG_DISK_DEVICE 0x40

#pragma pack(push, 1)
typedef struct {
	uint8_t status;       // 0x80 = bootable, 0x00 = not
	uint8_t chs_first[3]; // chs is ancient
	uint8_t type;         // partition type (0x83, 0x07, etc.)
	uint8_t chs_last[3];  // ignore
	uint32_t lba_start;   // starting LBA
	uint32_t lba_count;   // number of sectors
} mbr_partition_entry_t;

typedef struct {
	uint8_t boot_code[446];
	mbr_partition_entry_t entries[4];
	uint16_t signature; // 0xAA55 (little-endian)
} mbr_t;
#pragma pack(pop)

/**
 * Block device operations.
 * All I/O is in units of logical blocks (sectors).
 */
typedef struct block_device_ops {
	/**
	 * Read "count" blocks starting at logical block 'lba'
	 * into "buffer". Returns number of blocks read, or <0 on error.
	 */
	ssize_t (*read_blocks)(block_device_t* dev, uint64_t lba, size_t count, void* buffer);

	/**
	 * Write "count" blocks starting at logical block 'lba'
	 * from "buffer". Returns number of blocks written, or <0 on error.
	 */
	ssize_t (*write_blocks)(block_device_t* dev, uint64_t lba, size_t count, const void* buffer);

	/**
	 * Flush any cached writes to stable storage.
	 *
	 * May be a no-op for some devices like the ramdisk. Returns 0 on success, <0 on error.
	 */
	int (*flush)(block_device_t* dev);
} block_device_ops;

/**
 * Core block device descriptor.
 * This is what filesystems and the VFS see.
 */
struct block_device {
	/* "sda", "vda", "ram0", "loop0p1", ... */
	char* name;
	/* usually how many partitions this has */
	uint32_t children;
	/* in bytes, e.g., 512 or 4096 */
	uint32_t logical_block_size;
	/* total logical blocks */
	uint64_t num_blocks;
	/* driver-provided ops */
	block_device_ops* ops;
	/**
	 * Controller data, for example partition_t for partitions and
	 * disk_device_t for disk devices.
	 */
	void* driver_data;

	/* Optional layering: partitions or virtual devices */
	block_device_t* parent; /* NULL for real device, non-NULL for partitions */
	uint64_t start_lba;     /* offset in parent, used for partitions */

	/* todo: refcount, flags, stuff like that when needed */
	uint32_t flags;
};

/**
 * Represents a partitionable device
 */
typedef struct disk_device {
	/**
	 * Array of pointers to devices with BLOCK_DEVICE_FLAG_PARTITION,
	 * check the size of the partitionsarray from the `children`
	 * member of the block_device struct.
	 *
	 */
	linked_list_t* partitions;
} disk_device_t;

typedef struct {
	block_device_t* parent;
	uint64_t start_lba;
	uint64_t num_sectors;
} partition_t;

#define BDEV_FLAG_READONLY (1u << 0)
#define BDEV_FLAG_REMOVABLE (1u << 1)
#define BDEV_FLAG_RAMBD (1u << 2)

/**
 * Initializes the block device list and adds a ram block device.
 */
int bdev_initialize_blockdevices();

/**
 * Helper inline for partition-style devices:
 * convert this device's LBA to parent device LBA.
 */
static inline uint64_t bdev_to_parent_lba(block_device_t* dev, uint64_t lba) {
	return dev->start_lba + lba;
}

/**
 * Register a new block device with the kernel. Returns 0 on success.
 */
int bdev_register(block_device_t* dev);

/**
 * Look up a block device by name, e.g., "sda", "sda1".
 */
block_device_t* bdev_lookup(const char* name);

/**
 * Get all of the detected block devices
 * @return The list of block devices
 */
linked_list_t* get_block_devices();