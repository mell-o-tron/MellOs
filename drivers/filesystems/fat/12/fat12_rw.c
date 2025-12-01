#include "fat12_rw.h"

#include "fat.h"
#include "mellos/kernel/kernel_stdio.h"

#include "errno.h"
#include "string.h"

#include "dynamic_mem.h"

#include <math.h>

uint32_t cluster_first_sector(fat_driver_data_t* fs, uint32_t cluster) {
	return fs->first_data_sector + ((cluster - 2) * fs->bfb->sectors_per_cluster);
}

uint16_t fat12_get_next_cluster(superblock_t* fs, uint16_t active_cluster,
                                uint32_t first_fat_sector, uint32_t sector_size) {
	// todo: non-2 fat sector support (atleast that's what i understood this meaning)
	uint8_t FAT_table[sector_size * 2]; // needs two in case we straddle a sector
	uint32_t fat_offset = active_cluster + (active_cluster / 2); // multiply by 1.5
	uint32_t fat_sector = first_fat_sector + (fat_offset / sector_size);
	uint32_t ent_offset = fat_offset % sector_size;

	fs->bd->ops->read_blocks(fs->bd, fat_sector, 2, FAT_table);

	uint16_t table_value = *(uint16_t*)&FAT_table[ent_offset];

	table_value = (active_cluster & 1) ? table_value >> 4 : table_value & 0xfff;

	// the variable "table_value" now has the information you need about the next cluster in the
	// chain.

	return table_value;
}

int fat12_create(inode_t* dir, const char* name, uint32_t type, inode_t** out) {
	fat12_inode_t* fin = kmalloc(sizeof(fat12_inode_t));

	inode_t* in = kmalloc(sizeof(inode_t));

	in->private = fin;

	in->ops = &fat12_inode_ops;

	in->sb = dir->sb;
	in->fops = &fat12_super_ops;
	return 0;
}

bool fat12_match_83_name(fat12_dir_entry_t* entry, char* string) {
	return memcmp(entry->name, string, 11) == 0;
}

bool is_cluster_valid(uint32_t cluster) {
	if (cluster == 0 || cluster == 0xffff || cluster == 0xfffe) {
		return false;
	}

	return true;
}

int fat12_lookup(inode_t* dir, const char* name, inode_t** out) {
	fat_driver_data_t* fs = dir->sb->private;
	uint16_t bps = fs->bfb->bytes_per_sector;
	uint16_t ents_per_sec = bps / 32;

	fat12_inode_t* fin = dir->private;
	bool is_root = fin->is_root;

	uint32_t sector;
	uint32_t max_entries;
	uint32_t entry_index = 0;
	uint8_t buf[512];

	if (is_root) {
		sector = fs->first_data_sector - fs->root_dir_sectors; // first_root_sector
		max_entries = fs->bfb->root_entry_count;
	} else {
		uint32_t cluster = fin->first_cluster;

		while (is_cluster_valid(cluster)) {
			uint32_t first_sector = cluster_first_sector(fs, cluster);

			for (uint8_t s = 0; s < fs->bfb->sectors_per_cluster; s++) {
				uint32_t sec = first_sector + s;
				dir->sb->bd->ops->read_blocks(dir->sb->bd, sec, 1, buf);

				fat12_dir_entry_t* de = (fat12_dir_entry_t*)buf;

				for (uint32_t i = 0; i < ents_per_sec; i++) {

					// same Name[0] / Attr logic as above
					// but here, 0x00 also means "end of directory", so you can
					// early-return -ENOENT
				}
			}

			cluster = fat12_get_next_cluster(dir->sb, cluster, fs->bfb->reserved_sector_count,
			                                 fs->bfb->bytes_per_sector);
		}

		return -ENOENT;
	}

	while (entry_index < max_entries) {
		uint32_t sector = sector + (entry_index / ents_per_sec);

		dir->sb->bd->ops->read_blocks(dir->sb->bd, sector, 1, buf);

		fat12_dir_entry_t* de = buf;

		for (uint32_t i = 0; i < ents_per_sec && entry_index < max_entries; i++, entry_index++) {
			fat12_dir_entry_t* e = &de[i];

			if (e->name[0] == 0x00) {
				// end of directory
				return -ENOENT;
			}
			if (e->name[0] == 0xE5) {
				// deleted, skip
				continue;
			}
			if (e->attributes == 0x0F) {
				// LFN entry, skip for now
				continue;
			}

			if (fat12_match_83_name(e, name)) {
				// found it: create new inode + fill *out

				*out = dir->sb->ops->allocate_inode(dir->sb);
				((inode_t*)out)->private = e;

				return 0;
			}
		}
	}

	return -ENOENT;
}

int fat12_mkdir(inode_t* dir, const char* name, uint32_t type) {

	return 0;
}

ssize_t fat12_read(file_t* f, void* buf, size_t size, uint64_t offset) {
	fat12_file_t* ffile = f->private;

	if (offset >= ffile->size) {
		return -ENOTDIR;
	}

	size_t to_read = umin(size, ffile->size / ffile->block_size);

	return f->inode->sb->bd->ops->read_blocks(f->inode->sb->bd, ffile->first_block,
	                                   to_read, buf);
}
ssize_t fat12_write(file_t* f, const void* buf, size_t size, uint64_t offset) {
	return -ENOSYS;
}
size_t fat12_readdir(file_t* f, void* dirent_out) {
	return -ENOSYS;
}

size_t fat12_truncate(inode_t* vn, uint64_t new_size) {
	return -ENOSYS;
}
size_t fat12_ioctl(file_t* f, unsigned long cmd, void* arg) {
	return -ENOSYS;
}

// for swap
size_t fat12_mmap(file_t* f, void* addr, size_t length, int prot, int flags) {
	return -ENOSYS;
}