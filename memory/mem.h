#pragma once

#include "../utils/typedefs.h"

void* memset(void* dest, unsigned char val, size_t count);


void memcp(unsigned char* restrict source, unsigned char* restrict dest, size_t count);
void *memcpy(void * restrict to, const void * restrict from, unsigned int n);