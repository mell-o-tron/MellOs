#pragma once
#include "mellos/block_device.h"
#include "mellos/fs.h"

#define FAT12_NAME "fat12"
#define FAT12_IDENTIFIER 0xfa412
#define FAT16_NAME "fat16"
#define FAT16_IDENTIFIER 0xfa416
#define FAT32_NAME "fat32"
#define FAT32_IDENTIFIER 0xfa432

vfs_mount_t* fat12_mount(block_device_t* block_device, const char* mount_point, void* data);
int fat12_unmount(vfs_mount_t* mount);
int fat12_sync(superblock_t* sb);
int fat12_statfs(superblock_t* sb, statfs_t* st);
typedef struct {
	_Bool is_root;
	partition_t* partition;
	dentry_t* parent;
	superblock_t* superblock;
} fat_mount_data_t;

typedef enum { FAT_TYPE_12, FAT_TYPE_16, FAT_TYPE_32 } fat_type_t;
// https://academy.cba.mit.edu/classes/networking_communications/SD/FAT.pdf

// the next 3 structs are from osdev.org
typedef struct {
	// extended fat32 stuff
	uint32_t table_size_32;
	uint16_t extended_flags;
	uint16_t fat_version;
	uint32_t root_cluster;
	uint16_t fat_info;
	uint16_t backup_BS_sector;
	uint8_t reserved_0[12];
	uint8_t drive_number;
	uint8_t reserved_1;
	uint8_t boot_signature;
	uint32_t volume_id;
	uint8_t volume_label[11];
	uint8_t fat_type_label[8];

} __attribute__((packed)) fat_extbfb_32_t;

typedef struct {
	// extended fat12 and fat16 stuff
	uint8_t bios_drive_num;
	uint8_t reserved1;
	uint8_t boot_signature;
	uint32_t volume_id;
	uint8_t volume_label[11];
	uint8_t fat_type_label[8];

} __attribute__((packed)) fat_extbfb_16_t;

typedef struct {
	uint8_t bootjmp[3];
	// todo: set this to something like "mellos", this should specify
	//  what system formatted the volume according to the documentation
	uint8_t oem_name[8];
	// possible values: 512, 1024, 2048 or 40
	uint16_t bytes_per_sector;
	// power of 2, 1..128
	uint8_t sectors_per_cluster;
	// this is usually used to align the sectors, must be non 0
	// number of sectors reserved after the first sector,
	// also known as "first_fat_sector"
	uint16_t reserved_sector_count;
	// 2 is recommended, could be 1
	uint8_t table_count;
	// number of 32 byte directory entries in the root directory, 0 for FAT32
	// when multiplied by 32 this it should always result in an even multiple with
	// sectors_per_cluster
	uint16_t root_entry_count;
	// number of sectors for FAT 12 and 16, 0 for FAT32
	uint16_t total_sectors_16;
	// 0xF0, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
	// 0xF8 means non-removable and 0xF0 means removable
	uint8_t fat_media_type;
	// (FAT 12, 16) fat size in sectors
	// must be 0 on FAT32
	uint16_t table_size_16;
	//
	uint16_t sectors_per_track;
	uint16_t head_side_count;
	uint32_t hidden_sector_count;
	uint32_t total_sectors_32;

	// this will be cast to it's specific type once the driver actually knows what type of FAT this
	// is.
	unsigned char extended_section[54];

} __attribute__((packed)) fat_bfb_t;

/**
 * This is mostly what can't be directly got from the first block
 */
typedef struct {
	uint32_t free_sectors;
	uint32_t first_data_sector;
	uint32_t root_dir_sectors;
	uint32_t fat_size;
	uint32_t total_sectors;
	uint32_t data_sectors;
	uint32_t total_clusters;
	uint32_t free_inodes;
	uint32_t total_inodes;
	/**
	 * Sync before reading, stuff like free blocks could change without being updated
	 */
	fat_bfb_t* bfb;
} fat_driver_data_t;


fat_type_t detect_fat_type(const fat_bfb_t* bfb);
fs_type_t* fat_get_fs_type();
super_ops_t* fat_get_super_ops();
file_ops_t* fat_get_file_ops();
inode_ops_t* fat_get_inode_ops();