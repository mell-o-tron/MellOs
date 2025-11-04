#include "autoconf.h"
#include "errno.h"
#include "processes.h"

#include "mellos/kernel/kernel_stdio.h"

#include "cpu/idt.h"

#include "stdio.h"

#include <colours.h>
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
#include "paging/paging.h"
#include "process_memory.h"
#include "memory_area_spec.h"

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
		return sys_mmap(r);
	case 9:
		return sys_munmap(r);
	case 10:
		return sys_mprotect(r);
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


			const int written = kprintf("[KERNEL] %s", msg);

			if (written < 0) {
				return written;
			}
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

int sys_mmap(regs* r) {
	// ebx: subop (0=alloc, 1=free)
	// ecx: size (bytes) for alloc, or base address for free
	// edx: unused for alloc (can be hint/flags), or size for free (optional)
	process_t* proc = get_current_process();
	if (!proc) return -1;
	uint32_t subop = r->ebx;
	if (subop == 0) {
		// allocate N bytes for this process without using kernel allocator
		size_t size = (size_t)r->ecx;
		if (size == 0) return 0;
		size_t pages = (size + (PAGE_SIZE - 1)) / PAGE_SIZE;
		uintptr_t base = allocate_user_pages_return_base(proc->pid, pages, 0);
		if (base == 0) return 0; // failure -> return 0 like mmap
		// store region
		process_memory_add_region(proc->page_list, base, pages);
		return (int)base;
	}
	if (subop == 1) {
		// free region; if size provided in edx==0 then look up region by base
		uintptr_t base = (uintptr_t)r->ecx;
		size_t pages = 0;
		if (r->edx) {
			pages = ((size_t)r->edx + (PAGE_SIZE - 1)) / PAGE_SIZE;
		} else {
			if (!process_memory_find_region(proc->page_list, base, &pages)) {
				return -1;
			}
		}
		if (pages == 0)
			return -1;
		if (!free_user_pages(proc->pid, base, pages))
			return -1;
		process_memory_remove_region(proc->page_list, base);
		return 0;
	}
	return -1;
}

int sys_munmap(regs* r) {
	// compatibility wrapper: munmap(base, size)
	process_t* proc = get_current_process();
	if (!proc) return -1;
	uintptr_t base = (uintptr_t)r->ebx;
	size_t size = (size_t)r->ecx;
	size_t pages = (size + (PAGE_SIZE - 1)) / PAGE_SIZE;
	if (pages == 0) return -1;
	if (!free_user_pages(proc->pid, base, pages)) return -1;
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