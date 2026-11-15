#include "fat12_rw.h"
#include "assert.h"
#include "disk.h"
#include "dynamic_mem.h"

#include "errno.h"
#include "filesystems/fat.h"
#include "kernel_stdio.h"
#include "math.h"
#include "mellos/block_device.h"
#include "mellos/fs.h"
#include "mellos/kernel/dentry.h"
#include "mellos/kernel/kernel.h"
#include "rtc.h"
#include "string.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DRIVER_NAME "FAT"
#include "macros.h"

// https://academy.cba.mit.edu/classes/networking_communications/SD/FAT.pdf

uint32_t cluster_first_sector(fat_driver_data_t* fs, uint32_t cluster) {
	return fs->first_data_sector + ((cluster - 2) * fs->bfb->sectors_per_cluster);
}

uint16_t fat12_get_next_cluster(superblock_t* fs, uint16_t active_cluster,
                                uint32_t first_fat_sector, uint32_t sector_size) {
	// TODO: non-2 fat sector support (atleast that's what i understood this meaning)
	// it can be either 1 or 2, see the link at the top of the file
	uint8_t FAT_table[sector_size * 2]; // needs two in case we straddle a sector
	uint32_t fat_offset = active_cluster + (active_cluster / 2); // multiply by 1.5
	uint32_t fat_sector = first_fat_sector + (fat_offset / sector_size);
	uint32_t ent_offset = fat_offset % sector_size;

	// TODO: we assume the sector size is the same as the block size but this may not always be the case
	fs->bd->ops->read_blocks(fs->bd, fat_sector, 2, FAT_table);

	uint16_t table_value = *(uint16_t*)&FAT_table[ent_offset];

	table_value = (active_cluster & 1) ? table_value >> 4 : table_value & 0xfff;

	// the variable "table_value" now has the information you need about the next cluster in the
	// chain.

	return table_value;
}

uint16_t get_first_available_cluster(superblock_t* fs, uint32_t first_fat_sector, uint32_t sector_size) {

	bool cluster_empty = false;

	uint16_t cluster = 0;
	while (!cluster_empty) {
		// TODO: non-2 fat sector support, as specified in microsoft's standard there can also be only 1 fat sector
		uint16_t table_value = fat12_get_next_cluster(fs, cluster, first_fat_sector, sector_size);
		if (table_value == 0) { // free
			cluster_empty = true;
		}
		cluster++;
	}
	return cluster;
}

int fat12_create(inode_t* dir, const char* name, uint32_t mode, inode_t** out) {
	if (mode & S_IFDIR) {
		return -EINVAL;
	}
	inode_t* inode = dir->sb->ops->allocate_inode(dir->sb);
	inode->parent = dir;
	inode->mode = mode;
	*out = inode;
	return 0;
}

bool fat12_match_83_name(fat12_dir_entry_t* entry, char* string) {
	return kmemcmp(entry->name, string, 11) == 0;
}

bool is_cluster_valid(uint32_t cluster) {
	if (cluster == 0 || cluster == 0xffff || cluster == 0xfffe) {
		return false;
	}

	return true;
}

