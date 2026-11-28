#include "assert.h"
#include "linked_list.h"
#include "macros.h"
#include "mellos/fs.h"

#include "dynamic_mem.h"
#include "mellos/kernel/dentry.h"
#include "mellos/kernel/kernel.h"
#include "mellos/kernel/mount_manager.h"

#include "kernel_stdio.h"
#include "spinlock.h"
#include "stddef.h"
#include "string.h"

#include "statfs.h"

#include "mellos/block_device.h"

#include "errno.h"

#include "filesystems/procfs.h"
#include "filesystems/fat.h"

#include "ramdisk.h"
#include "string.h"

// todo: split this file into 2 files, 1 for mounts and 1 for filesystems

bool mounts_initialized = false;
/**
 * uses a vfs_mount_t type
 */
linked_list_t* mounts = NULL;
volatile int mounts_lock = 0;

bool fs_registry_initialized = false;
/**
 * uses a fs_type_t type
 */
linked_list_t* registered_filesystems = NULL;
volatile int fslock = 0;

vfs_mount_t* root_mnt = NULL;

static dentry_ops_t dops = {
    .dentry_alloc = &dentry_alloc,
    .dentry_delete = &dentry_delete,
    .dentry_init = &dentry_init,
};

/**
 * get all the mounts
 * @return list of mount_t
 */
linked_list_t* get_mounts() {
	return mounts;
}

linked_list_t* get_registered_filesystems() {
	return registered_filesystems;
}

vfs_mount_t* get_root_mount() {
	if (kstrcmp(root_mnt->sb->root->dentry->name, "/") == 0) {
        return root_mnt;
	}
	return NULL;
}

bool get_filesystem_filter_function(list_node_t* node, void* filterdata) {
	kassert(node != NULL);
	kassert(node->data != NULL);
	const vfs_mount_t* mnt = node->data;

	kassert(mnt != NULL);



	kassert(mnt->root != NULL);
	kassert(mnt->root->dentry != NULL);
	dentry_t* de = mnt->root->dentry;
	kassert(de != NULL);
	kassert(de->inode != NULL);
	if (de->inode == NULL) {
		asm("hlt");

		if (mnt->sb == NULL) {
			kprintf("%p sb is null\n", mnt->root);
			return false;
		}

		if (mnt->root == NULL) {
			kprintf("%p root is null\n", mnt);
			return false;
		}
		kprintf("no inode: %s, %s\n", filterdata, mnt->root->dentry->name);
		return false;
	}
	if (mnt->root == NULL) {
		kprintf("No filesystem root\n");
		return false;
	}
	if (mnt->root->dentry == NULL) {
		kprintf("No filesystem dentry\n");
		return false;
	}
	if (mnt->root->dentry->name == NULL) {
		kprintf("No filesystem dentry name\n");
		return false;
	}
	return kstrcmp(mnt->root->dentry->name, filterdata) == 0;
}

vfs_mount_t* get_proc_mount() {
	if (!mounts_initialized) {
		kprintf("Mounts not initialized, cannot get /proc mount\n");
		asm("hlt");
		return NULL;
	}
	kprintf("%i mounts\n", mounts->size);
	SpinLock(&mounts_lock);
	const list_node_t* proc_mnt =
	    linked_list_get_node(mounts, "/proc", &get_filesystem_filter_function);
	SpinUnlock(&mounts_lock);
	kassert(proc_mnt != NULL);
	if (proc_mnt == NULL) {
		kprintf("No proc filesystem found in mounts!\n");
		return NULL;
	}

	return proc_mnt->data;
}

