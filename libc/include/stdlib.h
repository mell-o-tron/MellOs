#pragma once
#include "stdint.h"

_Noreturn void exit(int status);
_Noreturn void abort(void);
void *malloc(size_t size);
void free(void *ptr);
void realloc(void *ptr, size_t size);