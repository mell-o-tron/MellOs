#include "dynamic_mem.h"
#include "errno.h"
#include "mellos/fd.h"
#include "mellos/ramfs.h"
#include "processes.h"
#include "string.h"

fd_t open_file_descriptors[FD_MAX_TOTAL];

fd_t* open_fd_standalone(fd_type_t type, mount_t* mount, process_t* process, int flags, int permissions, char* path) {
	const int fdid = find_first_free_fd();
	if (fdid < 0) {
		errno = ENFILE; // system wide limit reached
		return NULL;
	}
	if (open_file_descriptors[fdid].name != NULL) {
		errno = EBUSY;
		return NULL;
	}

	char* fd_name;

	open_file_descriptors[fdid].type = type;
	open_file_descriptors[fdid].flags = flags;
	switch (type) {
	case FD_TYPE_NULL:
		if (!strcmp(path, NULL_FILE)) {
			errno = EINVAL;
			return NULL;
		}
		file_t* null_file = ramfs_open_file_handle(NULL_FILE, FD_TYPE_NULL);
		if (null_file == NULL) {
			null_file = kmalloc(sizeof(file_t));
			null_file->inode->mode = S_IFCHR | permissions;
			null_file->inode->dentry->name = strdup(NULL_FILE);
			null_file->inode->ref_count = 1;


			open_file_descriptors[fdid].private_data = null_file;
		}

		fd_name = "null_fd";
		open_file_descriptors[fdid].name = kmalloc(strlen(fd_name));
		open_file_descriptors[fdid].name = fd_name;
		open_file_descriptors[fdid].private_data = null_file;
		break;
	case FD_TYPE_PIPE:
		fd_name = "pipe_fd";
		open_file_descriptors[fdid].name = kmalloc(strlen(fd_name));
		open_file_descriptors[fdid].name = fd_name;
		open_file_descriptors[fdid].private_data = kmalloc(sizeof(pipe_t));
		break;
	case FD_TYPE_FILE:
		fd_name = "file_fd";
		open_file_descriptors[fdid].name = kmalloc(strlen(fd_name));
		open_file_descriptors[fdid].name = fd_name;
		open_file_descriptors[fdid].private_data = kmalloc(sizeof(file_t));
		((file_t*)open_file_descriptors[fdid].private_data)->inode->mode = S_IFREG | permissions;
		((file_t*)open_file_descriptors[fdid].private_data)->inode->ref_count += 1;
		break;
	case FD_TYPE_DEVICE:
		fd_name = "device_fd";
		open_file_descriptors[fdid].name = kmalloc(strlen(fd_name));
		open_file_descriptors[fdid].name = fd_name;
		open_file_descriptors[fdid].private_data = kmalloc(sizeof(file_t));
		((file_t*)open_file_descriptors[fdid].private_data)->inode->mode = S_IFBLK | permissions;
		((file_t*)open_file_descriptors[fdid].private_data)->inode->ref_count += 1;
	case FD_TYPE_CHAR:
		fd_name = "char_fd";
		open_file_descriptors[fdid].name = kmalloc(strlen(fd_name));
		open_file_descriptors[fdid].name = fd_name;
		open_file_descriptors[fdid].private_data = kmalloc(sizeof(file_t));
		file_t *file = ramfs_open_file_handle(path, FD_TYPE_CHAR);
		((file_t*)open_file_descriptors[fdid].private_data)->inode =
		    file->inode;
		file->inode->ref_count--;


		((file_t*)open_file_descriptors[fdid].private_data)->inode->mode = S_IFCHR | permissions;
		((file_t*)open_file_descriptors[fdid].private_data)->inode->ref_count += 1;
	default:
		return NULL;
	}
	process->open_files_count++;
	return &open_file_descriptors[fdid];
}

int find_first_free_file_PID(uint32_t pid) {
	const process_t* process = get_process_by_pid(pid);

	for (int i = 0; i < MAX_OPEN_FILES_PROCESS; i++) {
		if (process->fd_table.fd_array[i].name == NULL) {
			if (i > process->fd_table.count) {
				krealloc(process->fd_table.fd_array, sizeof(fd_t) * process->fd_table.count,
				         sizeof(fd_t) * (i + 1));
			}
		}
	}
	return -EMFILE;
}

int find_first_free_fd() {
	for (int i = 0; i < MAX_OPEN_FILES; i++) {
		if (open_file_descriptors[i].name == NULL) {
			return i;
		}
	}
	return -1;
}

int find_first_free_fd_PID(uint32_t pid) {
	const process_t* process = get_process_by_pid(pid);

	for (int i = 0; i < MAX_OPEN_FILES_PROCESS; i++) {
		if (process->fd_table.fd_array[i].name == NULL) {
			if (i > process->fd_table.count) {
				krealloc(process->fd_table.fd_array, sizeof(fd_t) * process->fd_table.count,
				         sizeof(fd_t) * (i + 1));
			}
			return i;
		}
	}
	return -EMFILE;
}

int is_file_in_use(inode_t* file) {
	for (int i = 0; i < FD_MAX_TOTAL; i++) {
		if (open_file_descriptors[i].private_data == file &&
		    open_file_descriptors[i].type == FD_TYPE_FILE) {
			return 1;
		}
	}
	return 0;
}

int close_file_descriptor(fd_t* file) {
	if (file == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (file->status == FD_CLOSED) {
		return 0;
	}
	kfree(file->name);
	kfree(file->private_data);
	file->private_data = NULL;
	file->name = NULL;
	file->status = FD_CLOSED;
	return 0;
}

fd_t* get_file_descriptor(int fd) {
	return &open_file_descriptors[fd];
}