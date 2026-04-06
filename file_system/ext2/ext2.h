#pragma once
#include "../../utils/typedefs.h"

struct ext2_superblock {
    /* 0–83 (from o.g. table) */
    uint32_t s_inodes_count;             // 0x00: Total number of inodes in file system
    uint32_t s_blocks_count;             // 0x04: Total number of blocks in file system
    uint32_t s_r_blocks_count;           // 0x08: Number of blocks reserved for superuser
    uint32_t s_free_blocks_count;        // 0x0C: Total number of unallocated blocks
    uint32_t s_free_inodes_count;        // 0x10: Total number of unallocated inodes
    uint32_t s_first_data_block;         // 0x14: Block number of block containing superblock
    uint32_t s_log_block_size;           // 0x18: log2(block size) - 10
    uint32_t s_log_frag_size;            // 0x1C: log2(fragment size) - 10
    uint32_t s_blocks_per_group;         // 0x20: Number of blocks in each block group
    uint32_t s_frags_per_group;          // 0x24: Number of fragments in each block group
    uint32_t s_inodes_per_group;         // 0x28: Number of inodes in each block group
    uint32_t s_mtime;                    // 0x2C: Last mount time (POSIX time)
    uint32_t s_wtime;                    // 0x30: Last written time (POSIX time)
    uint16_t s_mnt_count;                // 0x34: Number of times volume mounted since last check
    uint16_t s_max_mnt_count;            // 0x36: Mounts allowed before consistency check
    uint16_t s_magic;                    // 0x38: Ext2 signature (0xEF53)
    uint16_t s_state;                    // 0x3A: File system state
    uint16_t s_errors;                   // 0x3C: What to do when error detected
    uint16_t s_minor_rev_level;          // 0x3E: Minor portion of version
    uint32_t s_lastcheck;                // 0x40: POSIX time of last consistency check
    uint32_t s_checkinterval;            // 0x44: Interval between forced checks
    uint32_t s_creator_os;               // 0x48: Operating system ID
    uint32_t s_rev_level;                // 0x4C: Major portion of version
    uint16_t s_def_resuid;               // 0x50: User ID that can use reserved blocks
    uint16_t s_def_resgid;               // 0x52: Group ID that can use reserved blocks

    /* 84–235 (from extended table) */
    uint32_t s_first_ino;                // 0x54: First non-reserved inode
    uint16_t s_inode_size;               // 0x58: Size of each inode structure
    uint16_t s_block_group_nr;           // 0x5A: Block group this superblock is part of
    uint32_t s_opt_features;             // 0x5C: Optional features
    uint32_t s_req_features;             // 0x60: Required features
    uint32_t s_ro_opt_features;          // 0x64: Read-only compatible features
    uint8_t  s_uuid[16];                 // 0x68: File system ID
    char     s_volume_name[16];          // 0x78: Volume name (C-string)
    char     s_last_mounted[64];         // 0x88: Path volume was last mounted to
    uint32_t s_algo_bitmap;              // 0xC8: Compression algorithms used
    uint8_t  s_prealloc_blocks;          // 0xCC: Blocks to preallocate for files
    uint8_t  s_prealloc_dir_blocks;      // 0xCD: Blocks to preallocate for directories
    uint16_t s_padding;                  // 0xCE: Unused
    uint8_t  s_journal_uuid[16];         // 0xD0: Journal ID
    uint32_t s_journal_inum;             // 0xE0: Journal inode
    uint32_t s_journal_dev;              // 0xE4: Journal device
    uint32_t s_last_orphan;              // 0xE8: Head of orphan inode list

    uint8_t  s_reserved[1024 - 236];     // 0xEC–0x3FF: Unused
} __attribute__((packed));
 
enum state {
    EXT2_STATE_CLEAN = 1,
    EXT2_STATE_ERROR = 2
};

enum err_handling_method {
    EXT2_IGNORE_ERROR = 1,
    EXT2_REMOUNT_READ_ONLY = 2,
    EXT2_PANIC = 3,
};

