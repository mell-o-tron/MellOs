#include "ext2.h"
#include "../../data_structures/bitmap.h"
#include "../../drivers/disk.h"
#include "../../utils/math.h"
#include "../../utils/bit_manip.h"
#include "../../utils/format.h"
#include "../../utils/string.h"
#include "../../memory/dynamic_mem.h"
#include "../../memory/mem.h"

#define INODE_SIZE 128
#define BLOCK_SIZE 1024
#define SECTOR_SIZE 512
#define SUPERBLOCK_SIZE 1024
#define INDIRECT_CAPACITY (BLOCK_SIZE / 4)
#define DOUBLY_INDIRECT_MAX_BLOCKS (INDIRECT_CAPACITY * INDIRECT_CAPACITY)

static struct ext2_superblock* superblock;
static struct ext2_group_desc* gdt;

void ext2_init () {
	ext2_read_superblock(0xA0);
	ext2_get_block_group_descriptor_table(0xA0);
}

struct ext2_superblock* ext2_read_superblock (uint8_t drive){
    uint16_t* res = kmalloc (SUPERBLOCK_SIZE);
	printf("reading supablock\n");
    LBA28_read_sector(drive, 2, 2, res);
	printf("read supablock\n");
	superblock = (struct ext2_superblock*)res;
	printf("inode count: %d\n", superblock -> s_inodes_count);
	printf("log2 block size - 10: %d\n", superblock -> s_log_block_size);
    return (struct ext2_superblock*)res;
}

struct ext2_group_desc* ext2_get_block_group_descriptor_table (uint8_t drive) {
	uint16_t* res = kmalloc (BLOCK_SIZE);
	LBA28_read_sector(drive, 4, BLOCK_SIZE / SECTOR_SIZE, res);
	gdt = (struct ext2_group_desc*)res;
	printf("number of directories detected: %d\n", gdt -> bg_used_dirs_count);
	return (struct ext2_group_desc*)res;
}

struct ext2_group_desc* ext2_get_block_group_descriptor (uint8_t drive, uint32_t block) {
	if (superblock->s_blocks_per_group == 0) {
		printf("blocks per group is zero: something is wrong.\n");
		while (1) {;}
	}
	uint32_t group_index = block / superblock->s_blocks_per_group;
	return &gdt[group_index];
}

struct ext2_group_desc* ext2_get_block_group_descriptor_for_inode (uint8_t drive, uint32_t inode) {
	if (superblock->s_inodes_per_group == 0) {
		printf("inodes per group is zero: something is wrong.\n");
		while (1) {;}
	}
	uint32_t group_index = (inode - 1) / superblock->s_inodes_per_group;
	return &gdt[group_index];
}

void ext2_write_superblock (uint8_t drive) {
	uint16_t* sb_data = (uint16_t*)superblock;
	LBA28_write_sector(drive, 2, 2, sb_data);
}

void ext2_write_block_group_descriptor_table (uint8_t drive) {
	uint16_t* gdt_data = (uint16_t*)gdt;
	LBA28_write_sector(drive, 4, BLOCK_SIZE / SECTOR_SIZE, gdt_data);
}

// Get the byte offset of an inode in the disk
static inline uint32_t get_inode_offset (struct ext2_group_desc* gdt, uint32_t inode) {
	return (gdt->bg_inode_table * BLOCK_SIZE) + (inode - 1) * INODE_SIZE;
}

// Read an inode from the disk
struct ext2_inode_indexed ext2_read_inode (uint8_t drive, uint32_t index) {
	struct ext2_group_desc* gdt = ext2_get_block_group_descriptor_for_inode(drive, index);

	uint32_t inode_offset = get_inode_offset(gdt, index);
	uint32_t sector = inode_offset / SECTOR_SIZE;

	// If the inode crosses a sector boundary, read two sectors
	uint8_t sectors_to_read;
	uint16_t remainder = inode_offset % SECTOR_SIZE;
	if (remainder + sizeof(struct ext2_inode) > SECTOR_SIZE) {
		sectors_to_read = 2;
	} else {
		sectors_to_read = 1;
	}

	uint16_t* res = kmalloc (SECTOR_SIZE * sectors_to_read);
	LBA28_read_sector(drive, sector, sectors_to_read, res);
	
	struct ext2_inode* inode_ptr = kmalloc(sizeof(struct ext2_inode));
	memcpy(inode_ptr, ((uint8_t*)res) + (remainder), sizeof(struct ext2_inode));
	kfree(res);

	return (struct ext2_inode_indexed){ .inode = inode_ptr, .index = index };
}

// Write back an inode to the disk
static void ext2_commit_changes_to_inode (uint8_t drive, struct ext2_inode_indexed inode) {
	struct ext2_group_desc* gdt = ext2_get_block_group_descriptor_for_inode(drive, inode.index);
	uint32_t inode_offset = get_inode_offset(gdt, inode.index);
	uint32_t sector = inode_offset / SECTOR_SIZE;

	// If the inode crosses a sector boundary, read two sectors
	uint8_t sectors_to_write;
	uint16_t remainder = inode_offset % SECTOR_SIZE;
	if (remainder + sizeof(struct ext2_inode) > SECTOR_SIZE) {
		sectors_to_write = 2;
	} else {
		sectors_to_write = 1;
	}

	// Read the sectors containing the inode
	uint16_t* res = kmalloc (SECTOR_SIZE * sectors_to_write);
	LBA28_read_sector(drive, sector, sectors_to_write, res);
	
	// Update the inode in the read sectors
	memcpy(((uint8_t*)res) + (remainder), inode.inode, sizeof(struct ext2_inode));
	
	// Write back the updated sectors
	LBA28_write_sector(drive, sector, sectors_to_write, res);
	kfree(res);
}

