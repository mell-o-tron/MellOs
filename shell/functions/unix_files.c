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

void print_nodes_for(block_device_t* bd) {
	if (!are_mounts_initialized()) {
		kprintf("Trying to access mounts while mounts are not initialized!\n");
		return;
	}

	linked_list_t* mounts = get_mounts();
	if (mounts == NULL) {
		kprintf("mounts is null\n");
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
		if (m->sb == NULL) {
			kprintf("superblock is null\n");
			goto free;
		}
		if (m->root == NULL) {
			kprintf("mountpoint is null\n");
			goto free;
		}
		if (m->sb->fs == NULL) {
			kprintf("fs is null\n");
			goto free;
		}
		if (bd == NULL) {
			kprintf("block device is null\n");
			goto free;
		}
		m->sb->ops->statfs(m->sb, statfs);
		//rebuild_path(m->mountpoint, path);
		if (m->sb->bd == bd) {
			kprintf("%s %s %s %llu %s %s\n", path, NULL, bd->flags & 1 ? "yes" : "no",
			        statfs->f_blocks * statfs->f_bsize, NULL, "part");
		}
		node = node->next;
	}
free:
	kfree(path);
	kfree(statfs);
}

FDEF(lsblk) {
	kprintf("NAME FSTYPE READONLY SIZE TYPE MOUNTPOINTS\n");

	linked_list_t* devices = get_block_devices();
	list_node_t* node = devices->head;
	if (!devices) {
		kprintf("No block devices found. Rebooting is advised.\n");
		return;
	}
	if (!node) {
		kprintf("No block devices found. Rebooting is advised.\n");
		return;
	}
	while (node) {

		if (!(node->data)) {
			kprintf("Corrupted block device node. Rebooting is advised.\n");
			return;
		}
		block_device_t* bd = node->data;
		kprintf("%s %s %s %llu %s %s\n", bd->name, NULL, bd->flags & 1 ? "yes" : "no",
		        bd->logical_block_size * bd->num_blocks, NULL, "disk");
		print_nodes_for(bd);

		node = node->next;
	}
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