enum creator_os_id {
    EXT2_LINUX = 0,
    EXT2_HURD = 1,
    EXT2_MASIX = 2,
    EXT2_FREEBSD = 3,
    OTHER = 4
};

enum opt_feature_flags {
    EXT2_PREALLOCATE = 0x01,
    EXT2_AFS_SERVER_INODE_EXIST = 0x02, // ????
    EXT2_HAS_JOURNAL = 0x04, // EXT3
    EXT2_INODES_EXT_ATTRIBUTE = 0x08,
    EXT2_CAN_RESIZE = 0x10,
    EXT2_DIRS_USE_HASH = 0x20
};

enum req_feature_flags {
    EXT2_COMPRESSION = 0x01,
    EXT2_DIRS_CONTAIN_TYPE_FIELD = 0x02,
    EXT2_REPLAY_JOURNAL = 0x04,
    EXT2_USES_JOURNAL_DEVICE = 0x08
};

enum ro_feature_flags {
    EXT2_SPARSE = 0x01,
    EXT2_64 = 0x02,
    EXT2_DIRS_AS_BINARY_TREE = 0x04
};

struct ext2_group_desc {
    uint32_t bg_block_bitmap;        // 0x00: Block address of block usage bitmap
    uint32_t bg_inode_bitmap;        // 0x04: Block address of inode usage bitmap
    uint32_t bg_inode_table;         // 0x08: Starting block address of inode table
    uint16_t bg_free_blocks_count;   // 0x0C: Number of unallocated blocks in group
    uint16_t bg_free_inodes_count;   // 0x0E: Number of unallocated inodes in group
    uint16_t bg_used_dirs_count;     // 0x10: Number of directories in group
    uint16_t bg_pad;                 // 0x12: Padding (unused)
    uint32_t bg_reserved[3];         // 0x14–0x1F: Unused
} __attribute__((packed));


struct ext2_inode {
    uint16_t i_mode;          // 0–1: Type and Permissions
    uint16_t i_uid;           // 2–3: User ID
    uint32_t i_size;          // 4–7: Lower 32 bits of size in bytes
    uint32_t i_atime;         // 8–11: Last Access Time
    uint32_t i_ctime;         // 12–15: Creation Time
    uint32_t i_mtime;         // 16–19: Last Modification Time
    uint32_t i_dtime;         // 20–23: Deletion Time
    uint16_t i_gid;           // 24–25: Group ID
    uint16_t i_links_count;   // 26–27: Count of hard links
    uint32_t i_sectors;        // 28–31: Count of disk sectors (not Ext2 blocks) in use by this inode, not counting the actual inode structure nor directory entries linking to the inode.
    uint32_t i_flags;         // 32–35: Flags
    uint32_t i_osd1;          // 36–39: Operating System Specific value #1
    uint32_t i_blocks[12];    // 40–99: Pointers to direct blocks (12)
    uint32_t i_singly_ind;    // Singly indirect pointer - I'm singlintherain
    uint32_t i_doubly_ind;    // Doubly indirect pointer - feckin eejit!
    uint32_t i_triply_ind;    // Triply indirect pointer - feckin eejit!
    uint32_t i_generation;    // 100–103: Generation number (used for NFS)
    uint32_t i_file_acl;      // 104–107: Extended attribute block (or reserved)
    uint32_t i_dir_acl;       // 108–111: Upper 32 bits of file size or directory ACL
    uint32_t i_faddr;         // 112–115: Block address of fragment
    uint8_t  i_osd2[12];      // 116–127: Operating System Specific value #2
};

struct ext2_inode_indexed {
	struct ext2_inode* inode;
	uint32_t index;
};

// File type mask (bitwise AND with 0xF000 to extract)
enum ext2_inode_type {
    EXT2_S_IFSOCK = 0xC000,  // Socket
    EXT2_S_IFLNK  = 0xA000,  // Symbolic link
    EXT2_S_IFREG  = 0x8000,  // Regular file
    EXT2_S_IFBLK  = 0x6000,  // Block device
    EXT2_S_IFDIR  = 0x4000,  // Directory
    EXT2_S_IFCHR  = 0x2000,  // Character device
    EXT2_S_IFIFO  = 0x1000,  // FIFO (pipe)
};


