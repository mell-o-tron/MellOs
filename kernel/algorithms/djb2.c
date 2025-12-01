#include "mellos/hash.h"

uint32_t hash_djb2(const void* target, size_t length) {
	const unsigned char* p = target;
	uint32_t hash = 5381;

	for (size_t i = 0; i < length; i++)
		hash = ((hash << 5) + hash) ^ p[i]; // hash * 33 ^ p[i]

	return hash;
}

uint32_t hash_djb2_string(const char* string) {
	uint32_t hash = 5381;
	while (*string) {
		hash = ((hash << 5) + hash) ^ (unsigned char)(*string++);
	}

	return hash;
}