void* ext2_read_block (uint8_t drive, uint32_t block) {
	// printf("Reading block %d\n", block);
	uint16_t* res = kmalloc (BLOCK_SIZE);
	LBA28_read_sector(drive, block * (BLOCK_SIZE / SECTOR_SIZE), BLOCK_SIZE / SECTOR_SIZE, res);
	return (void*)res;
}

bool ext2_is_directory (struct ext2_inode* inode) {
	return (inode->i_mode & 0x4000) != 0;
}

/**
 * @brief Reads from an array of contiguous block indices.
 *
 * @param drive The drive number.
 * @param blocks The array of contiguous block indices.
 * @param offset Read offset from start of file, in bytes
 * @param size Number of bytes to be read from offset, in bytes
 * @param buffer Pointer to result buffer; has to be allocated externally
 * @return number of bytes read
 */
static uint32_t ext2_read_blocks_from_blocklist (uint8_t drive, uint32_t blocks[], uint8_t recursion_level, uint32_t offset, uint32_t size, uint8_t* buffer) {
	uint16_t max_blocks = INDIRECT_CAPACITY;
	switch (recursion_level) {
		case 0:
			max_blocks = 12;
		case 1: {
			// base case: read a maximum of INDIRECT_CAPACITY blocks from a list of blocks.
			/* start reading with an offset of offset (bytes)
			 * - we must start reading from block offset / BLOCK_SIZE
			 * - if the offset is not block-aligned, we also obtain a remainder: offset % BLOCK_SIZE
			 * - we must stop reading at block (offset + size) / BLOCK_SIZE - 1
			 * - in the final block we must only read (offset + size) % BLOCK_SIZE bytes
			 */

			uint32_t start_block		 = offset / BLOCK_SIZE;
			uint32_t first_block_offset	 = offset % BLOCK_SIZE;
			uint32_t end_block 			 = (offset + size) / BLOCK_SIZE - ( ((offset + size) % BLOCK_SIZE) == 0 ? 1 : 0 );
			uint32_t last_block_boundary = (offset + size - 1) % BLOCK_SIZE + 1;
			
			uint16_t seen_blocks = 0;
			uint32_t bytes_read = 0;
			for (uint32_t i = start_block; i <= end_block && i < max_blocks; i++) {
				uint32_t block_num = blocks[i];
				void* block_data   = ext2_read_block(drive, block_num);
				
				uint32_t copy_start = (i == start_block) ? first_block_offset 	: 0;
				uint32_t copy_end 	= (i == end_block)	 ? last_block_boundary 	: BLOCK_SIZE;
				uint32_t buffer_offset = (i - start_block) * BLOCK_SIZE - (i == start_block ? 0 : first_block_offset);

				// we copy (end - start) bytes from (data + start) to (buffer + (corrected block idx) * BLOCK_SIZE + start)
				memcpy(buffer + buffer_offset, (uint8_t*)block_data + copy_start, copy_end - copy_start);

				kfree(block_data);
				seen_blocks++;
				bytes_read += (copy_end - copy_start);
			}
			// printf("Read %d bytes\n", bytes_read);
			
			return bytes_read;
		}
		case 2: {
			// recursive case: 
			/* - Now we read a bunch of blocks, each containing INDIRECT_CAPACITY block indices
			 * - We first read the blocks as before
			 * - Instead of copying the result to the buffer, we first call the function recursively (with rec lev 1) on it
			 * - Then we concatenate all the reads into the result buffer.
			 * 
			 * Each block is a list of length INDIRECT_CAPACITY. 
			 * The maximum number of bytes we can read through one indirect block is (BLOCK_SIZE * INDIRECT_CAPACITY)
			 */

			uint32_t start_block		 = offset / (BLOCK_SIZE * INDIRECT_CAPACITY);
			uint32_t first_block_offset	 = offset % (BLOCK_SIZE * INDIRECT_CAPACITY);
			uint32_t end_block 			 = (offset + size) / (BLOCK_SIZE * INDIRECT_CAPACITY) - ( ((offset + size) % ((BLOCK_SIZE * INDIRECT_CAPACITY))) == 0 ? 1 : 0 );
			uint32_t last_block_boundary = (offset + size - 1) % ((BLOCK_SIZE * INDIRECT_CAPACITY)) + 1;
			
			// printf("last block boundary: %d\n", last_block_boundary);

			uint16_t seen_blocks = 0;
			uint32_t total_bytes_read = 0;
			for (uint32_t i = start_block; i <= end_block && i < max_blocks; i++) {
				uint32_t block_num = blocks[i];
				void* block_data   = ext2_read_block(drive, block_num);

				uint32_t meta_offset = (i == start_block) ? first_block_offset 		: 0;
				uint32_t meta_size 	 = ((i == end_block)   ? last_block_boundary 	: INDIRECT_CAPACITY * BLOCK_SIZE) - meta_offset;

				// printf("metaoffset: %d, metasize: %d\n", meta_offset, meta_size);

				void* meta_buffer = kmalloc(meta_size);

				uint32_t buffer_offset = total_bytes_read;
				total_bytes_read += ext2_read_blocks_from_blocklist(drive, block_data, 1, meta_offset, meta_size, meta_buffer);

				// we now need to copy this buffer with an offset of offset and a size of size.

				// uint32_t copy_start = (i == start_block) ? offset % (BLOCK_SIZE * INDIRECT_CAPACITY) : 0;
				// uint32_t copy_end = (i == end_block) ?  (offset + size) % ( BLOCK_SIZE * INDIRECT_CAPACITY) : BLOCK_SIZE * INDIRECT_CAPACITY;
				uint32_t copy_start = 0;
				uint32_t copy_end = meta_size;

				// printf("copy start: %d, copy end: %d, block offset: %d, buffer offset: %d\n", copy_start, copy_end, (i - start_block), buffer_offset);

				// printf("reading block %d:\tbuffer size: %u, (e-s): %u\n", i, meta_size, copy_end - copy_start);

				memcpy(buffer + buffer_offset, (uint8_t*)meta_buffer + copy_start, copy_end - copy_start);
				
				kfree(meta_buffer);
				kfree(block_data);

				seen_blocks++;
			}
			return total_bytes_read;
		}
		case 3: {

			break;
		}
		default: {
			//TODO: SCASS TUTT COS; SHE'S TURNED THE WEANS AGAINST US
			printf("error: Tried to read n-ly indirect block with n > 3\n");
		}
	}
	return 0;
}

