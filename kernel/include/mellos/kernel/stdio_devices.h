#pragma once
#include "mellos/fs.h"
#include "processes.h"

// now defined on processes.h to fix circular dependency
// typedef struct {
// 	const char *name;
// 	int (*read)(void *buf, int size);
// 	int (*write)(void *buf, int size);
// 	int (*ioctl)(int request, void *arg);
// } device_t;

void init_kernel_devices();
void init_stdio_files();

void init_stdio_devices(process_t* proc);

ssize_t stdin_read(file_t* f, void* buf, size_t size, uint64_t offset);

ssize_t stdout_write(file_t* f, const void* buf, size_t size, uint64_t offset);

ssize_t stderr_write(file_t* f, const void* buf, size_t size, uint64_t offset);