int init_vfs(char* root_part) {
	if (!root_part) {
		kprintf("root partition not set in kernel command line");
		return 1;
	}
	if (kstrlen(root_part) < 2) {
		kprintf("root partition name length too short");
		return 1;
	}
	kprintf("Initializing VFS...\n");
	if (mounts != NULL) {
		kprintf("VFS already initialized!\n");
		return 1;
	}

	mounts = linked_list_create();

	bdev_initialize_blockdevices();

	procfs_mount_data_t procfs_mount_data = {.parent_dentry = root_mnt->root->dentry};

	void* ramdata = kzalloc(RAMFS_BLOCK_SIZE * 32);
	//void* ramdata2 = kzalloc(RAMFS_BLOCK_SIZE * 32);
	block_device_t* ramdisk = ramdisk_create("ram0", ramdata, RAMFS_BLOCK_SIZE * 32, RAMFS_BLOCK_SIZE);
	//block_device_t* ramdisk2 = ramdisk_create("ram1", ramdata2, RAMFS_BLOCK_SIZE * 32, RAMFS_BLOCK_SIZE);
	inode_t** procfile_ifitexists;
	root_mnt->root->ops->lookup(root_mnt->root, "proc", &procfile_ifitexists);
	if (*procfile_ifitexists != NULL) {
        kprintf((*(procfile_ifitexists))->dentry->name);
    }

	if (procfile_ifitexists == NULL) {
		kprintf("/proc does not exist, creating...");
		if (root_mnt->root->ops->mkdir(root_mnt->root, "proc",
		                               S_IRGRP | S_IRGRP | S_IROTH | S_IXGRP | S_IXUSR | S_IXOTH |
		                                   S_ISUID) < 0) {
			kprintf("/proc creation failed, halting");
			asm("hlt");
        }
	}


	vfs_mount_t* procfs_mount =
	    procfs_getfs()->mount(ramdisk, "/proc", &procfs_mount_data);

	if (procfs_mount == NULL) {
		kprintf("Failed to get procfs mount from mount()\n");
		asm("hlt\n");
		return 1;
	}

	SpinLock(&mounts_lock);
	linked_list_push_back(mounts, procfs_mount);
	SpinUnlock(&mounts_lock);
	kprintf("VFS initialized!\n");
	mounts_initialized = true;
	return 0;
}

void uninitialize_mount_manager() {
	list_node_t* node = mounts->head;

	// todo: start droppping from the end of the tree instead of like this
	while (node) {
		superblock_t* sb = node->data;
		vfs_mount_t* mnt = get_mount_for_sb(sb);
		if (mnt == NULL) {
			kpanic_message("Failed to get mount for superblock\n");
			return;
		}
		sb->fs->unmount(mnt);
		node = node->next;
	}

	SpinLock(&mounts_lock);
	linked_list_destroy(mounts);
	SpinUnlock(&mounts_lock);
	mounts = NULL;
	mounts_initialized = false;
}

void init_fs_registry() {
	if (registered_filesystems != NULL) {
		kprintf("Filesystem registry already initialized!\n");
		return;
	}
	fs_registry_initialized = true;
	registered_filesystems = linked_list_create();
	SpinLock(&fslock);
	linked_list_push_back(registered_filesystems, procfs_get_file_ops());
	linked_list_push_back(registered_filesystems, fat_get_super_ops());
	SpinUnlock(&fslock);
}

void destroy_fs_registry() {
	list_node_t* node = registered_filesystems->head;
	while (node) {
		statfs_t* statfs = node->data;
		kfree(statfs);
		node = node->next;
	}

	SpinLock(&fslock);
	linked_list_destroy(registered_filesystems);
	SpinUnlock(&fslock);
}

int register_filesystem(fs_type_t* fs) {
	if (!fs_registry_initialized) {
		kfprintf(kstderr, "Filesystem registry is not initialized!\n");
		return -EAGAIN;
	}
	return 0;
}

void register_fs(vfs_mount_t* mount) {
	if (!fs_registry_initialized) {
		return;
	}
	char* str = kzalloc(sizeof(char) * 10);
	// who said java is verbose? it does not need stupid amounts of error checks!
	if (str == NULL) {
		kfprintf(kstderr, "Failed to allocate memory for filesystem name\n");
		return;
	}
	if (mount->root == NULL) {
		kfprintf(kstderr, "Filesystem root is not mounted\n");
		kfree(str);
		return;
	}
	if (mount->root->dentry == NULL) {
		kfprintf(kstderr, "Filesystem root dentry is NULL\n");
		kfree(str);
		return;
	}
	if (mount->root->dentry->name == NULL) {
		kfprintf(kstderr, "Filesystem root dentry name is NULL\n");
		kfree(str);
		return;
	}
	// todo: add shortcut to siblings
	if (mount->root->dentry->parent == NULL) {
		// this is root
		kprintf("Adding %s...", mount->root->dentry->name);
		ksnprintf(str, 10, "%sp%u", mount->root->sb->bd->name, 0);
		mount->root->sb->bd->children++;
		mount->sb->fs->name = str;

        SpinLock(&fslock);
		linked_list_push_back(registered_filesystems, mount);
        SpinUnlock(&fslock);
		return;
	}

	if (mount->root->dentry->parent->inode == NULL) {
		kfprintf(kstderr, "Filesystem root dentry parent inode is NULL\n");
		kfree(str);
		return;
	}

	if (mount->root->dentry->parent->inode->sb == NULL) {
		kfprintf(kstderr, "Filesystem root dentry parent inode sb is NULL\n");
		kfree(str);
		return;
	}

	if (mount->root->dentry->parent->inode->sb->bd == NULL) {
		kfprintf(kstderr, "Filesystem root dentry parent inode sb bd is NULL\n");
		kfree(str);
		return;
	}

	ksnprintf(str, 10, "%sp%u", mount->root->sb->bd->name, mount->root->sb->bd->children);
	mount->root->sb->bd->children++;
	mount->sb->fs->name = str;
	SpinLock(&fslock);
	linked_list_push_back(registered_filesystems, mount);
	SpinUnlock(&fslock);
}

