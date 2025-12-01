#pragma once
#include "mellos/kernel/mount_manager.h"

typedef struct {
	bool is_root;
	uint16_t first_cluster;
	size_t size;
} fat12_inode_t;

typedef struct {
	uint16_t first_block;
	uint16_t first_cluster;
	// size in bytes
	uint16_t size;
	uint16_t block_size;
} fat12_file_t;

typedef struct {
	uint8_t name[11];
	uint8_t attributes;
	uint8_t reserver_for_win_nt;
	// 100ths of a second, windows 7 stores 0-199 in this
	uint8_t creation_time_cent;
	// hours: 5 bits, minutes: 6 bits, seconds: 5 bits
	uint16_t creation_time_short;
	// year: 7 bits, month: 4 bits, day: 5 bits
	uint16_t creation_time_long;
	// same as above
	uint16_t access_date;
	// high 16 bits of the cluster number always zero for fat 12 and 16
	uint16_t high_16_cluster;
	// last modification date, same format as creation date
	uint16_t modification_time;
	uint16_t modification_date;
	// low 16 bits of the cluster number
	uint16_t low_16_cluster;
	// file size in bytes
	uint32_t file_size;
} fat12_dir_entry_t;


uint16_t fat12_get_next_cluster(superblock_t* fs, uint16_t active_cluster, uint32_t first_fat_sector, uint32_t sector_size);
int fat12_lookup(inode_t* dir, const char* name, inode_t** out);
int fat12_create(inode_t* dir, const char* name, uint32_t type, inode_t** out);
int fat12_mkdir(inode_t* dir, const char* name, uint32_t type);

ssize_t fat12_read(file_t* f, void* buf, size_t size, uint64_t offset);
ssize_t fat12_write(file_t* f, const void* buf, size_t size, uint64_t offset);
size_t fat12_readdir(file_t* f, void* dirent_out);

size_t fat12_truncate(inode_t* vn, uint64_t new_size);
size_t fat12_ioctl(file_t* f, unsigned long cmd, void* arg);

// for swap
size_t fat12_mmap(file_t* f, void* addr, size_t length, int prot, int flags);