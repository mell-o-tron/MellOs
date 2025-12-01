#pragma once
#include "stdint.h"

typedef struct hash_map_bucket {
	uint32_t hash;
	char* key;
	size_t key_length;
	void* value;
	// for hash collisions we have a linked list
	struct hash_map_bucket* next;
} hash_map_bucket_t;

typedef struct hash_map {
  /* buckets */
	uint32_t capacity;
  /* entries */
	uint32_t size;
	hash_map_bucket_t** buckets;
} hash_map_t;

hash_map_t* hash_map_create();
_Bool hash_map_put(hash_map_t* map, const char* key, const void* value);
void* hash_map_get(hash_map_t* map, const void* key, size_t key_len);
void* get_by_string(hash_map_t* map, const char* key);
_Bool hash_map_remove(hash_map_t* map, const char* key);
void hash_map_clear(hash_map_t* map);
void hash_map_free(hash_map_t* map);