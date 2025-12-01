#include "fdef.h"
#include "mellos/block_device.h"
#include "mellos/fs.h"
#include "mellos/kernel/kernel_stdio.h"

#include "dynamic_mem.h"
#include "mellos/kernel/mount_manager.h"
#include "shell/shell.h"

FDEF(ls) {
	kprintf("searching directory: %s\n", s);
	kprintf("todo: implement\n");
}

bool filter(vfs_mount_t* m) {
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

void print_nodes_for(const block_device_t* bd) {

	const linked_list_t* list = get_block_devices();
	if (list == NULL) {
		kfprintf(kstderr, "lsblk: get_mounts() is null!\n");
		return;
	}

	const list_node_t* node = list->head;
	while (node) {
		const block_device_t* it = node->data;
		if (!it) {
			kfprintf(kstderr, "lsblk: Corrupted mount node. Rebooting is advised.\n");
			break;
		}
		if (!(it->flags & BLOCK_DEVICE_FLAG_PARTITION)) {
			node = node->next;
			continue;
		}

		if (it->name == NULL) {
			kfprintf(kstderr, "lsblk: block device name is null!\n");
			break;
		}

		if (it == bd) {
			const vfs_mount_t* mnt = get_mount_for_bd(bd);
			if (mnt == NULL) {
				kfprintf(kstderr, "lsblk: get_mount_for_bd() is null!\n");
				break;
			}

			kprintf("%s%s %s %s %u %s\n", "|-->", it->name, "part",
			        it->flags & BLOCK_DEVICE_FLAG_READ_ONLY ? "true" : "false",
			        RAMFS_BLOCK_SIZE * RAMFS_MAX_BLOCKS, mnt->root->dentry);
		}
		node = node->next;
	}
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

		if (bd->parent == NULL) {

			const vfs_mount_t* mnt = get_mount_for_bd(bd);
			char* mount_point = NULL;
			if (mnt != NULL) {
				mount_point = mnt->root->dentry->name;
			} else {
				mount_point = "";
			}

			kprintf("%s%s %s %s %u %s\n", "", bd->name,
		        "disk", bd->flags & BLOCK_DEVICE_FLAG_READ_ONLY ? "true" : "false",
		        bd->logical_block_size * bd->num_blocks, mount_point);
		} else {
			print_nodes_for(bd);
		}

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
	inode_t* inode = get_inode_from_path_relative(get_root_mount()->root, s);
	if (!inode) {
		kprintf("cd: directory not found");
		return;
	}
	set_working_dir(inode);
	refreshShell();
}
