#include "mellos/ramfs.h"
#include "dynamic_mem.h"
#include "errno.h"
#include "math.h"
#include "mellos/fs.h"
#include "mellos/kernel/kernel_stdio.h"
#include "stddef.h"
#include "string.h"
#include "mellos/fd.h"
#include "ramdisk.h"
#include "mem.h"

#include "mellos/kernel/mount_manager.h"

inode_ops_t ramfs_operations = {
    .create = &ramfs_create_file,
    .lookup = &ramfs_lookup,
    .mkdir = &ramfs_mkdir,
    .unlink = &ramfs_unlink,
    .readlink = &ramfs_readlink,
    .symlink = &ramfs_symlink,
    .link = &ramfs_link,
};

file_ops_t ramfs_fileops = {
    .read = &ramfs_read,
    .write = &ramfs_write,
    .readdir = &ramfs_readdir,
    .truncate = NULL,
    .ioctl = &ramfs_ioctl,
    .mmap = &ramfs_mmap,
};

super_ops_t ramfs_super_ops = {
	.statfs = &ramfs_statfs,
	.sync = &ramfs_sync,
};

superblock_t* root_ramfs_superblock;
superblock_t* proc_ramfs_superblock;
superblock_t* dev_ramfs_superblock;

inode_t* root_inode;
dentry_t* ramfs_root_dentry;

dentry_t* ramfs_get_root_dentry() {
	return ramfs_root_dentry;
}

typedef struct {
	uint64_t total_blocks;
} mount_data_t;

vfs_mount_t* ramfs_root_mount;

file_t* open_files;

fs_type_t* ramfs_fs_type;

// todo: init helpers so there wont be a need to write 50 lines of code to create 1 file
int ramfs_init() {
	ramfs_fs_type = kmalloc(sizeof(fs_type_t));
	ramfs_fs_type->name = "ramfs";

	ramfs_fs_type->mount = &ramfs_mount;
	ramfs_fs_type->unmount = &ramfs_unmount;

	open_files = kmalloc(sizeof(file_t) * MAX_OPEN_FILES);

	ramfs_root_dentry = kmalloc(sizeof(dentry_t));
	root_inode = kmalloc(sizeof(inode_t));
	ramfs_root_dentry->inode = root_inode;
	root_inode->dentry = ramfs_root_dentry;
	root_inode->mode =
	    S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IFDIR;
	root_inode->private = NULL;

	// todo: proper superblock init
	root_ramfs_superblock = kmalloc(sizeof(superblock_t));
	mount_data_t* mdata = kmalloc(sizeof(mount_data_t));
	mdata->total_blocks = 512;
	root_ramfs_superblock->root = root_inode;
	root_ramfs_superblock->ops = &ramfs_super_ops;
	mount(root_ramfs_superblock, get_block_device_by_name("ram0"), "/");

	root_inode->sb = root_ramfs_superblock;
	root_inode->ops = &ramfs_operations;
	root_inode->ref_count = 1;
	root_inode->fops = &ramfs_fileops;

	ramfs_root_mount = kmalloc(sizeof(vfs_mount_t));
	ramfs_root_mount->sb = root_ramfs_superblock;
	ramfs_root_mount->root = root_inode;

	root_inode->private = kmalloc(sizeof(struct ramfs_dir));
	struct ramfs_dir* ramfs_dir = root_inode->private;
	ramfs_dir->capacity = 64;
	ramfs_dir->count = 2;
	ramfs_dir->entries = kmalloc(sizeof(struct ramfs_dir_entry) * ramfs_dir->count);

	inode_t* dev_inode = kmalloc(sizeof(inode_t*));
	inode_t* proc_inode = kmalloc(sizeof(inode_t*));

	ramfs_create_file(root_inode, "proc", S_IFDIR | S_IRUSR | S_IWUSR, &proc_inode);
	ramfs_create_file(root_inode, "dev", S_IFDIR | S_IRUSR | S_IWUSR, &dev_inode);

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

	vfs_mount_t* proc_mount = kmalloc(sizeof(vfs_mount_t));
	proc_mount->root = kmalloc(sizeof(inode_t));


	proc_ramfs_superblock = kmalloc(sizeof(superblock_t));
	proc_ramfs_superblock->root = proc_mount->root;
	proc_ramfs_superblock->ops = &ramfs_super_ops;
	proc_ramfs_superblock->bd = get_block_device_by_name("ram0");

	proc_mount->sb = proc_ramfs_superblock;
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


	vfs_mount_t* dev_mount = kmalloc(sizeof(vfs_mount_t));
	dev_mount->root = kmalloc(sizeof(inode_t));


	dev_ramfs_superblock = kmalloc(sizeof(superblock_t));
	dev_ramfs_superblock->fs = ramfs_fs_type;
	dev_ramfs_superblock->identifier = RAMFS_IDENTIFIER;
	dev_ramfs_superblock->block_size = RAMFS_BLOCK_SIZE;
	dev_ramfs_superblock->total_blocks = RAMFS_MAX_BLOCKS;
	dev_ramfs_superblock->root = dev_mount->root;
	dev_ramfs_superblock->ops = &ramfs_super_ops;
	dev_ramfs_superblock->bd = get_block_device_by_name("ram0");

	dev_mount->sb = dev_ramfs_superblock;

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

	register_fs(dev_mount);
	register_fs(proc_mount);
	return 0;
}

