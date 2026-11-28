#include "stdlib.h"
#include "stddef.h"

#include "unistd.h"

#include "errno.h"

typedef struct {
	uint32_t size;
	void* data;
} memory_block_t;

// todo: allocator

void* grow_memory_block(memory_block_t* block, uint32_t size) {

	const long ret = syscall2(SYS_MMAP, SYS_MMAP_MAP, (long)size);
	if (ret < 0) {
		errno = -ret;
		return NULL;
	}

	block->data = (void*)ret;

	block->size = size;
	return block->data;
}

void* malloc(size_t size){
    return NULL;
}

void free(void* ptr){

}

