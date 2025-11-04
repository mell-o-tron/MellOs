#include "mellos/kernel/stdio_devices.h"
#include "autoconf.h"
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "colours.h"

//todo: the keyboard input driver that the shell polls should be passed to this
int stdin_read(void* buf, int size) {
	return 0;
}

int stdout_write(void* buf, int size) {
	if (!buf || size <= 0) return 0;

	char buffer[size + 1];
	for (int i = 0; i < size; i++) {
		buffer[i] = ((char*)buf)[i];
	}
	buffer[size] = '\0';

	kprint_col(buffer, DEFAULT_COLOUR);
	return size;
}

int stderr_write(void* buf, int size) {
	if (!buf || size <= 0) return 0;

	char buffer[size + 1];
	for (int i = 0; i < size; i++) {
		buffer[i] = ((char*)buf)[i];
	}
	buffer[size] = '\0';

	kprint_col(buffer, STDERR_COLOUR);
	return size;
}

device_t stdin_device = {
	.name = "stdin",
	.read = stdin_read,
	.write = NULL,
	.ioctl = NULL,
};

device_t stdout_device = {
	.name = "stdout",
	.read = NULL,
	.write = stdout_write,
	.ioctl = NULL,
};

device_t stderr_device = {
	.name = "stderr",
	.read = NULL,
	.write = stderr_write,
	.ioctl = NULL,
};

void init_stdio_devices(process_t *proc) {
	if (!proc) return;

	// Bind default stdio devices to the process
	proc->stdin_device = &stdin_device;
	proc->stdout_device = &stdout_device;
	proc->stderr_device = &stderr_device;
}

FILE _stdin = { .fd = 0, .device = NULL};
FILE _stdout = { .fd = 1, .device = NULL};
FILE _stderr = { .fd = 2, .device = NULL};

FILE* stdin = &_stdin;
FILE* stdout = &_stdout;
FILE* stderr = &_stderr;