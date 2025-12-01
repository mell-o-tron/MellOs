#pragma once
#include "mellos/fs.h"
/**
 * Since this blocks us from setting driver_data to disk_device_t we just include it here
 */
typedef struct {
  void* base;
  uint64_t num_blocks;
  uint32_t block_size;
  disk_device_t* driver_data;
} ramdisk_state;

/**
 * Creates a new ramdisk.
 *
 * @param name Name of the disk
 * @param base Start address of the disk
 * @param num_blocks How many blocks long do we want this disk to be
 * @param block_size What is the block size
 * @return The block device representing the ramdisk - NULL on failure
 */
block_device_t* ramdisk_create(const char* name, void* base, uint32_t num_blocks, uint32_t block_size);
block_device_t* get_block_device_by_name(const char* name);