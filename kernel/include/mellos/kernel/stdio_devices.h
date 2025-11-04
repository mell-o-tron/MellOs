#pragma once
#include "processes.h"

// now defined on processes.h to fix circular dependency
// typedef struct {
// 	const char *name;
// 	int (*read)(void *buf, int size);
// 	int (*write)(void *buf, int size);
// 	int (*ioctl)(int request, void *arg);
// } device_t;

typedef struct _FILE {
  spinlock_t lock;
  int fd;        // Which stream (0=stdin, 1=stdout, 2=stderr)
  void* device;  // Optional device pointer
} FILE;

void init_stdio_devices(process_t *proc);

int stdin_read(void* buf, int size);

int stdout_write(void* buf, int size);

int stderr_write(void* buf, int size);

extern device_t stdin_device;
extern device_t stdout_device;
extern device_t stderr_device;