/**
 * @brief Reads from an inode.
 *
 * @param drive The drive number.
 * @param inode The inode to be read.
 * @param offset Read offset from start of file, in bytes
 * @param size Number of bytes to be read from offset, in bytes
 * @return A pointer to an allocated buffer containing the contents of the file
 */
uint8_t* ext2_read_from_inode (uint8_t drive, struct ext2_inode* inode, uint32_t offset, uint32_t size) {
	if (offset + size > inode->i_size) {
		size = inode->i_size - offset; // adjust size to not exceed file size
	}

	// printf("Reading %d bytes from inode at offset %d (file size: %d)\n", size, offset, inode->i_size);

	// size to which the buffer will be eventually resized.
	uint32_t final_size = size;
	uint32_t bytes_read = 0;

	// buffer size is rounded up to nearest multiple of BLOCK_SIZE
	uint8_t* buffer = kmalloc(ROUND_UP(size, BLOCK_SIZE));
	size_t buffer_old_size = ROUND_UP(size, BLOCK_SIZE);

	uint8_t* offsetted_buffer = buffer;
	uint32_t start_block = offset / BLOCK_SIZE;
	uint32_t end_block = (offset + size) / BLOCK_SIZE - ( ((offset + size) % BLOCK_SIZE) == 0 ? 1 : 0 );
	uint32_t block_offset = offset % BLOCK_SIZE;

	// printf("start block: %d, end block: %d, block offset: %d\n", start_block, end_block, block_offset);

	if (start_block < 12) { // Read the part that fits in the direct blocks
		// number of direct blocks to be read
		uint32_t direct_blocks_to_read = (end_block < 12) ? (end_block - start_block + 1) : (12 - start_block);

		bytes_read += ext2_read_blocks_from_blocklist(drive, inode->i_blocks, 0, (start_block * BLOCK_SIZE) + block_offset, direct_blocks_to_read * BLOCK_SIZE - block_offset, offsetted_buffer);
		if (end_block < 12) {
			return krealloc(buffer, buffer_old_size, final_size); // all done
		}

		// case end_block >= 12
		
		// Update offset and size to reflect what has been read
		offset = 0;
		size -= bytes_read;
		offsetted_buffer += bytes_read;
		bytes_read = 0;
	} else {
		offset -= 12 * BLOCK_SIZE;
	}

	if (start_block < INDIRECT_CAPACITY + 12) {	// case: start reading before end of singly indirect
		// Read the part that fits in the singly indirect blocks
		uint8_t* singlynd = ext2_read_block(drive, inode->i_singly_ind);

		uint32_t bytes_to_read = (offset + size) < BLOCK_SIZE * INDIRECT_CAPACITY ?
			size :
			(BLOCK_SIZE * (INDIRECT_CAPACITY) - offset - bytes_read); 

		bytes_read += ext2_read_blocks_from_blocklist(drive, (uint32_t*)singlynd, 1, offset, bytes_to_read, offsetted_buffer);

		if(offset + size < (BLOCK_SIZE * (INDIRECT_CAPACITY))) { // Case: all fits within direct and singly indirect
			// printf("all fits within singly\n");
			return krealloc(buffer, buffer_old_size, final_size);
		}

		// printf("still a bunch to be read\n");
		// printf("read %d bytes from singly indirect, at offset %d\n", bytes_read, offset + (12 * BLOCK_SIZE));
		size -= bytes_read;
		offsetted_buffer += bytes_read;
		offset = 0;
		bytes_read = 0;
	} else {
		offset -= INDIRECT_CAPACITY * BLOCK_SIZE;
	}

	if (start_block < (INDIRECT_CAPACITY + 12) + (DOUBLY_INDIRECT_MAX_BLOCKS)) { // case: start reading before end of doubly indirect
		// printf("reading from doubly indirect at offset %d size %d\n", offset + ((12 + INDIRECT_CAPACITY) * BLOCK_SIZE), size);
		// printf("start block: %d\n", start_block);
		uint8_t* doublynd = ext2_read_block(drive, inode->i_doubly_ind);

		uint32_t max = (DOUBLY_INDIRECT_MAX_BLOCKS);
		uint32_t bytes_to_read = offset + size + bytes_read < BLOCK_SIZE * max ?
			size :
			(BLOCK_SIZE * max - offset - bytes_read);

		ext2_read_blocks_from_blocklist(drive, (uint32_t*)doublynd, 2, offset, bytes_to_read, offsetted_buffer);

		return krealloc(buffer, buffer_old_size, final_size);
	}

	// triply indirect not yet implemented.
	return krealloc(buffer, buffer_old_size, final_size);
}

