#include "dynamic_mem.h"
#include "file_system.h"
#include "math.h"
#include "mellos/fd.h"
#include "kernel_stdio.h"
#include "mellos/pipe.h"
#include "processes.h"
#include "shell/shell.h"
#include "spinlock.h"
#include "stddef.h"
#include "syscalls.h"
#include "stdint.h"
#include "errno.h"
#include "string.h"
#include "circular_buffer.h"

int sys_write(regs_t* r) {
	uint32_t LBA = r->ebx;
	char* msg = (char*)(r->ecx);

	uint32_t len = r->edx;

	const process_t* current_process = get_current_process();
	if (current_process == NULL) {
		return -1;
	}
	if (current_process->pid == 0) {
		fd_t* stdout_local = NULL;
		switch (LBA) {
			case 1:
				stdout_local = current_process->stdout->private_data;
				break;
			case 2:
				stdout_local = current_process->stderr->private_data;
				break;
			default:
				break;
		}

		if (stdout_local != NULL) {

			return kprintf("[KERNEL] %s", msg);
		}

		return -EINVAL;
	}

	switch (LBA) {
		case 1:
			if (kstrlen(msg) > len) {
				msg[len - 1] = 0;
			}

			pipe_write(current_process->stdout, msg, len);
			return 0;
		case 2: // stderr
			if (kstrlen(msg) > len) {
				msg[len - 1] = 0;
			}
			pipe_write(current_process->stderr, msg, len);
			return 0;
		default:
			break;
	}

	char* tmp = kmalloc((len / 512 + 1) * 512);

	for (uint32_t i = 0; i < (len / 512 + 1) * 512; i++) {
		if (i < len)
			tmp[i] = msg[i];
		else
			tmp[i] = 0;
	}



	fd_t* filedescriptor = get_file_descriptor(r->ebx);
	if (!process_memory_owns(current_process->page_list, r->ecx, r->ecx + r->edx)) {
		errno = EACCES;
		goto clean;
	}

	if (errno != 0) {
		goto clean;
	}

	if (!(filedescriptor->flags & FD_PERM_WRITE)) {
		errno = EACCES;
		goto clean;
	}

	if (filedescriptor->type == FD_TYPE_PIPE) {
		pipe_t* realpipe = ((pipe_t*)filedescriptor->private_data);
		if (realpipe->flags & O_PIPE_OPEN) {
			ssize_t ret = pipe_write(filedescriptor, (char*)r->ecx, r->edx);
			kfree(tmp);
			return ret;
		}
	} else {

		file_t* realfile = ((file_t*)filedescriptor->private_data);

		if (!realfile) {
			goto clean;
		}

		realfile->ops->write(realfile, (char*)r->ecx, r->edx, realfile->position);
		realfile->position += r->edx;
	}


	old_file_t* files = get_file_list(0xA0, 1, 1);

	for (uint32_t i = 0; i < 32; i++) {
		if (LBA == files[i].LBA) {
			add_filewrite_task(tmp, files[i].name, len);
			return 0;
		}
	}
clean:
	kfree(tmp);
	return -1;
}