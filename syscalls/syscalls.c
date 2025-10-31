#include "autoconf.h"
#include "errno.h"
#include <processes.h>

#include "cpu/idt.h"
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "dynamic_mem.h"
#include "file_system.h"
#include "shell/shell.h"
#include "string.h"
#include "syscalls.h"

int syscall_stub(regs* r) {
	switch (r->eax) {
	case 1:
		// sys_exit
		return sys_exit(r);

	case 2:
		// sys_fork
		return sys_fork(r);
	case 3:
		// sys_read
		return sys_read(r);
	case 4:
		// sys_write
		return sys_write(r);
	case 5:
		// sys_open
		return sys_open(r);
	case 6:
		// sys_close
		return sys_close(r);
	case 7:
		return get_pid(r);
	case 8:
		return sys_memory(r);
	}

	return 0;
}

int sys_exit(regs* r) {
	errno = ENOSYS;
	kprint("sys_exit\n");
	return -1;
}

int sys_fork(regs* r) {
	kprint("sys_fork\n");
	return -1;
}

int sys_read(regs* r) {
	kprint("sys_read\n");
	return -1;
}

int sys_write(regs* r) {
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
			stdout_local = current_process->stdout->resource;
			break;
		case 2:
			stdout_local = current_process->stderr->resource;
			break;
		default:
			break;
		}

		if (stdout_local != NULL) {
			char buf[128];

			while (true) {
				const int ret = pipe_read(stdout_local, buf, 127);
				if (ret < 0) {
					errno = ret;
					return -1;
				}
				if (ret == 0) {
					return 0;
				}
				const int written = pipe_write(current_process->parent->stdin, buf, ret);

				if (written < 0) {
					errno = written;
					return -1;
				}
				if (written != ret) {
					return -1;
				}

				// kprint(buf);
			}
		}

		return -EINVAL;
	}

	// If the file descriptor is 1, we write to "stdout"
	// Which for now does not exist lol, we just print.
	switch (LBA) {
	case 1: // stdout
		if (strlen(msg) > len)
			msg[len - 1] = 0;
		// For now, write to the screen directly regardless of process context
		kprint(msg);
		return 0;
	case 2: // stderr
		if (strlen(msg) > len)
			msg[len - 1] = 0;
		print_error(msg);
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

	file_t* files = get_file_list(0xA0, 1, 1);

	for (uint32_t i = 0; i < 32; i++) {
		if (LBA == files[i].LBA) {
			add_filewrite_task(tmp, files[i].name, len);
			return 0;
		}
	}

	return -1;
}

int sys_open(regs* r) {
	kprint("sys_open\n");
	return -1;
}

int sys_close(regs* r) {
	kprint("sys_close\n");
	return -1;
}

int sys_memory(regs* r) {
	switch (r->ebx) {
	case 0: // allocate
		kmalloc(r->ecx);
	// TODO: allocate memory directly from the page manager
	case 1: // free
		kfree((void*)r->ecx);
		break;
	default:
		return -1;
	}
	return 0;
}

int get_pid(regs* r) {
	const process_t* proc = get_current_process();
	if (proc == NULL) {
		return -1;
	}
	return (int)proc->pid;
}