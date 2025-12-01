#include "fat.h"

inline uint32_t bs_total_sectors(const fat_bfb_t* bs) {
	if (bs->total_sectors_16 != 0) {
		return bs->total_sectors_16;
	}
	return bs->total_sectors_32;
}

fat_type_t detect_fat_type(const fat_bfb_t* bfb) {
	uint32_t total_sectors = bs_total_sectors(bfb);
	uint32_t root_dir_sectors =
		((bfb->root_entry_count * 32) + (bfb->bytes_per_sector - 1)) / bfb->bytes_per_sector;
	uint32_t fat_size_sectors = bfb->table_size_16; // for FAT12/16
	uint32_t data_sectors =
		total_sectors -
		(bfb->reserved_sector_count + (bfb->table_count * fat_size_sectors) + root_dir_sectors);
	uint32_t total_clusters = data_sectors / bfb->sectors_per_cluster;

	if (total_clusters < 4085) {
		return FAT_TYPE_12;
	}
	if (total_clusters < 65525) {
		return FAT_TYPE_16;
	}
	return FAT_TYPE_32;
}