void ext2_list_files_in_dir (struct ext2_inode* dir) {

	if(!dir) {printf("directory is null\n"); return;}

	printf("type and permissions: %d\n", dir -> i_mode);
	if(!ext2_is_directory(dir)) {
		printf("Tried to list files in a non-directory.\n");
		return;
	}

	void* block0 = ext2_read_block(0xA0, dir->i_blocks[0]);
	uint8_t inode_num;
	struct ext2_directory_entry* entry = (struct ext2_directory_entry*)block0;
	//print_hex(block0, 64);	
	while ((uint8_t*)entry < (uint8_t*)block0 + BLOCK_SIZE) {
		printf("Entry inode: %d\n", entry->inode);
		if (entry == 0) { // TODO: why does this work???
			break;
		}
		char name[256];

		printf("length: %d\n", entry -> name_length);
		memcpy(name, &(entry->name), entry->name_length);
		name[entry->name_length] = 0;
        printf("Inode: %d, Rec Len: %d, Name Len: %d, Name: %s, Type: %s, Mode: %o\n",
			entry->inode,
			entry->size_of_entry,
			entry->name_length,
			name,
			ext2_is_directory(ext2_read_inode(0xA0, entry->inode).inode) ? "Directory" : "File",
			ext2_read_inode(0xA0, entry->inode).inode->i_mode
		);
        entry = (struct ext2_directory_entry*)((uint8_t*)entry + entry->size_of_entry);
		
		// to please valgrind (avoid invalid read)
		if ((uint8_t*)entry < (uint8_t*)block0 + BLOCK_SIZE) {
        	inode_num = entry->inode;
		}
    }
}

struct ext2_inode_indexed find_file_by_name (struct ext2_inode* dir, const char* target_name) {
	if(!ext2_is_directory(dir)) {
		printf("Tried to find file in a non-directory.\n");
		return (struct ext2_inode_indexed){.inode = NULL, .index = 0};
	}

	void* block0 = ext2_read_block(0xA0, dir->i_blocks[0]);
	uint8_t inode_num;
	struct ext2_directory_entry* entry = (struct ext2_directory_entry*)block0;
	//print_hex(block0, 64);	
	while ((uint8_t*)entry < (uint8_t*)block0 + BLOCK_SIZE) {
		char name[256];
		memcpy(name, &(entry->name), entry->name_length);
		name[entry->name_length] = 0;

		if(strcmp(name, target_name) == 0) {
			struct ext2_inode_indexed res = ext2_read_inode(0xA0, entry->inode);
			kfree(block0);
			return res;
		}

		entry = (struct ext2_directory_entry*)((uint8_t*)entry + entry->size_of_entry);

		// to please valgrind (avoid invalid read)
		if ((uint8_t*)entry < (uint8_t*)block0 + BLOCK_SIZE) {
        	inode_num = entry->inode;
		}
    }

	printf("%s: No such file or directory.\n", target_name);
	kfree(block0);
	return (struct ext2_inode_indexed){.inode = NULL, .index = 0};
}

static void trim_newline(char *s) {
    size_t n = strlen(s);
    if (n > 0 && s[n-1] == '\n') s[n-1] = '\0';
}

uint32_t ext2_write_block (uint8_t drive, uint32_t block, uint8_t* data) {
	// printf("Writing block %d\n", block);
	LBA28_write_sector(drive, block * (BLOCK_SIZE / SECTOR_SIZE), BLOCK_SIZE / SECTOR_SIZE, (uint16_t*)data);
	return BLOCK_SIZE;
}

static uint32_t ext2_write_blocks_from_blocklist (uint8_t drive, uint32_t blocks[], uint8_t recursion_level, uint32_t offset, uint32_t size, uint8_t* buffer) {
	uint16_t max_blocks = INDIRECT_CAPACITY;
	switch (recursion_level) {
		case 0:
			max_blocks = 12;
		case 1: {
			// base case: write a maximum of INDIRECT_CAPACITY blocks from a list of blocks.
			/* start writing with an offset of offset (bytes)
			 * - we must start writing from block offset / BLOCK_SIZE
			 * - if the offset is not block-aligned, we also obtain a remainder: offset % BLOCK_SIZE
			 * - we must stop writing at block (offset + size) / BLOCK_SIZE - 1
			 * - in the final block we must only write (offset + size) % BLOCK_SIZE bytes
			 */

			uint32_t start_block		 = offset / BLOCK_SIZE;
			uint32_t first_block_offset	 = offset % BLOCK_SIZE;
			uint32_t end_block 			 = (offset + size) / BLOCK_SIZE - ( ((offset + size) % BLOCK_SIZE) == 0 ? 1 : 0 );
			uint32_t last_block_boundary = (offset + size - 1) % BLOCK_SIZE + 1;
			
			uint16_t seen_blocks = 0;
			uint32_t bytes_writ = 0;
			for (uint32_t i = start_block; i <= end_block && i < max_blocks; i++) {
				uint32_t block_num = blocks[i];
				bool partial_write = ((i == start_block) || (i == end_block));
				if (partial_write) {
					void* block_data = ext2_read_block(drive, block_num);
					uint32_t copy_start = (i == start_block) ? first_block_offset 	: 0;
					uint32_t copy_end 	= (i == end_block)	 ? last_block_boundary 	: BLOCK_SIZE;
					memcpy((uint8_t*)block_data + copy_start, buffer, copy_end - copy_start);
					ext2_write_block(drive, block_num, block_data);
					kfree(block_data);
					bytes_writ += (copy_end - copy_start);
					seen_blocks++;
					buffer += bytes_writ;
					continue;
				}
				bytes_writ += ext2_write_block(drive, block_num, buffer);
				seen_blocks++;
				buffer += bytes_writ;
			}
			// printf("writ %d bytes\n", bytes_writ);
			
			return bytes_writ;
		}
		case 2: {
			// recursive case: 
			/* - Now we write a bunch of blocks, each containing INDIRECT_CAPACITY block indices
			 * - We first write the blocks as before
			 * - Instead of copying the result to the buffer, we first call the function recursively (with rec lev 1) on it
			 * - Then we concatenate all the writes into the result buffer.
			 * 
			 * Each block is a list of length INDIRECT_CAPACITY. 
			 * The maximum number of bytes we can write through one indirect block is (BLOCK_SIZE * INDIRECT_CAPACITY)
			 */

			uint32_t start_block		 = offset / (BLOCK_SIZE * INDIRECT_CAPACITY);
			uint32_t first_block_offset	 = offset % (BLOCK_SIZE * INDIRECT_CAPACITY);
			uint32_t end_block 			 = (offset + size) / (BLOCK_SIZE * INDIRECT_CAPACITY) - ( ((offset + size) % ((BLOCK_SIZE * INDIRECT_CAPACITY))) == 0 ? 1 : 0 );
			uint32_t last_block_boundary = (offset + size - 1) % ((BLOCK_SIZE * INDIRECT_CAPACITY)) + 1;
			
			// printf("last block boundary: %d\n", last_block_boundary);

			uint16_t seen_blocks = 0;
			uint32_t total_bytes_writ = 0;
			for (uint32_t i = start_block; i <= end_block && i < max_blocks; i++) {
				uint32_t block_num = blocks[i];
				void* block_data   = ext2_read_block(drive, block_num);

				uint32_t meta_offset = (i == start_block) ? first_block_offset 		: 0;
				uint32_t meta_size 	 = ((i == end_block)   ? last_block_boundary 	: INDIRECT_CAPACITY * BLOCK_SIZE) - meta_offset;

				// printf("metaoffset: %d, metasize: %d\n", meta_offset, meta_size);

				uint32_t buffer_offset = total_bytes_writ;
				total_bytes_writ += ext2_write_blocks_from_blocklist(drive, block_data, 1, meta_offset, meta_size, buffer + buffer_offset);

				// we now need to copy this buffer with an offset of offset and a size of size.

				// uint32_t copy_start = (i == start_block) ? offset % (BLOCK_SIZE * INDIRECT_CAPACITY) : 0;
				// uint32_t copy_end = (i == end_block) ?  (offset + size) % ( BLOCK_SIZE * INDIRECT_CAPACITY) : BLOCK_SIZE * INDIRECT_CAPACITY;
				uint32_t copy_start = 0;
				uint32_t copy_end = meta_size;

				// printf("copy start: %d, copy end: %d, block offset: %d, buffer offset: %d\n", copy_start, copy_end, (i - start_block), buffer_offset);

				// printf("writing block %d:\tbuffer size: %u, (e-s): %u\n", i, meta_size, copy_end - copy_start);

				kfree(block_data);

				seen_blocks++;
			}
			return total_bytes_writ;
		}
		case 3: {

			break;
		}
		default: {
			//TODO: SCASS TUTT COS; SHE'S TURNED THE WEANS AGAINST US
			printf("error: Tried to write n-ly indirect block with n > 3\n");
		}
	}
	return 0;
}

