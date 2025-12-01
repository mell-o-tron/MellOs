#include "autoconf.h"
#include "errno.h"
#include "processes.h"

#include "mellos/kernel/kernel_stdio.h"

#include "cpu/idt.h"

#include "stdio.h"

#include "unistd.h"

#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "dynamic_mem.h"
#include "file_system.h"
#include "memory_area_spec.h"
#include "paging/paging.h"
#include "process_memory.h"
#include "shell/shell.h"
#include "string.h"
#include "syscalls.h"

int syscall_stub(regs* r) {
	switch (r->eax) {
	case SYS_EXIT:
		// sys_exit
		return sys_exit(r);

	case SYS_FORK:
		// sys_fork
		return sys_fork(r);
	case SYS_READ:
		// sys_read
		return sys_read(r);
	case SYS_WRITE:
		// sys_write
		return sys_write(r);
	case SYS_OPEN:
		// sys_open
		return sys_open(r);
	case SYS_CLOSE:
		// sys_close
		return sys_close(r);
	case SYS_GETPID:
		return get_pid(r);
	case SYS_MMAP:
		return sys_mmap(r);
	case SYS_MUNMAP:
		return sys_munmap(r);
	case SYS_MPROTECT:
		return sys_mprotect(r);
	default:
		return -ENOSYS;
	}
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
		if (strlen(msg) > len) {
			msg[len - 1] = 0;
		}

		pipe_write(current_process->stdout, msg, len);
		return 0;
	case 2: // stderr
		if (strlen(msg) > len) {
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

	old_file_t* files = get_file_list(0xA0, 1, 1);

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

// todo: add proper error codes from errno
int sys_mmap(regs* r) {
	// ebx: subop (0=alloc, 1=free)
	// ecx: size (bytes) for alloc, or base address for free
	// edx: unused for alloc (can be hint/flags), or size for free (optional)
	process_t* proc = get_current_process();
	if (!proc) {
		return -1;
	}
	uint32_t subop = r->ebx;
	if (subop == 0) {
		// allocate N bytes for this process without using kernel allocator
		const size_t size = r->ecx;
		if (size == 0) {
			return 0;
		}
		const size_t pages = (size + (PAGE_SIZE - 1)) / PAGE_SIZE;
		const uintptr_t ret = allocate_user_pages_return_base(proc->pid, pages, 0);
		if (ret == 0) {
			return 0; // failure -> return 0 like mmap
		}
		// store region
		process_memory_add_region(proc->page_list, ret, pages);
		return (int)ret;
	}
	if (subop == 1) {
		// free region; if size provided in edx==0 then look up region by base
		uintptr_t base = (uintptr_t)r->ecx;
		size_t pages = 0;
		if (r->edx) {
			pages = ((size_t)r->edx + (PAGE_SIZE - 1)) / PAGE_SIZE;
		} else {
			if (!process_memory_find_region(proc->page_list, base, &pages)) {
				// todo: properly signal the process that it is trying to access invalid memory
				return -1;
			}
		}
		if (pages == 0) {
			return -1;
		}
		if (!free_user_pages(proc->pid, base, pages)) {
			return -1;
		}
		process_memory_remove_region(proc->page_list, base);
		return 0;
	}
	return -1;
}

int sys_munmap(regs* r) {
	// compatibility wrapper: munmap(base, size)
	process_t* proc = get_current_process();
	if (!proc)
		return -1;
	uintptr_t base = (uintptr_t)r->ebx;
	size_t size = (size_t)r->ecx;
	size_t pages = (size + (PAGE_SIZE - 1)) / PAGE_SIZE;
	if (pages == 0)
		return -1;
	if (!free_user_pages(proc->pid, base, pages))
		return -1;
	process_memory_remove_region(proc->page_list, base);
	return 0;
}

int sys_mprotect(regs* r) {
	// Not yet implemented: just return success for now
	return 0;
}

int get_pid(regs* r) {
	const process_t* proc = get_current_process();
	if (proc == NULL) {
		return -1;
	}
	return (int)proc->pid;
}