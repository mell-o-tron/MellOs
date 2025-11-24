#include "mellos/fs.h"

#include "dynamic_mem.h"
#include "mellos/kernel/kernel_stdio.h"
#include "mellos/kernel/mount_manager.h"

#include "string.h"

#include "mellos/ramfs.h"
#include "statfs.h"

bool mounts_initialized = false;
/**
 * list of mount_t type
 */
linked_list_t* mounts = NULL;
bool fs_registry_initialized = false;
/**
 * mount_t type
 */
linked_list_t* registered_filesystems = NULL;

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

// todo: more NULL protection
bool get_root_filter_function(list_node_t* node, void* filterdata) {
	if (node == NULL) return false;
	mount_t* mnt = node->data;
	return strcmp(mnt->root->dentry->name, filterdata) == 0;
}

mount_t* get_root_mount() {
	if (!mounts_initialized) {
		return NULL;
	}
	return linked_list_get_node(mounts, "/", get_root_filter_function)->data;
}

void init_mount_manager() {
	mounts_initialized = true;
	if (mounts != NULL) {
		kprintf("Mount manager already initialized!\n");
		return;
	}

	mounts = linked_list_create();

	mount_t* mnt = kmalloc(sizeof(mount_t));
	mnt->root = ramfs_get_root_dentry()->inode;
	mnt->root->dentry = ramfs_get_root_dentry();
	mnt->sb = mnt->root->sb;
	mnt->mounted = true;

	linked_list_push_back(mounts, mnt);
}

void uninitialize_mount_manager() {
	list_node_t* node = mounts->head;
	while (node) {
		superblock_t* sb = node->data;
		sb->fs->unmount(sb);
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

void register_fs(mount_t* mount) {
	if (!fs_registry_initialized) {
		return;
	}
	linked_list_push_back(registered_filesystems, mount);
}

void unregister_fs(mount_t* mount) {
	list_node_t* node = registered_filesystems->head;
	while (node) {
		if (node->data == mount) {
			linked_list_remove(registered_filesystems, node);
			return;
		}
		node = node->next;
	}
}

void mount(superblock_t* sb, const char* path) {
	statfs_t* statfs = kmalloc(sizeof(statfs_t));
	if (path == NULL) {
		path = "/";
	}
	if (sb == NULL || sb->ops == NULL || sb->ops->statfs == NULL) {
		kprintf("Invalid superblock!\n");
		goto free;
	}

	sb->ops->statfs(sb, statfs);

	kprintf("Registering FS: %s type=%s path=%s\n", sb->fs->name, statfs->f_type, path);

	mount_t* mount_point = kmalloc(sizeof(mount_t));
	mount_point->sb = sb;

	linked_list_push_back(mounts, mount_point);

free:
	kfree(statfs);
}

bool are_mounts_initialized() {
	return mounts_initialized;
}