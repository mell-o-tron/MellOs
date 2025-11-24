#include "fdef.h"
#include "mellos/block_device.h"
#include "mellos/fs.h"
#include "mellos/kernel/kernel_stdio.h"

#include "mellos/kernel/mount_manager.h"
#include "dynamic_mem.h"
#include "shell/shell.h"

FDEF(ls) {
	kprintf("searching directory: %s", s);
}

bool filter(mount_t* m) {
	if (m->sb == NULL) {
		kfprintf(kstderr, "superblock is null!\n");
		return false;
	}
	if (m->root == NULL) {
		kfprintf(kstderr, "mountpoint is null!\n");
		return false;
	}
	if (m->sb->fs == NULL) {
		kfprintf(kstderr, "fs is null\n");
		return false;
	}
	if (m->sb->ops == NULL) {
		kfprintf(kstderr, "ops is null\n");
		return false;
	}
	if (m->sb->ops->statfs == NULL) {
		kfprintf(kstderr, "ops->statfs is null\n");
		return false;
	}
	return true;
}

void print_nodes_for(block_device_t* bd) {
	if (bd == NULL) {
		kfprintf(kstderr, "lsblk: Block device is null!\n");
		return;
	}
	if (!are_mounts_initialized()) {
		kfprintf(kstderr, "lsblk: Trying to access mounts while mounts are not initialized!\n");
		return;
	}

	linked_list_t* mounts = get_mounts();
	if (mounts == NULL) {
		kfprintf(kstderr, "lsblk: get_mounts() is null!\n");
		return;
	}
	if (linked_list_is_empty(mounts)) {
		kfprintf(kstderr, "lsblk: No mounts found!\n");
		return;
	}
	list_node_t* node = mounts->head;
	if (node == NULL) {
		return;
	}
	mount_t* m;
	statfs_t* statfs = kmalloc(sizeof(statfs_t));
	char* path = kmalloc(256);
	while (node) {
		m = node->data;
		if (!filter(m)) {
			goto free;
		}
		m->sb->ops->statfs(m->sb, statfs);
		//rebuild_path(m->sb->fs->name, path);
		if (m->sb->bd == bd) {
			kprintf("|-->%s %s %s %llu %s %s\n", m->sb->fs->name, NULL, bd->flags & 1 ? "yes" : "no",
			        statfs->f_blocks * statfs->f_bsize, NULL, "part");
		}
		node = node->next;
	}
free:
	kfree(path);
	kfree(statfs);
}

FDEF(lsblk) {
	kprintf("NAME TYPE READONLY SIZE MOUNTPOINTS\n");

	linked_list_t* devices = get_block_devices();
	if (!devices) {
		kfprintf(kstderr, "lsblk: No block devices found. Rebooting is advised.\n");
		return;
	}
	list_node_t* node = devices->head;
	if (!node) {
		kfprintf(kstderr, "lsblk: No block devices found. Rebooting is advised.\n");
		return;
	}
	while (node) {

		if (!(node->data)) {
			kfprintf(kstderr, "lsblk: Corrupted block device node. Rebooting is advised.\n");
			return;
		}
		block_device_t* bd = node->data;
		// todo: print mountpoint
		kprintf("%s %s %s %u %s\n", bd->name, bd->parent == NULL ? "disk" : "part", bd->flags & 1 ? "true" : "false",
		        bd->logical_block_size * bd->num_blocks, "");
		print_nodes_for(bd);

		node = node->next;
	}

	linked_list_t* list = get_registered_filesystems();
	if (list == NULL) {
		kfprintf(kstderr, "lsblk: get_registered_filesystems() is null!\n");
		return;
	}

	node = list->head;
	statfs_t* statfs = kmalloc(sizeof(statfs_t));
	while (node) {
		mount_t* m = node->data;
		if (!m) {
			kfprintf(kstderr, "lsblk: Corrupted mount node. Rebooting is advised.\n");
			goto free;
		}

		if (m->sb == NULL) {
			kfprintf(kstderr, "lsblk: fs is null\n");
			goto free;
		}

		if (m->sb->bd != NULL) {
			goto next;
		}

		if (!m->root->dentry) {
			kfprintf(kstderr, "lsblk: root dentry is null\n");
			goto free;
		}

		if (m->sb->ops == NULL) {
			kfprintf(kstderr, "lsblk: ops is null\n");
			goto free;
		}
		m->sb->ops->statfs(m->sb, statfs);
		kprintf("%s %s %s %u %s\n", m->sb->fs->name, "part",
			m->sb->flags & 1 ? "true" : "false",
			RAMFS_BLOCK_SIZE * RAMFS_MAX_BLOCKS,
			m->sb->root->dentry->name);
	next:
		node = node->next;
	}
free:
	kfree(statfs);
}

FDEF(pwd) {
	char* workingdir = kmalloc(1024);
	rebuild_path(get_working_dir(), &workingdir);
	kprintf("%s\n", workingdir);
	kfree(workingdir);
}

FDEF(cd) {
	if (s[0] == '\0') {
		kprintf("cd: no directory specified\n");
		return;
	}
	if (s[0] != '/') {
		kprintf("cd: directory must start with /\n");
		kprintf("cd: this is going to change in the future.\n");
		return;
	}
	set_working_dir(get_inode_from_path(*get_root_mount(), s));
}