#include "mellos/ramfs.h"
#include "dynamic_mem.h"
#include "errno.h"
#include "math.h"
#include "mellos/fs.h"
#include "mellos/kernel/kernel_stdio.h"
#include "mem.h"
#include "string.h"

inode_ops ramfs_operations = {
    .create = &ramfs_create,
    .lookup = &ramfs_lookup,
    .mkdir = &ramfs_mkdir,
    .unlink = &ramfs_unlink,
    .readlink = &ramfs_readlink,
    .symlink = &ramfs_symlink,
    .link = &ramfs_link,
};

file_ops ramfs_fileops = {
    .read = &ramfs_read,
    .write = &ramfs_write,
    .readdir = &ramfs_readdir,
    .truncate = NULL,
    .ioctl = &ramfs_ioctl,
    .mmap = &ramfs_mmap,
};
superblock_t* ramfs_superblock;

inode_t* root_inode;
dentry_t* ramfs_root_dentry;

dentry_t* ramfs_get_root_dentry() {
	return ramfs_root_dentry;
}

mount_t* ramfs_mount;

file_t* open_files;

int ramfs_init() {
	open_files = kmalloc(sizeof(file_t) * MAX_OPEN_FILES);
	ramfs_root_dentry = kmalloc(sizeof(dentry_t));
	root_inode = kmalloc(sizeof(inode_t));
	ramfs_root_dentry->name = "/";
	ramfs_root_dentry->inode = root_inode;
	root_inode->dentry = ramfs_root_dentry;
	root_inode->mode =
	    S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IFDIR;
	root_inode->private = NULL;
	root_inode->sb = ramfs_superblock;
	root_inode->ops = &ramfs_operations;
	root_inode->ref_count = 1;
	root_inode->fops = &ramfs_fileops;

	ramfs_mount = kmalloc(sizeof(mount_t));
	ramfs_mount->sb = ramfs_superblock;
	ramfs_mount->root = root_inode;

	root_inode->private = kmalloc(sizeof(struct ramfs_dir));
	struct ramfs_dir* ramfs_dir = root_inode->private;
	ramfs_dir->capacity = 64;
	ramfs_dir->count = 2;
	ramfs_dir->entries = kmalloc(sizeof(struct ramfs_dir_entry) * ramfs_dir->count);

	inode_t* dev_inode = kmalloc(sizeof(inode_t*));
	inode_t* proc_inode = kmalloc(sizeof(inode_t*));

	ramfs_create(root_inode, "proc", S_IFDIR | S_IRUSR | S_IWUSR, &proc_inode);
	ramfs_create(root_inode, "dev", S_IFDIR | S_IRUSR | S_IWUSR, &dev_inode);

	if (dev_inode == NULL || proc_inode == NULL) {
		return -ENOMEM;
	}

	proc_inode->private = kmalloc(sizeof(struct ramfs_dir));
	dev_inode->private = kmalloc(sizeof(struct ramfs_dir));
	struct ramfs_dir* proc_dir = proc_inode->private;
	struct ramfs_dir* dev_dir = dev_inode->private;
	proc_dir->capacity = 1024;
	dev_dir->capacity = 128;
	proc_dir->count = 0;
	dev_dir->count = 0;

	mount_t* proc_mount = kmalloc(sizeof(mount_t));
	proc_mount->root = proc_inode;
	proc_mount->sb = kmalloc(sizeof(superblock_t));
	proc_mount->mounted = true;
	proc_mount->root->dentry = kmalloc(sizeof(dentry_t));
	proc_mount->root->dentry->inode = dev_inode;
	proc_mount->root->dentry->parent = ramfs_root_dentry;
	proc_mount->root->dentry->name = "/proc";
	proc_mount->root->mode =
		S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IFDIR;
	proc_mount->root->sb = proc_mount->sb;
	proc_mount->root->ops = &ramfs_operations;
	proc_mount->root->fops = &ramfs_fileops;
	proc_mount->root->ref_count = 1;


	mount_t* dev_mount = kmalloc(sizeof(mount_t));
	dev_mount->root = dev_inode;
	dev_mount->sb = kmalloc(sizeof(superblock_t));
	dev_mount->mounted = true;
	dev_mount->root->dentry = kmalloc(sizeof(dentry_t));
	dev_mount->root->dentry->inode = dev_inode;
	dev_mount->root->dentry->parent = ramfs_root_dentry;
	dev_mount->root->dentry->name = "/dev";
	dev_mount->root->mode =
		S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IFDIR;
	dev_mount->root->sb = dev_mount->sb;
	dev_mount->root->ops = &ramfs_operations;
	dev_mount->root->fops = &ramfs_fileops;
	dev_mount->root->ref_count = 1;

	register_fs(ramfs_mount);
	register_fs(dev_mount);
	register_fs(proc_mount);
	return 0;
}

int ramfs_lookup(inode_t* dir, const char* name, inode_t** out) {
	if (!(dir->mode & S_IRUSR)) {
		return -EACCES;
	}

	if (!S_ISDIR(dir->mode)) {
		return -EINVAL;
	}

	struct ramfs_dir* directory = dir->private;

	if (directory->count > 0) {
		for (uint32_t i = directory->count; i > 0; i--) {
			if (!strcmp(directory->entries[directory->count].name, name)) {
				if (directory->entries[directory->count].vnode->mode & S_IRUSR) {
					*out = directory->entries[directory->count].vnode;
					return 0;
				} else {
					return -EACCES;
				}
			}
		}
	}
	return -1;
}