/**
 * @param mount = NULL to remove all
 */
void unmount(vfs_mount_t* mount) {

	list_node_t* node = registered_filesystems->head;
	while (node) {
		if (mount == NULL || node->data == mount) {
			if (!mount->sb->fs->unmount(mount)) {
				kfprintf(kstderr, "Failed to unmount cleanly %s\n", mount->sb->bd->name);
				return;
			}
			kfree(mount->sb->bd);
			SpinLock(&fslock);
			linked_list_remove(registered_filesystems, node);
            SpinUnlock(&fslock);
			return;
		}
		node = node->next;
	}
}

/**
 * Returns NULL on invalid superblock
 */
vfs_mount_t* mount(superblock_t* sb, block_device_t* bdev, char* path) {
	statfs_t* statfs = kzalloc(sizeof(statfs_t));
	char* realpath = NULL;
	if (path == NULL) {
        realpath = "/";
	} else {
        realpath = kstrdup(path);
    }
    kassert(realpath);
	vfs_mount_t* mount_point = NULL;

	if (sb == NULL || sb->ops == NULL || sb->ops->statfs == NULL) {
		kprintf("Invalid superblock!, %b %b %b\n", sb == NULL, sb->ops == NULL, (sb->ops == NULL || sb->ops->statfs == NULL));
		goto free;
	}

	fat_mount_data_t* mdata = kzalloc(sizeof(fat_mount_data_t));
	mdata->is_root = path == NULL || kstrcmp("/", realpath) == 0;
	mdata->partition = ((partition_t*)bdev->driver_data);
	mdata->superblock = sb;
	mount_point = sb->fs->mount(bdev, (const char*)realpath, mdata);
	if (mount_point == NULL) {
		goto free;
	}

	kassert(mount_point->sb->ops != NULL);
	kassert(mount_point->sb->ops->statfs != NULL);
	kassert(statfs != NULL);

	mount_point->sb->ops->statfs(mount_point->sb, statfs);

#ifdef MELLOS_DEBUG
	kprintf("mounted filesystem: name=%s\ntype=%X (magic number)\n", mount_point->sb->fs->name, statfs->f_type);
	kprintf("mountpoint=%s\n", realpath);
#endif // MELLOS_DEBUG
	SpinLock(&mounts_lock);
	linked_list_push_back(mounts, mount_point);
    SpinUnlock(&mounts_lock);
free:
	if (path == NULL) {
        kfree(realpath);
    }
	kfree(statfs);
	return mount_point;
}

bool are_mounts_initialized() {
	return mounts_initialized;
}

vfs_mount_t* get_mount_for_bd(block_device_t* bd) {
	if (!mounts_initialized) {
		kfprintf(kstderr, "Trying to get block device mount while mounts are not initialized!");
		return NULL;
	}

	const list_node_t* node = mounts->head;
	while (node) {
		vfs_mount_t* mount = node->data;
		if (mount->sb == NULL) {
			kfprintf(kstderr, "get_mount_for_bd: Invalid mount found: %p (no superblock)\n", mount);
			return NULL;
		}
		if (mount->sb->bd == NULL) {
			kfprintf(kstderr, "get_mount_for_bd: Invalid mount found: %p (no block device)\n",
			         mount);
			return NULL;
		}
		if (mount->sb->bd == bd) {
			return mount;
		}
		node = node->next;
	}
	return NULL;
}

superblock_t* create_superblock(block_device_t* blockdevice, char* mount_point, fs_type_t* fs) {

	superblock_t* sb = kzalloc(sizeof(superblock_t));

	sb->bd = blockdevice;
	sb->block_size = blockdevice->logical_block_size;
	sb->total_blocks = blockdevice->num_blocks;
	return sb;
}

vfs_mount_t* get_mount_for_sb(superblock_t* sb) {
	if (!mounts_initialized) {
		kfprintf(kstderr,
		         "Trying to get mount from superblock while mounts are not initialized!\n");
		return NULL;
	}
	const list_node_t* node = mounts->head;
	while (node) {
		vfs_mount_t* mount = node->data;
		if (mount->sb == NULL) {
			kfprintf(kstderr, "get_mount_for_sb: Invalid mount found: %p (no superblock)\n", mount);
			return NULL;
		}
		if (mount->sb == sb) {
			return mount;
		}
		node = node->next;
	}
	return NULL;
}
