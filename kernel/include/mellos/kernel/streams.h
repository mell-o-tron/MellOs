#pragma once
#include "spinlock.h"
#include "stdint.h"

typedef struct _FILE FILE; // NOLINT(*-reserved-identifier)

typedef struct {
	size_t (*write)(FILE* stream, const char* s, size_t size);
	size_t (*read)(FILE* stream, char* s, size_t size);
	size_t (*flush)(FILE* stream);
	size_t (*close)(FILE* stream);
} stream_ops_t;

typedef struct _FILE { // NOLINT(*-reserved-identifier)
	stream_ops_t* ops;
	spinlock_t lock;
	int fd;       // Which stream (0=stdin, 1=stdout, 2=stderr)
	void* device; // Optional device pointer
} FILE;

size_t kstream_write(FILE* stream, const char* s, size_t size);
size_t kstream_read(FILE* stream, char* s, size_t size);
size_t kstream_flush(FILE* stream);
size_t kstream_close(FILE* stream);