uint32_t ext2_write_to_inode (uint8_t drive, struct ext2_inode* inode, uint32_t offset, uint32_t size, uint8_t* buffer) {
	uint32_t start_block = offset / BLOCK_SIZE;
	uint32_t end_block = (offset + size) / BLOCK_SIZE - ( ((offset + size) % BLOCK_SIZE) == 0 ? 1 : 0 );
	uint32_t block_offset = offset % BLOCK_SIZE;

	uint32_t bytes_writ = 0;
	if (start_block < 12) { // Write the part that fits in the direct blocks
		bytes_writ += ext2_write_blocks_from_blocklist(drive, inode->i_blocks, 0, offset, size, buffer);
		if (end_block < 12) {
			return size; // all done
		}
		buffer += bytes_writ;
		size -= bytes_writ;
		offset = 0;
		bytes_writ = 0;
	} else {
		offset -= 12 * BLOCK_SIZE;
	}

	if (start_block < INDIRECT_CAPACITY + 12) {
		// Write the part that fits in the singly indirect blocks
		uint8_t* singlynd = ext2_read_block(drive, inode->i_singly_ind);

		uint32_t bytes_to_write = (offset + size) < BLOCK_SIZE * INDIRECT_CAPACITY ?
			size :
			(BLOCK_SIZE * (INDIRECT_CAPACITY) - offset); 

		bytes_writ += ext2_write_blocks_from_blocklist(drive, (uint32_t*)singlynd, 1, offset, bytes_to_write, buffer);

		if(offset + size < (BLOCK_SIZE * (INDIRECT_CAPACITY))) { // Case: all fits within direct and singly indirect
			return size;
		}

		size -= bytes_writ;
		buffer += bytes_writ;
		offset = 0;
		bytes_writ = 0;
	} else {
		offset -= INDIRECT_CAPACITY * BLOCK_SIZE;
	}

	if (start_block < (INDIRECT_CAPACITY + 12) + (DOUBLY_INDIRECT_MAX_BLOCKS)) { // case: start writing before end of doubly indirect
		uint8_t* doublynd = ext2_read_block(drive, inode->i_doubly_ind);

		uint32_t max = (DOUBLY_INDIRECT_MAX_BLOCKS);
		uint32_t bytes_to_write = offset + size < BLOCK_SIZE * max ?
			size :
			(BLOCK_SIZE * max - offset);

		uint32_t bytes_writ = ext2_write_blocks_from_blocklist(drive, (uint32_t*)doublynd, 2, offset, bytes_to_write, buffer);

		// If we wrote all the bytes, we're done
		if (bytes_writ >= bytes_to_write) {
			return size;
		}

		// If we didn't write all the bytes, we need to update the buffer and continue
		buffer += bytes_writ;
		size -= bytes_writ;
		offset = 0;
		bytes_writ = 0;
	} else {
		// triply indirect not yet implemented.
		printf("error: Tried to write to triply indirect block, which is not yet implemented.\n");
		return 0;
	}

	return size;
}

