
#include "mellos/kernel/streams.h"

#include "mellos/fs.h"
#include "assert.h"

file_t* kstdin;
file_t* kstdout;
file_t* kstderr;

size_t kstream_write(file_t* stream, const char* s, size_t size) {
	kassert_msg(stream != NULL, "Stream is NULL");
	return stream->ops->write(stream, s, size, 0);
}
size_t kstream_read(file_t* stream, char* s, size_t size) {
	kassert_msg(stream != NULL, "Stream is NULL");
	return stream->ops->read(stream, s, size, 0);
}
size_t kstream_flush(file_t* stream) {
	kassert_msg(stream != NULL, "Stream is NULL");
	stream->ops->ioctl(stream, 0, NULL);
	return 0;
}
size_t kstream_close(file_t* stream) {
	kassert_msg(stream != NULL, "Stream is NULL");
	stream->ops->ioctl(stream, 3, NULL);
	return 0;
}