// Special permission bits
enum ext2_inode_special_perms {
    EXT2_S_ISUID = 0x0800,  // Set UID bit
    EXT2_S_ISGID = 0x0400,  // Set GID bit
    EXT2_S_ISVTX = 0x0200,  // Sticky bit
};

// Owner permissions
enum ext2_inode_owner_perms {
    EXT2_S_IRUSR = 0x0100,  // Owner read
    EXT2_S_IWUSR = 0x0080,  // Owner write
    EXT2_S_IXUSR = 0x0040,  // Owner execute
};

// Group permissions
enum ext2_inode_group_perms {
    EXT2_S_IRGRP = 0x0020,  // Group read
    EXT2_S_IWGRP = 0x0010,  // Group write
    EXT2_S_IXGRP = 0x0008,  // Group execute
};

// Others permissions
enum ext2_inode_other_perms {
    EXT2_S_IROTH = 0x0004,  // Others read
    EXT2_S_IWOTH = 0x0002,  // Others write
    EXT2_S_IXOTH = 0x0001,  // Others execute
};


enum ext2_inode_flags {
    EXT2_SECRM_FL         = 0x00000001,  // Secure deletion
    EXT2_UNRM_FL          = 0x00000002,  // Undelete
    EXT2_COMPR_FL         = 0x00000004,  // Compress file
    EXT2_SYNC_FL          = 0x00000008,  // Synchronous updates
    EXT2_IMMUTABLE_FL     = 0x00000010,  // Immutable file
    EXT2_APPEND_FL        = 0x00000020,  // Append only
    EXT2_NODUMP_FL        = 0x00000040,  // Do not dump file
    EXT2_NOATIME_FL       = 0x00000080,  // Do not update access time
    EXT2_HASH_IDX_FL      = 0x00010000,  // Hash Indexed Directory
    EXT2_AFS_FL           = 0x00020000,  // AFS directory
    EXT2_JOURNAL_FL       = 0x00040000,  // Journal file data
};

enum type_indicator {
    EXT2_UNKNOWN = 0,
    EXT2_REGULAR_FILE = 1,
    EXT2_DIRECTORY = 2,
    EXT2_CHAR_DEVICE = 3,
    EXT2_BLOCK_DEVICE = 4,
    EXT2_FIFO = 5,
    EXT2_SOCKET = 6,
    EXT2_SYMLINK = 7
};

struct ext2_directory_entry {
    uint32_t inode;
    uint16_t size_of_entry;
    uint8_t name_length;
    uint8_t type_indicator;
    char* name;
} __attribute__((packed));

struct ext2_superblock* ext2_read_superblock (uint8_t drive);
struct ext2_group_desc* ext2_get_block_group_descriptor_table (uint8_t drive);
struct ext2_inode_indexed ext2_read_inode (uint8_t drive, uint32_t inode);
void* ext2_read_block (uint8_t drive, uint32_t block);
bool ext2_is_directory (struct ext2_inode* inode);
uint8_t* ext2_read_from_inode (uint8_t drive, struct ext2_inode* inode, uint32_t offset, uint32_t size);
uint32_t ext2_write_to_inode (uint8_t drive, struct ext2_inode* inode, uint32_t offset, uint32_t size, uint8_t* buffer);
struct ext2_inode_indexed ext2_create_inode (uint8_t drive, uint16_t mode, struct ext2_inode_indexed parent, const char* name);

void ext2_list_files_in_dir (struct ext2_inode* dir);
struct ext2_inode_indexed find_file_by_name (struct ext2_inode* dir, const char* target_name);
uint32_t ext2_append(uint8_t drive, struct ext2_inode_indexed inode, uint32_t size, uint8_t* buffer);
uint32_t ext2_write(uint8_t drive, struct ext2_inode_indexed inode, uint32_t offset, uint32_t size, uint8_t* buffer);

void ext2_init ();