uint32_t* ext2_alloc_blocks (uint8_t drive, uint32_t block_count) {
	if (block_count < superblock->s_free_blocks_count) {
		uint32_t* allocated_blocks = kmalloc (block_count * sizeof(uint32_t));
		bitmap_t block_bitmap = ext2_read_block(drive, gdt->bg_block_bitmap);
		
		uint32_t group_offset = superblock->s_first_data_block;

		// Load first GDT
		struct ext2_group_desc* gdt = ext2_get_block_group_descriptor(drive, group_offset);
		uint32_t block_group = 0;

		// Allocate the blocks
		for (uint32_t i = 0; i < block_count; i++) {
			superblock->s_free_blocks_count--;

			while(gdt->bg_free_blocks_count == 0) {
				// Move to next block group
				// TODO: Check if there are supergroup backups
				block_group++;
				group_offset += superblock->s_blocks_per_group;
				kfree(block_bitmap);
				gdt = ext2_get_block_group_descriptor(drive, block_group * superblock->s_blocks_per_group);
				block_bitmap = ext2_read_block(drive, gdt->bg_block_bitmap);
				// printf("Looking into group %d for kfree blocks...\n", block_group);
			}

			gdt->bg_free_blocks_count--;
			// No need to handle the case where there are no kfree blocks, as we already checked above
			allocated_blocks[i] = get_free(block_bitmap);
			set_bitmap(block_bitmap, allocated_blocks[i]);
			allocated_blocks[i] += group_offset;

			// Write back updated GDT
			ext2_write_block_group_descriptor_table(drive);

			// Write back the updated block bitmap
			ext2_write_block(drive, gdt->bg_block_bitmap, block_bitmap);
		}
		kfree(block_bitmap);

		ext2_write_superblock(drive);
		// ext2_write_block_group_descriptor_table(drive);

		// Zero out the allocated blocks
		uint8_t* zero_block = kmalloc(BLOCK_SIZE);
		memset(zero_block, 0, BLOCK_SIZE);
		for(uint32_t i = 0; i < block_count; i++) {
			ext2_write_block(drive, allocated_blocks[i], zero_block);
		}
		kfree(zero_block);

		return allocated_blocks;
	} else {
		printf("error: Not enough kfree blocks to allocate %d blocks.\n", block_count);
		return NULL;
	}
}

static inline uint32_t ext2_get_directory_entry_actual_size (struct ext2_directory_entry* entry) {
	uint32_t size = sizeof(struct ext2_directory_entry) - sizeof(char*) + (sizeof(char) * entry->name_length);
	if (size % 4 != 0) {
		size += 4 - (size % 4);
	}
	return size;
}

static void ext2_add_blocks_to_inode (uint8_t drive, struct ext2_inode_indexed inode, uint32_t count) {
	uint32_t* new_blocks = ext2_alloc_blocks(drive, count);
	uint32_t remaining = count;
	// Fill direct blocks first
	for (uint8_t i = 0; i < 12 && remaining > 0; i++) {
		if (inode.inode->i_blocks[i] == 0) {
			inode.inode->i_blocks[i] = new_blocks[count - remaining];
			remaining--;
			// printf("Allocated direct block %d: %d\n", i, inode.inode->i_blocks[i]);
		}
		else {
			// printf("Direct block %d already allocated: %d\n", i, inode.inode->i_blocks[i]);
		}
	}

	if (remaining == 0) {
		goto end;
	}

	// Singly indirect blocks
	// First check if we need to allocate a new block for the singly indirect pointer
	if (inode.inode->i_singly_ind == 0) {
		uint32_t* singly_ind_block = ext2_alloc_blocks(drive, 1);
		inode.inode->i_singly_ind = singly_ind_block[0];
		kfree(singly_ind_block);
		inode.inode->i_sectors += BLOCK_SIZE / SECTOR_SIZE;
		// printf("Allocated singly indirect block: %d\n", inode.inode->i_singly_ind);
	}

	// Read the singly indirect block
	uint32_t* singly_ind_data = (uint32_t*)ext2_read_block(drive, inode.inode->i_singly_ind);

	bool singly_ind_written = false;
	// Fill the singly indirect block
	for (uint32_t i = 0; i < INDIRECT_CAPACITY && remaining > 0; i++) {
		if (singly_ind_data[i] == 0) {
			singly_ind_data[i] = new_blocks[count - remaining];
			remaining--;
			// printf("Allocated singly indirect block entry %d: %d\n", i, singly_ind_data[i]);
			singly_ind_written = true;
		}
		else {
			// printf("Singly indirect block entry %d already allocated: %d\n", i, singly_ind_data[i]);
		}
	}
	if (singly_ind_written) {
		// Write back the singly indirect block
		ext2_write_block(drive, inode.inode->i_singly_ind, (uint8_t*)singly_ind_data);
		kfree(singly_ind_data);
	}
	if (remaining == 0) {
		goto end;
	}

	// Doubly indirect blocks
	// First check if we need to allocate a new block for the doubly indirect pointer
	if (inode.inode->i_doubly_ind == 0)	{
		uint32_t* doubly_ind_block = ext2_alloc_blocks(drive, 1);
		inode.inode->i_doubly_ind = doubly_ind_block[0];
		kfree(doubly_ind_block);
		inode.inode->i_sectors += BLOCK_SIZE / SECTOR_SIZE;
		// printf("Allocated doubly indirect block: %d\n", inode.inode->i_doubly_ind);
	}

	// Read the doubly indirect block
	uint32_t* doubly_ind_data = (uint32_t*)ext2_read_block(drive, inode.inode->i_doubly_ind);
	bool doubly_ind_written = false;
	// For each entry in the doubly indirect block
	for (uint32_t i = 0; i < INDIRECT_CAPACITY && remaining > 0; i++) {
		// Check if we need to allocate a new singly indirect block
		if (doubly_ind_data[i] == 0) {
			uint32_t* new_singly_ind_block = ext2_alloc_blocks(drive, 1);
			doubly_ind_data[i] = new_singly_ind_block[0];
			doubly_ind_written = true;
			kfree(new_singly_ind_block);
			inode.inode->i_sectors += BLOCK_SIZE / SECTOR_SIZE;
			// printf("Allocated new singly indirect block for doubly indirect entry %d: %d\n", i, doubly_ind_data[i]);
		}
		// Read the singly indirect block
		uint32_t* singly_ind_data = (uint32_t*)ext2_read_block(drive, doubly_ind_data[i]);
		bool singly_ind_written = false;
		// Fill the singly indirect block
		for (uint32_t j = 0; j < INDIRECT_CAPACITY && remaining > 0; j++) {
			if (singly_ind_data[j] == 0) {
				singly_ind_data[j] = new_blocks[count - remaining];
				remaining--;
				// printf("Allocated doubly indirect block entry %d, singly indirect entry %d: %d\n", i, j, singly_ind_data[j]);
				singly_ind_written = true;
			}
			else {
				// printf("Doubly indirect block entry %d, singly indirect entry %d already allocated: %d\n", i, j, singly_ind_data[j]);
			}
		}
		if (singly_ind_written) {
			// Write back the singly indirect block
			ext2_write_block(drive, doubly_ind_data[i], (uint8_t*)singly_ind_data);
			kfree(singly_ind_data);
		} else {
			kfree(singly_ind_data);
		}
	}
	if (doubly_ind_written) {
		// Write back the doubly indirect block
		ext2_write_block(drive, inode.inode->i_doubly_ind, (uint8_t*)doubly_ind_data);
		kfree(doubly_ind_data);
	} else {
		kfree(doubly_ind_data);
	}

	if (remaining == 0) {
		goto end;
	}

	// TODO: handle indirect blocks
	printf("error: ext2_add_blocks_to_inode: Not enough space to allocate %d blocks to inode %d. Triply indirect blocks not yet implemented.\n", count, inode.index);
end:
	inode.inode->i_sectors += (count * BLOCK_SIZE) / SECTOR_SIZE;
	ext2_commit_changes_to_inode(drive, inode);
	kfree(new_blocks);
}

