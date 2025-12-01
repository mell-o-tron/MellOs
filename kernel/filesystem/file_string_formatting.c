#include "mellos/fs.h"

#include "dynamic_mem.h"
#include "mellos/kernel/kernel_stdio.h"
#include "mellos/kernel/mount_manager.h"
#include "mem.h"
#include "string.h"

// todo: this but it allocates, no need to pass buffer
int rebuild_path(inode_t* inode, char** buffer) {
	inode_t* node_i = inode;
	size_t str_len = 0;
	uint32_t i = 0;

	while (node_i) {
		*buffer[i++] = '/';
		str_len = strlen(node_i->dentry->name) - 1; // remove \0
		if (!memcpy(*buffer + i, node_i->dentry->name, str_len)) {
			return -2;
		}
		node_i = inode->parent;
		if (node_i == NULL) {
			return -1;
		}
		i += str_len;
	}
	*buffer[i] = '\0';
	return 0;
}

/**
 * Gets an inode for a file from a unix style path.
 * @param inode Mount point
 * @param path Path to the file that we are trying to retrieve the inode for. Needs to start at
 * root.
 * @return Inode for the file defined by path
 */
inode_t* get_inode_from_path_relative(inode_t* inode, const char* path) {

	size_t offset = 0;
	inode_t* tmp_inode = kmalloc(sizeof(inode_t));
	if (path[offset] != '/') {
		kfprintf(kstderr, "Path must start with \"/\"!\n");
		return NULL;
	}
	offset++;

	char* collected = kmalloc(strlen(path));
	while (true) {
		if (path[offset] == '/') {
			collected[offset] = '\0';
			inode->ops->lookup(inode, collected, &tmp_inode);
			inode = tmp_inode;
			if (inode == NULL) {
				kfree(collected);
				return NULL;
			}
			path += offset;
			offset = 0;
			offset++;
			continue;
		}
		if (path[offset] == '\0') {
			if (inode->ops == NULL) {
				kfprintf(kstderr, "node->ops is null (%s)\n", inode->dentry->name);
				kfree(collected);
				kfree(tmp_inode);
				return NULL;
			}
			inode->ops->lookup(inode, collected, &tmp_inode);
			inode = tmp_inode;
			kfree(collected);
			return inode;
		}
		collected[offset] = path[offset];
		offset++;
	}
}

inode_t* get_inode_from_path(const char* path) {
	return get_inode_from_path_relative(get_root_mount()->root, path);
}