int fat12_lookup(inode_t* dir, const char* name, inode_t*** out) {
	fat_driver_data_t* fs = dir->sb->private;
	uint16_t bps = fs->bfb->bytes_per_sector;
	uint16_t ents_per_sec = bps / 32;

	fat12_inode_t* fin = dir->private;
	bool is_root = fin->is_root;

	uint32_t sector = 0;
	uint32_t max_entries;
	uint32_t entry_index = 0;
	char buf[512];
	ksnprintf(buf, sizeof(buf), "isroot = %s", (is_root ? "true" : "false"));
	//INFOF("name: %s", (char*) name);
	kmemset(buf, 0, sizeof(buf) - 1);

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

				for (uint32_t i = 0; i < ents_per_sec; i++) {

					// same Name[0] / Attr logic as above
					// but here, 0x00 also means "end of directory", so you can
					// early-return -ENOENT
				}
			}

			// TODO: is the 3rd argument really supposed to be fs->bfb->reserved_sector_count? i'm not sure but i will not think about it right now
			cluster = fat12_get_next_cluster(dir->sb, cluster, fs->bfb->reserved_sector_count,
			                                 fs->bfb->bytes_per_sector);
		}
		return -ENOENT;
	}

    size_t current_inode_buf_size = sizeof(inode_t*) * 16;
	size_t inode_buf_increment = sizeof(inode_t*) * 16;

	*out = kzalloc(current_inode_buf_size);

	uint32_t valid_inodes = 0;
	while (entry_index < max_entries) {
		INFODI("sector", sector);

		kassert(dir->sb);
		kassert(dir->sb->bd);
        kassert(dir->sb->bd->ops);
		kassert(dir->sb->bd->ops->read_blocks);
		kassert(dir->sb->bd->start_lba);
		kassert(dir->sb->bd->parent);

		/*if (!(dir->sb->flags & BLOCK_DEVICE_FLAG_PARTITION)) {
		    ERR("Superblock does not contain partition flag!");
		    asm("hlt");
            }*/
		uint64_t startlba = dir->sb->bd->start_lba;
        INFOI(startlba);
		int ret = dir->sb->bd->parent->ops->read_blocks(dir->sb->bd->parent, dir->sb->bd->start_lba + sector, 1, buf);
		if (ret < 0) {
			ERR("Unable to read blocks from root partition.");
			asm("hlt");
        }
		fat12_dir_entry_t* de = (fat12_dir_entry_t*)buf;

        uint32_t i = 0;
		for (;i < ents_per_sec && i + entry_index < max_entries; i++) {
			fat12_dir_entry_t* e = &de[i];
			kprintf("%x\n", e);

			if (e->name[0] == 0x00) {
				INFO("Returning, end of dir");
				// end of directory
				return -ENOENT;
			}
			if (e->name[0] == 0xE5) {
				INFO("Skipping, deleted");
				// deleted, skip
				continue;
			}
			if (e->attributes == 0x0F) {
				INFO("Skipping, LFN");
				// LFN entry, skip for now
				continue;
			}
			if (name == NULL) {
				if (current_inode_buf_size <= valid_inodes * sizeof(inode_t*)) {
					*out = krealloc(*out, current_inode_buf_size,
					               current_inode_buf_size + inode_buf_increment);
                    if (*out == NULL) {
                        // failed to allocate memory
                        return -errno;
                    }
                    current_inode_buf_size += inode_buf_increment;
				}
				*out[valid_inodes] = dir->sb->ops->allocate_inode(dir->sb);

				kprintf("found node: %x", (*out[valid_inodes]));
				((inode_t*)*out[valid_inodes])->private = e;
				valid_inodes++;
            } else {
                if (fat12_match_83_name(e, (char*)name)) {
                    // found it: create new inode + fill *out
                    *out[0] = dir->sb->ops->allocate_inode(dir->sb);
                    ((inode_t*)*out[0])->private = e;

                    return 0;
                }
			}
		}
		entry_index += i;

		sector = sector + (entry_index / ents_per_sec);
	}

	return -ENOENT;
}

int fat12_mkdir(inode_t* parent, const char* name, uint32_t type) {
	if (!(type & S_IFDIR)) {
		return -EINVAL;
	}
	inode_t* inode = parent->sb->ops->allocate_inode(parent->sb);
	dentry_t* de = kzalloc(sizeof(dentry_t));
	dentry_init(de, inode, name);
    de->inode = inode;
	inode->parent = parent;
	inode->mode = type;

	fat12_dir_entry_t* direntry = kzalloc(sizeof(fat12_dir_entry_t));
	char* strc = kstrdup(name);
	drop_after_last('/', strc, false);
	// we could theoretically have a thread conflict here and leave a dangling inode/dentry
	if (kstrlen(strc) > 10) {
		ERR("Unable to create file, file name too long.");
		free_dentry(de);
		parent->sb->ops->destroy_inode(inode);
		return -1;
    }
	kstrcpy(direntry->name, strc);
    kfree(strc);

	int blocks_written = parent->sb->bd->ops->write_blocks(parent->sb->bd,get_first_available_cluster(parent->sb, 3, parent->sb->block_size), sizeof(fat12_dir_entry_t), direntry);

    return blocks_written;
}

ssize_t fat12_read(file_t* f, void* buf, size_t size, uint64_t offset) {
	fat12_file_t* ffile = f->private;

	kassert(ffile);
	if (offset >= ffile->size) {
		return -ENOTDIR;
	}

	size_t to_read = umin(size, ffile->size / ffile->block_size);


	return f->inode->sb->bd->ops->read_blocks(f->inode->sb->bd, ffile->first_block, to_read, buf);
}
ssize_t fat12_write(file_t* f, const void* buf, size_t size, uint64_t offset) {
	fat12_file_t* ffile = f->private;

    kassert(ffile);
	if (f->inode->sb->bd->flags & BDEV_FLAG_READONLY) {
		return -EROFS;
	}

	if (f->inode->mode & S_IFDIR) {
		return -EISDIR;
	}

	if (offset >= ffile->size) {
		return -E2BIG;
	}

	return f->inode->sb->bd->ops->write_blocks(f->inode->sb->bd, ffile->first_block, ffile->size, buf);
}
size_t fat12_readdir(file_t* f, void* dirent_out) {
	// TODO: directory listing
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