static uint32_t ext2_count_allocated_blocks (struct ext2_inode* inode) {
	uint32_t count = (inode->i_sectors * SECTOR_SIZE) / BLOCK_SIZE;
	uint32_t blocks = count;
	if (blocks > 12) {
		// Account for singly indirect block
		count -= 1;
		blocks -= 1;

		if (blocks > 12 + INDIRECT_CAPACITY + 1) {
			// Account for doubly indirect block
			count -= 1;
			blocks -= 1;

			// Account for singly indirect blocks pointed to by doubly indirect block
			// GENERALISED FORMULA:
			uint32_t tmp = blocks - 12 - INDIRECT_CAPACITY - 1;
			tmp = tmp > (INDIRECT_CAPACITY + 1) * (INDIRECT_CAPACITY + 1) ? (INDIRECT_CAPACITY + 1) * (INDIRECT_CAPACITY + 1) : tmp;
			count -= (tmp + (INDIRECT_CAPACITY + 1) - 1) / (INDIRECT_CAPACITY + 1);

			if (blocks > 12 + INDIRECT_CAPACITY + INDIRECT_CAPACITY * INDIRECT_CAPACITY) {
				// TODO: Handle triply indirect blocks
				printf("error: ext2_count_allocated_blocks: Triply indirect blocks not yet implemented.\n");
			}
		}
	}
	// TODO: instead of BLOCK_SIZE, use actual block size from superblock
	return count;
}

// Read a directory entry from a directory inode at a given offset.
// Reading it directly does not include the name, so we first read the entry to get its actual size, then read it again with the correct size.
struct ext2_directory_entry* ext2_read_directory_entry (uint8_t drive, struct ext2_inode_indexed parent, uint32_t offset) {
	struct ext2_directory_entry* entry = (struct ext2_directory_entry*)ext2_read_from_inode(drive, parent.inode, offset, sizeof(struct ext2_directory_entry));

	if (entry == NULL) {
		return NULL;
	}

	uint32_t actual_size = ext2_get_directory_entry_actual_size(entry);
	kfree(entry);

	return (struct ext2_directory_entry*)ext2_read_from_inode(drive, parent.inode, offset, actual_size);
}

struct ext2_directory_entry* ext2_add_directory_entry (uint8_t drive, struct ext2_inode_indexed parent, struct ext2_directory_entry* entry) {
	if (!ext2_is_directory(parent.inode)) {
		printf("error: Parent inode is not a directory.\n");
		return NULL;
	}

	// Calculate size of new entry (4-byte aligned)
	uint32_t new_entry_size = ext2_get_directory_entry_actual_size(entry);

	// Iterate through directory entries to find either some kfree space at the end of a block, or the last entry in the directory
	// Since a directory always contains at least the . and .. entries, we can initialize prev_entry and current_entry to them
	// Even if we immediately kfree the prev_entry at the first iteration, we need to read it first to get its size_of_entry
	struct ext2_directory_entry* prev_entry = ext2_read_directory_entry(drive, parent, 0);
	struct ext2_directory_entry* current_entry = ext2_read_directory_entry(drive, parent, prev_entry->size_of_entry);
	uint32_t offset = prev_entry->size_of_entry;
	uint32_t prev_entry_actual_size;
	do {
		kfree(prev_entry);
		prev_entry = current_entry;
		
		// Debug prints
		char name[current_entry->name_length + 1];
		memcpy(name, &(current_entry->name), current_entry->name_length);
		name[current_entry->name_length] = '\0';
		printf("Current entry inode: %d, offset: %d, name: %s\n", current_entry->inode, offset, name);
		
		// Calculate actual size of current entry (4-byte aligned, not including padding)
		prev_entry_actual_size = ext2_get_directory_entry_actual_size(current_entry);

		printf("size of entry: %d, actual size: %u\n", current_entry->size_of_entry, prev_entry_actual_size);

		offset += current_entry->size_of_entry;
		printf("Next offset: %d\n", offset);
		current_entry = ext2_read_directory_entry(drive, parent, offset);

		if (prev_entry->size_of_entry - prev_entry_actual_size >= new_entry_size) {
			// There's enough space in the current block to fit the new entry
			printf("Found space for new entry after offset %d\n", offset - prev_entry->size_of_entry);
			break;
		}
	} while (current_entry != NULL);
	