int ramfs_mkdir(inode_t* dir, const char* name, uint32_t mode) {
	if (!(dir->mode & S_IWUSR)) {
		return -EACCES;
	}

	if (!S_ISDIR(dir->mode)) {
		return -EINVAL;
	}

	struct ramfs_dir* directory = dir->private;

	if (directory->count >= directory->capacity) {
		return -ENOSPC;
	}
	// todo: add buffering
	directory->entries =
	    krealloc(directory->entries, sizeof(struct ramfs_dir_entry) * (directory->count),
	             sizeof(struct ramfs_dir_entry) * (directory->count + 1));

	directory->count++;
	struct ramfs_dir_entry entry = directory->entries[directory->count];
	entry.vnode = kmalloc(sizeof(inode_t));
	entry.vnode->mode = mode;
	entry.vnode->ops = &ramfs_operations;
	entry.vnode->fops = &ramfs_fileops;
	entry.vnode->sb = dir->sb;
	entry.name = strdup(name);

	return -1;
}
int ramfs_unlink(inode_t* dir, const char* name) {
	return -1;
}
int ramfs_readlink(inode_t* vn, char* buf, size_t size) {
	return -1;
}
int ramfs_symlink(inode_t* dir, const char* name, const char* target) {
	return -1;
}
int ramfs_link(inode_t* dir, const char* name, inode_t* target) {
	return -1;
}
ssize_t ramfs_read(file_t* f, void* buf, size_t size, uint64_t offset) {
	if (!((f->inode->mode) & S_IRUSR)) {
		return -EACCES;
	}

	struct ramfs_file* file = f->inode->private;
	if (file == NULL) {
		return -ENOENT;
	}

	if (offset >= file->count) {
		return -EINVAL;
	}

	memcpy(buf, file->buffer + offset, umin(file->count - offset, size));

	return 0;
}
ssize_t ramfs_write(file_t* f, const void* buf, size_t size, uint64_t offset) {
	if (!((f->inode->mode) & S_IWUSR)) {
		return -EACCES;
	}

	struct ramfs_file* file = f->inode->private;
	if (file == NULL) {
		return -ENOENT;
	}

	if (size + offset > file->count) {
		file->buffer = krealloc(file->buffer, file->count, offset + size);
	}

	memcpy(file->buffer + offset, buf, size);
	return 0;
}
int ramfs_readdir(file_t* f, void* dirent_out) {
	return -1;
}
int ramfs_ioctl(file_t* f, unsigned long cmd, void* arg) {
	return -1;
}
int ramfs_mmap(file_t* f, void* addr, size_t length, int prot, int flags) {
	return -1;
}
file_t* ramfs_get_file(const char* path) {
	return NULL;
}

inode_t* ramfs_resolve_file(const char* path) {
	bool sc = true;
	inode_t* last = ramfs_superblock->root;

	char* current_dir_name = kmalloc(strlen(path));
	int path_iterator = 0;
	int name_iterator = 0;
	bool last_is_escape = false;
	while (sc) {

		char current_char = path[path_iterator];
		if (current_char == '\\') {
			if (!last_is_escape) {
				last_is_escape = true;
				continue;
			}
			last_is_escape = false;
		}
		if (current_char == '/') {
			if (!last_is_escape) {

				current_dir_name[name_iterator] = '\0';

				ramfs_lookup(last, current_dir_name, &last);

				name_iterator = 0;
			} else {
				last_is_escape = false;
			}
		} else if (current_char == '\0') {
			sc = false;
			current_dir_name[name_iterator] = '\0';
			ramfs_lookup(last, current_dir_name, &last);
		} else {
			current_dir_name[name_iterator] = current_char;
			name_iterator++;
			path_iterator++;
			ramfs_lookup(last, path, NULL);
		}
	}
	kfree(current_dir_name);
	return last;
}
void ramfs_list_dir(inode_t* dir) {
	if (dir == NULL) {
		return;
	}

	struct ramfs_dir* directory = dir->private;
	for (uint32_t i = 0; i < directory->count; i++) {
		kprintf("%s\n", directory->entries[i].name);
	}
}

int ramfs_create(inode_t* dir, const char* name, uint32_t mode, inode_t** out) {
	struct ramfs_dir* directory = dir->private;

	if (dir->mode & S_IWUSR) {
		inode_t* res = kmalloc(sizeof(inode_t));
		if (!res) {
			return -ENOMEM;
		}
		if (directory->count >= directory->capacity) {
			return -ENOSPC;
		}
		directory->count++;
		*out = res;
		res->mode = mode;
		res->ops = &ramfs_operations;
		res->fops = &ramfs_fileops;
		res->sb = dir->sb;
		if (!res->dentry) {
			res->dentry = kmalloc(sizeof(dentry_t));
			if (!res->dentry) {
				return -ENOMEM;
			}
		}
		res->dentry->name = strdup(name);
		res->dentry->inode = res;

		return 0;
	}
	return -EACCES;
}

file_t* ramfs_open_file_handle(char* path, int type) {
	if (path == NULL) {
		errno = EINVAL;
		return NULL;
	}
	if (type == FD_TYPE_PIPE) {
		errno = EINVAL;
		return NULL;
	}
	inode_t* inode = get_inode_from_path(*get_root_mount(), path);

	const int file_id = find_first_free_fd();
	if (file_id < 0) {
		errno = -file_id;
		return NULL;
	}

	open_files[file_id].inode = inode;
	open_files[file_id].ops = &ramfs_fileops;
	return &open_files[file_id];
}

int ramfs_close_file_handle(file_t* file) {
	if (file == NULL) {
		errno = EINVAL;
		return -1;
	}
	file->inode->ref_count--;
	if (file->inode->ref_count == 0) {
		kfree(file);
	}
	return 0;
}

int ramfs_set_file_mode(file_t* file, int mode) {
	// todo: more error checking
	if (file == NULL) {
		errno = EINVAL;
		return -1;
	}
	file->inode->mode = mode;
	return 0;
}