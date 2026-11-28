#pragma once
#include "stdint.h"
/**
 * Filesystem statistics (returned by statfs)
 */
typedef struct {
	uint64_t f_type;    /* filesystem magic number / identifier */
	uint64_t f_blocks;  /* total blocks in filesystem */
	uint64_t f_bfree;   /* free blocks */
	uint64_t f_bavail;  /* free blocks for non-root users */
	uint64_t f_files;   /* total inodes */
	uint64_t f_ffree;   /* free inodes */
	uint32_t f_bsize;   /* optimal block size */
	uint32_t f_namelen; /* max filename length */
} statfs_t;