	// Offset should point at the point right before the previous entry
	offset -= prev_entry->size_of_entry;
	
	printf("Prev entry actual size: %d, new entry size: %d\n", prev_entry_actual_size, new_entry_size);

	bool should_write_prev_entry = false;
	if (prev_entry->size_of_entry - prev_entry_actual_size >= new_entry_size) {
		printf("Resizing previous entry from %d to %d\n", prev_entry->size_of_entry, prev_entry_actual_size);
		entry->size_of_entry = prev_entry->size_of_entry - prev_entry_actual_size;
		prev_entry->size_of_entry = prev_entry_actual_size;
		should_write_prev_entry = true;
	} else {
		printf("Putting new entry in a new block\n");
		entry->size_of_entry = BLOCK_SIZE;
		parent.inode->i_size += BLOCK_SIZE;
		// Check if we need to allocate a new block
		if (ext2_count_allocated_blocks(parent.inode) * BLOCK_SIZE < parent.inode->i_size) {
			printf("Allocating new block for directory\n");
			ext2_add_blocks_to_inode(drive, parent, 1);
		}
	}

	printf("New entry size of entry: %d\n", entry->size_of_entry);

	// Write back the modified blocks
	if (should_write_prev_entry) {
		ext2_write_to_inode(drive, parent.inode, offset, prev_entry->size_of_entry, (uint8_t*)prev_entry);
	}
	ext2_write_to_inode(drive, parent.inode, offset + prev_entry->size_of_entry, entry->size_of_entry, (uint8_t*)entry);

	kfree(prev_entry);
	if (current_entry != NULL) {
		kfree(current_entry);
	}
	return entry;
}

struct ext2_inode_indexed ext2_create_inode (uint8_t drive, uint16_t mode, struct ext2_inode_indexed parent, const char* name) {
	if (!ext2_is_directory(parent.inode)) {
		printf("error: Parent inode is not a directory.\n");
		return (struct ext2_inode_indexed){.inode = NULL, .index = 0};
	}
	if (superblock->s_free_inodes_count == 0) {
		printf("error: No kfree inodes available.\n");
		return (struct ext2_inode_indexed){.inode = NULL, .index = 0};
	}

	// Allocate a new inode
	superblock->s_free_inodes_count--;
	gdt->bg_free_inodes_count--;
	bitmap_t inode_bitmap = ext2_read_block(drive, gdt->bg_inode_bitmap);
	uint32_t free_inode_idx = get_free(inode_bitmap);
	set_bitmap(inode_bitmap, free_inode_idx);
	// TODO: Set block group inode bitmap

	struct ext2_inode_indexed inode = {.inode = kmalloc (sizeof(struct ext2_inode)), .index = free_inode_idx + 1}; // inodes are 1-indexed
	memset(inode.inode, 0, sizeof(struct ext2_inode));
	inode.inode->i_mode = mode;
	inode.inode->i_size = 0;
	inode.inode->i_links_count = 1;

	// Create directory entry for the new inode
	const uint32_t name_length = strlen(name);
	uint32_t new_entry_size = sizeof(struct ext2_directory_entry) - sizeof(char*) + (sizeof(char) * name_length);
	if (new_entry_size % 4 != 0) {
		new_entry_size += 4 - (new_entry_size % 4);
	}
	struct ext2_directory_entry* new_entry = kmalloc(new_entry_size);
	memset(new_entry, 0, new_entry_size);
	new_entry->name_length = name_length;
	new_entry->inode = inode.index;
	
	if (CHK_FLAG(superblock->s_req_features, EXT2_DIRS_CONTAIN_TYPE_FIELD)) {
		new_entry->type_indicator = ext2_is_directory(inode.inode) ? EXT2_DIRECTORY : EXT2_REGULAR_FILE;
	}

	memcpy(&(new_entry->name), name, name_length);

	// Add the directory entry to the parent directory
	ext2_add_directory_entry(drive, parent, new_entry);

	uint8_t blocks_to_preallocate = ext2_is_directory(inode.inode) ? superblock->s_prealloc_dir_blocks : superblock->s_prealloc_blocks;
	
	ext2_add_blocks_to_inode(drive, inode, blocks_to_preallocate);
	
	// Commit changes to disk
	ext2_write_superblock(drive);
	ext2_write_block_group_descriptor_table(drive);
	ext2_write_block(drive, gdt->bg_inode_bitmap, inode_bitmap);
	ext2_commit_changes_to_inode(drive, inode);
	
	kfree(inode_bitmap);
	return inode;
}

uint32_t ext2_write(uint8_t drive, struct ext2_inode_indexed inode, uint32_t offset, uint32_t size, uint8_t* buffer) {
	// Calculate how many blocks are needed
	uint32_t end_offset = offset + size;
	uint32_t end_block = ROUND_UP(end_offset, BLOCK_SIZE) / BLOCK_SIZE;
	uint32_t current_blocks = ext2_count_allocated_blocks(inode.inode);
	uint32_t blocks_needed = end_block - current_blocks;
	if (blocks_needed > 0) {
		// printf("end_offset: %d, end_block: %d, current_blocks: %d, blocks_needed: %d, total inode blocks: %d\n", end_offset, end_block, current_blocks, blocks_needed, (inode.inode->i_sectors * SECTOR_SIZE) / BLOCK_SIZE);
		ext2_add_blocks_to_inode(drive, inode, blocks_needed);
	}

	inode.inode->i_size = end_offset;
	ext2_commit_changes_to_inode(drive, inode);

	return ext2_write_to_inode(drive, inode.inode, offset, size, buffer);
}

uint32_t ext2_append(uint8_t drive, struct ext2_inode_indexed inode, uint32_t size, uint8_t* buffer) {
	return ext2_write(drive, inode, inode.inode->i_size, size, buffer);
}