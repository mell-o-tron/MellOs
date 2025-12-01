#pragma once
#include "mellos/block_device.h"
#include "mellos/fs.h"

/**
 * Initializes the /dev filesystem
 */
void init_devfs(void);
vfs_mount_t* devfs_mount(block_device_t* bdev, const char* mount_point, void* data);
void devfs_register_bdev(block_device_t* bdev);
