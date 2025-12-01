#include "mellos/fs.h"

#include "dynamic_mem.h"
#include "mellos/kernel/dentry.h"
#include "mellos/kernel/kernel.h"
#include "mellos/kernel/kernel_stdio.h"
#include "mellos/kernel/mount_manager.h"

#include "stddef.h"
#include "string.h"

#include "mellos/ramfs.h"
#include "statfs.h"

#include "mellos/block_device.h"

#include "errno.h"

#include "filesystems/procfs.h"

#include "ramdisk.h"

// todo: split this file into 2 files, 1 for mounts and 1 for filesystems

bool mounts_initialized = false;
/**
 * uses a vfs_mount_t type
 */
linked_list_t* mounts = NULL;
bool fs_registry_initialized = false;
/**
 * uses a fs_type_t type
 */
linked_list_t* registered_filesystems = NULL;

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
	return root_mnt;
}

// todo: more NULL protection
bool get_filesystem_filter_function(list_node_t* node, void* filterdata) {
	if (node == NULL || node->data == NULL) {
		kfprintf(kstderr, "Invalid node or data in get_file_filter_function\n");
		asm("hlt\n");
		return false;
	}
	const vfs_mount_t* mnt = node->data;
	kprintf("comparing: %s\n", filterdata);
	if (get_or_create_dentry(filterdata)->inode == NULL) {

		if (mnt->sb == NULL) {
			kfprintf(kstderr, "%p sb is null\n", mnt->root);
			return false;
		}

		if (mnt->root == NULL) {
			kfprintf(kstderr, "%p root is null\n", mnt);
			return false;
		}
		kfprintf(kstderr, "no inode: %s, %s\n", filterdata, mnt->root->dentry->name);
		return false;
	}
	if (mnt->root == NULL) {
		kfprintf(kstderr, "No filesystem root\n");
		asm("hlt\n");
		return false;
	}
	if (mnt->root->dentry == NULL) {
		kfprintf(kstderr, "No filesystem dentry\n");
		asm("hlt\n");
		return false;
	}
	if (mnt->root->dentry->name == NULL) {
		kfprintf(kstderr, "No filesystem dentry name\n");
		asm("hlt\n");
		return false;
	}
	return strcmp(mnt->root->dentry->name, filterdata) == 0;
}

vfs_mount_t* get_proc_mount() {
	if (!mounts_initialized) {
		kfprintf(kstderr, "Mounts not initialized, cannot get /proc mount\n");
		return NULL;
	}
	kprintf("%i mounts\n", mounts->size);
	const list_node_t* proc_mnt =
	    linked_list_get_node(mounts, "/proc", get_filesystem_filter_function);
	if (proc_mnt == NULL) {
		kfprintf(kstderr, "No proc filesystem found in mounts!\n");
		return NULL;
	}

	return proc_mnt->data;
}

void init_vfs() {
	kprintf("Initializing VFS...\n");
	if (mounts != NULL) {
		kprintf("VFS already initialized!\n");
		return;
	}

	mounts = linked_list_create();

	// todo: implement some fs for this
	vfs_mount_t* mnt = kmalloc(sizeof(vfs_mount_t));
	mnt->root = kmalloc(sizeof(inode_t));
	root_mnt = mnt;
	mnt->root->sb = kmalloc(sizeof(superblock_t));
	mnt->sb = mnt->root->sb;
	mnt->mounted = true;

	dentry_t* droot = dops.dentry_alloc(NULL, "/");
	dops.dentry_init(droot, mnt->root);
	droot->dops = &dops;
	droot->refcount++;
	droot->inode = mnt->root;
	mnt->root->dentry = droot;

	linked_list_push_back(mounts, mnt);

	procfs_mount_data_t procfs_mount_data = {.parent_dentry = droot};

	vfs_mount_t* procfs_mount =
	    procfs_getfs()->mount(get_block_device_by_name("ram0p0"), "/proc", &procfs_mount_data);

	if (procfs_mount == NULL) {
		kfprintf(kstderr, "Failed to get procfs mount from mount()\n");
		asm("hlt\n");
		return;
	}

	linked_list_push_back(mounts, procfs_mount);

	kprintf("VFS initialized!\n");
	mounts_initialized = true;
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
	linked_list_destroy(mounts);
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
	linked_list_push_back(registered_filesystems, procfs_get_file_ops());
}

void destroy_fs_registry() {
	list_node_t* node = registered_filesystems->head;
	while (node) {
		statfs_t* statfs = node->data;
		kfree(statfs);
		node = node->next;
	}
	linked_list_destroy(registered_filesystems);
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
	char* str = kmalloc(sizeof(char) * 10);
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
		linked_list_push_back(registered_filesystems, mount);
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
	linked_list_push_back(registered_filesystems, mount);
}

void unmount(vfs_mount_t* mount) {
	list_node_t* node = registered_filesystems->head;
	while (node) {
		if (node->data == mount) {
			if (!mount->sb->fs->unmount(mount)) {
				kfprintf(kstderr, "Failed to unmount cleanly %s\n", mount->sb->bd->name);
				return;
			}
			kfree(mount->sb->bd);
			linked_list_remove(registered_filesystems, node);
			return;
		}
		node = node->next;
	}
}

/**
 * Returns NULL on invalid superblock
 */
vfs_mount_t* mount(superblock_t* sb, block_device_t* bdev, const char* path) {
	statfs_t* statfs = kmalloc(sizeof(statfs_t));
	if (path == NULL) {
		path = "/";
	}
	vfs_mount_t* mount_point = NULL;

	if (sb == NULL || sb->root == NULL || sb->ops == NULL || sb->ops->statfs == NULL) {
		kprintf("Invalid superblock!\n");
		goto free;
	}

	mount_point = sb->fs->mount(bdev, path, statfs);

	sb->ops->statfs(sb, statfs);

	kprintf("Mounting FS: %s type=%s path=%s\n", sb->fs->name, statfs->f_type, path);
	mount_point->sb = sb;

	linked_list_push_back(mounts, mount_point);

free:
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