/**
 * no op
 * @param sb superblock
 * @return always 0
 */
int ramfs_sync(superblock_t* sb) {
	return 0;
}
/**
 * gets fs stats, expects a pointer to preallocated memory, sizeof statfs_t
 * @param sb superblock
 * @param st stats to write
 * @return success
 */
int ramfs_statfs(superblock_t* sb, statfs_t* st) {
	st->f_type = RAMFS_IDENTIFIER;
	st->f_bsize = sb->block_size;
	st->f_blocks = sb->total_blocks;
	return 0;
}

vfs_mount_t* ramfs_mount(superblock_t* sb, block_device_t* bdev, void* data) {

	vfs_mount_t* m = kmalloc(sizeof(vfs_mount_t));
	mount_data_t* mount_data = data;
	// if the user is trying to mount with a non-null root inode,
	// we notify them by setting errno and returning NULL
	// this behaviour is exclusive to ramfs as ramfs bypasses
	// the block device layer
	if (sb->root != NULL) {
		kfree(m);
		errno = EINVAL;
		return NULL;
	}
	m->root = kmalloc(sizeof(inode_t));
	m->sb = sb;
	m->sb->identifier = RAMFS_IDENTIFIER;
	m->sb->block_size = RAMFS_BLOCK_SIZE;
	if (mount_data != NULL) {
		m->sb->total_blocks = mount_data->total_blocks;
	}
	m->sb->fs = ramfs_fs_type;
	m->mounted = true;
	m->root = sb->root;
	m->root->sb = sb;
	m->root->fops = &ramfs_fileops;
	m->root->ref_count = 1;
	m->root->sb->bd = bdev;
	return m;
}

int ramfs_unmount(superblock_t* sb) {

	return -ENOSYS;
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
size_t ramfs_readdir(file_t* f, void* dirent_out) {
	return -1;
}
size_t ramfs_ioctl(file_t* f, unsigned long cmd, void* arg) {
	return -1;
}
size_t ramfs_mmap(file_t* f, void* addr, size_t length, int prot, int flags) {
	return -1;
}
file_t* ramfs_get_file(const char* path) {
	return NULL;
}

inode_t* ramfs_resolve_file(const char* path) {
	bool sc = true;
	inode_t* last = root_ramfs_superblock->root;

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

int ramfs_create_file(inode_t* dir, const char* name, uint32_t mode, inode_t** out) {
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
	inode_t* inode = get_inode_from_path_relative(get_root_mount()->root, path);

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
