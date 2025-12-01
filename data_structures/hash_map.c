#include "hash_map.h"
#include "autoconf.h"
#include "dynamic_mem.h"
#include "mellos/hash.h"
#include "mellos/kernel/kernel_stdio.h"
#include "mem.h"
#include "stddef.h"
#include "stdint.h"
#include "string.h"

hash_map_t* hash_map_create() {
	hash_map_t* map = kzalloc(sizeof(hash_map_t));
	map->capacity = 16;
	map->size = 0;
	map->buckets = kzalloc(map->capacity * sizeof(hash_map_bucket_t*));
	memset(map->buckets, 0, map->capacity * sizeof(hash_map_bucket_t*));

	return map;
}

bool rehash(hash_map_t* map, uint32_t new_capacity) {

	hash_map_bucket_t** old_buckets = map->buckets;
	uint32_t old_capacity = map->capacity;

	void* allocation_result = kzalloc(new_capacity * sizeof(hash_map_bucket_t*));
	if (allocation_result == NULL) {
		// we cannot immediately set map->buckets to this as otherwise if kmalloc
		// returns NULL we would lose the old pointer

		// todo: call oom killer / memory fragmentation fixer and try again
		return false;
	}
	map->buckets = allocation_result;

	for (uint32_t i = 0; i < old_capacity; i++) {
		hash_map_bucket_t* bucket = old_buckets[i];
		while (bucket != NULL) {
			hash_map_bucket_t* next = bucket->next;

			const uint32_t new_index = bucket->hash % new_capacity;

			bucket->next = map->buckets[new_index];
			map->buckets[new_index] = bucket;

			bucket = next;
		}
	}

	kfree(old_buckets);
	return true;
}

float get_load_factor(hash_map_t* map) {
	return (float)map->size / (float)map->capacity;
}

hash_map_bucket_t* create_bucket(const char* key, uint32_t key_len, uint32_t hash) {

	hash_map_bucket_t* next = kzalloc(sizeof(hash_map_bucket_t));

	if (next == NULL) {
		// todo: call oom killer / memory fragmentation fixer and try again
		return NULL;
	}

	next->next = NULL;
	next->value = NULL;
	next->key = (char*)key;
	next->key_length = key_len;
	next->hash = hash;
	return next;
}

bool hash_map_put(hash_map_t* map, const char* key, const void* value) {

	map->size++; // this is here for the following load factor check

	const uint32_t hash = hash_djb2_string(key);

	if (get_load_factor(map) > (float)CONFIG_HASHMAP_LOAD_FACTOR / (float)100) {
		rehash(map, map->capacity * CONFIG_HASHMAP_GROW_MULTIPLIER / 100);
	}

	hash_map_bucket_t* bucket = map->buckets[hash % map->capacity];

	if (bucket == NULL) {
		bucket = create_bucket(key, strlen(key), hash);
	}

	while ((bucket->value != NULL || bucket->key != NULL) &&
	       (strlen(bucket->key) != bucket->key_length &&
	        memcmp(bucket->key, key, strlen(bucket->key)) != 0)) {
		if (bucket->next == NULL) {
			bucket->next = create_bucket(key, strlen(key), hash);
			if (bucket->next == NULL) {
				// was unable to allocate memory
				map->size--;
				return false;
			}
		}
		bucket = bucket->next;
	}

	if (bucket->value == NULL) {
		bucket->hash = hash;
		bucket->value = (void*)value;
		bucket->key_length = strlen(key);
	} else {
		kfprintf(kstderr, "corrupted hash map? bucket: %p", bucket);
	}
	return true;
}

void* hash_map_get(hash_map_t* map, const void* key, size_t key_len) {
	uint32_t hash = hash_djb2(key, key_len);
	hash_map_bucket_t* hmb = map->buckets[hash % map->capacity];

	while (hmb != NULL) {
		if (hmb->key_length != key_len) {
			if (hmb->next == NULL) {
				return NULL;
			} else {
				hmb = hmb->next;
				continue;
			}
		}

		if (memcmp(hmb->key, key, hmb->key_length) != 0) {
			if (hmb->next == NULL) {
				return NULL;
			}
			hmb = hmb->next;
			continue;
		}

		return hmb->value;
	}

	return NULL;
}

void* get_by_string(hash_map_t* map, const char* key) {
	return hash_map_get(map, key, strlen(key));
}
// todo: non-string binary remove
bool hash_map_remove(hash_map_t* map, const char* key) {
	size_t key_len = strlen(key);
	uint32_t hash = hash_djb2(key, strlen(key));
	uint32_t walked = 0;
	hash_map_bucket_t* hmb = map->buckets[hash % map->capacity];
	hash_map_bucket_t* prev = NULL;
	while (hmb) {
		if (key_len == hmb->key_length && memcmp(key, hmb->key, key_len) == 0) {

			if (walked == 0) {
				hmb->key_length = 0;
				hmb->value = NULL;
				hmb->key = NULL;
				map->size--;
				return true;
			}
			if (hmb->next == NULL) {
				kfree(hmb);
				map->size--;
				return true;
			}
			prev->next = hmb->next;
			kfree(hmb);
			map->size--;
			return true;
		}
		walked++;
		prev = hmb;
		hmb = hmb->next;
	}
	return false;
}

void hash_map_clear(hash_map_t* map) {
	for (int i = 0; i < map->capacity; i++) {
		hash_map_bucket_t* bucket = map->buckets[i];
		if (bucket != NULL) {
			// Clear the first bucket but keep it allocated
			bucket->key = NULL;
			bucket->value = NULL;
			bucket->key_length = 0;

			// Free all later buckets in the chain
			hash_map_bucket_t* next = bucket->next;
			while (next != NULL) {
				hash_map_bucket_t* to_free = next;
				next = next->next;
				kfree(to_free);
			}
			bucket->next = NULL;
		}
	}
	map->size = 0;
}

void hash_map_free(hash_map_t* map) {
	for (int i = 0; i < map->capacity; i++) {
		hash_map_bucket_t* bucket = map->buckets[i];
		while (bucket != NULL) {
			hash_map_bucket_t* next = bucket->next;
			kfree(bucket);
			bucket = next;
		}
	}
	kfree(map->buckets);
	kfree(map);
}
