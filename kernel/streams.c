
#include "mellos/kernel/streams.h"

#include "mellos/fs.h"

FILE* k_stdin;
FILE* k_stdout;
FILE* k_stderr;

size_t kstream_write(FILE* stream, const char* s, size_t size) {
	if (!stream) {
		return 0;
	}
	file_t* f = stream->device;
	return f->ops->write(f, s, size, 0);
}
size_t kstream_read(FILE* stream, char* s, size_t size) {
	if (!stream) {
		return 0;
	}
	file_t* f = stream->device;
	return f->ops->read(f, s, size, 0);
}
size_t kstream_flush(FILE* stream) {
	if (!stream) {
		return 0;
	}
	file_t* f = stream->device;
	f->ops->ioctl(f, 0, 0);
	return 0;
}
size_t kstream_close(FILE* stream) {
	if (!stream) {
		return 0;
	}
	return 0;
}