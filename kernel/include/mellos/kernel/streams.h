#pragma once
#include "spinlock.h"
#include "stdint.h"

typedef struct _FILE FILE; // NOLINT(*-reserved-identifier)

typedef struct {
	/**
	 * @brief Function pointer for writing to a stream.
	 *
	 * Implements the `write` operation of a `stream_ops_t` object. Handles writing
	 * data from a buffer to the specified `FILE` stream.
	 *
	 * @param stream Target `FILE` stream to write to.
	 * @param s      Pointer to the buffer containing the data to be written.
	 * @param size   Number of bytes to write from the buffer.
	 * @return Number of bytes successfully written.
	 */
	size_t (*write)(FILE* stream, const char* s, size_t size);
	/**
	 * @brief Function pointer for reading data from a stream.
	 *
	 * Defines a signature for functions handling read operations on a `FILE` stream.
	 * This function is responsible for populating a buffer with up to `size` bytes
	 * of data read from the specified `stream`.
	 *
	 * @param stream Pointer to the `FILE` instance from which data is read.
	 * @param s      Pointer to the buffer where the read data should be stored.
	 * @param size   Number of bytes requested to read into the buffer.
	 * @return       Number of bytes successfully read, which may be less than `size`
	 *               depending on availability, or zero to indicate end-of-stream.
	 */
	size_t (*read)(FILE* stream, char* s, size_t size);
	/**
	 * @brief Pointer to a function handling stream buffer flushing.
	 *
	 * Ensures that any buffered output data for the `FILE` stream is written to the
	 * underlying file or device. May also facilitate internal state updates or resource
	 * synchronization related to the stream.
	 *
	 * @param stream Pointer to the `FILE` stream to flush.
	 * @return The number of bytes successfully flushed, or implementation-defined behavior
	 * in case of an error.
	 */
	size_t (*flush)(FILE* stream);
	/**
	 * @brief Function pointer for closing a file stream.
	 *
	 * Represents a customizable operation to release resources associated with a `FILE` stream.
	 *
	 * @param stream Pointer to the `FILE` stream to close.
	 * @return 0 on success, or an error code.
	 */
	size_t (*close)(FILE* stream);
} stream_ops_t;

/**
 * @struct _FILE
 * @brief I/O stream abstraction.
 *
 * Links `stream_ops_t` operations with a file descriptor and optional device.
 * Uses `rwlock_t` for thread-safe concurrent access.
 *
 * @typedef FILE
 * @field ops    Polymorphic operations (read, write, flush, close).
 * @field lock   Synchronization primitive for thread-safety.
 * @field fd     File descriptor (0=stdin, 1=stdout, 2=stderr, etc.).
 * @field device Optional pointer to low-level `file_t` device/source.
 */
typedef struct _FILE { // NOLINT(*-reserved-identifier)
	stream_ops_t* ops;
	rwlock_t lock;
	int fd;       // Which stream (0=stdin, 1=stdout, 2=stderr)
	void* device; // Optional device pointer
} FILE;

/**
 * @brief Writes data to a stream.
 * @param stream Target `FILE` stream.
 * @param s      Source buffer.
 * @param size   Bytes to write.
 * @return Number of bytes written.
 */
size_t kstream_write(FILE* stream, const char* s, size_t size);

/**
 * @brief Reads data from a stream.
 * @param stream Source `FILE` stream.
 * @param s      Destination buffer.
 * @param size   Bytes to read.
 * @return Number of bytes read.
 */
size_t kstream_read(FILE* stream, char* s, size_t size);

/**
 * @brief Flushes a stream's buffer.
 * @param stream `FILE` stream to flush.
 * @return Number of bytes flushed.
 */
size_t kstream_flush(FILE* stream);

/**
 * @brief Closes a stream.
 * @param stream `FILE` stream to close.
 * @return 0 on success, or an error code.
 */
size_t kstream_close(FILE* stream);