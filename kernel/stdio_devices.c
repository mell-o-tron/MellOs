#include "mellos/kernel/stdio_devices.h"
#include "mellos/kernel/streams.h"
#include "mellos/kernel/kernel_stdio.h"
#include "autoconf.h"
#include "dynamic_mem.h"

#include "errno.h"
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "colours.h"

file_t* kernel_stdin_device;
file_t* kernel_stdout_device;
file_t* kernel_stderr_device;

extern FILE* kstdin;
extern FILE* kstdout;
extern FILE* kstderr;

//todo: the keyboard input driver that the shell polls should be passed to this
ssize_t stdin_read(file_t* f, void* buf, size_t size, uint64_t offset) {
	return kstdin->ops->read(kstdin, buf, size);
}

ssize_t stdout_write(file_t* f, const void* buf, size_t size, uint64_t offset) {
	if (!buf || size <= 0) return 0;
	if (f != kernel_stdout_device) {
		return -EINVAL;
	}

	char buffer[size + 1];
	buffer[size] = 0;
	for (int i = 0; i < size; i++) {
		buffer[i] = ((char*)buf)[i];
	}

	kprint_col(buffer, DEFAULT_COLOUR);
	return size;
}

ssize_t stderr_write(file_t* f, const void* buf, size_t size, uint64_t offset) {
	if (!buf || size <= 0) return 0;
	if (f != kernel_stderr_device) {
		return -EINVAL;
	}

	char buffer[size + 1];
	for (int i = 0; i < size; i++) {
		buffer[i] = ((char*)buf)[i];
	}

	kprint_col(buffer, ERROR_COLOUR);
	return size;
}

size_t file_read(FILE* stream, char* buf, size_t size) {
	file_t* f = stream->device;
	return f->ops->read(f, buf, size, 0);
}

size_t file_write(FILE* stream, const char* buf, size_t size) {
	file_t* f = stream->device;
	return f->ops->write(f, buf, size, 0);
}

file_ops stdout_fops = {
	.read = NULL,
	.write = stdout_write,
};

file_ops stderr_fops = {
	.read = NULL,
	.write = stderr_write,
};

file_ops stdin_fops = {
	.read = stdin_read,
	.write = NULL,
};

stream_ops_t kernel_stdout_streamops = {
	.write = kstream_write,
};

stream_ops_t kernel_stderr_streamops = {
	.write = kstream_write,
};

stream_ops_t kernel_stdin_streamops = {
	.read = kstream_read,
};

void init_kernel_devices() {
	kernel_stdout_device = kmalloc(sizeof(file_t));
	kernel_stdin_device = kmalloc(sizeof(file_t));
	kernel_stderr_device = kmalloc(sizeof(file_t));


	kernel_stdin_device->ops = &stdin_fops;
	kernel_stdout_device->ops = &stdout_fops;
	kernel_stderr_device->ops = &stderr_fops;
}

void init_stdio_devices(process_t *proc) {
	if (!proc) return;

	// Bind default stdio devices to the process
	proc->stdin_device = kernel_stdin_device;
	proc->stdout_device = kernel_stdout_device;
	proc->stderr_device = kernel_stderr_device;
}



void init_stdio_files() {
	kstdin = kmalloc(sizeof(FILE));
	kstdout = kmalloc(sizeof(FILE));
	kstderr = kmalloc(sizeof(FILE));
	kstdin->device = kernel_stdin_device;
	kstdout->device = kernel_stdout_device;
	kstderr->device = kernel_stderr_device;

	kstdout->ops = &kernel_stdout_streamops;
	kstderr->ops = &kernel_stderr_streamops;
	kstdin->ops = &kernel_stdin_streamops;
}
