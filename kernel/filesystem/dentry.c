#include "mellos/kernel/dentry.h"
#include "dynamic_mem.h"
#include "linked_list.h"
#include "mellos/fs.h"
#include "mellos/kernel/kernel.h"
#include "mellos/kernel/kernel_stdio.h"
#include "spinlock.h"
#include "stddef.h"
#include "string.h"
#include "hash_map.h"

hash_map_t* dentry_map;
volatile int32_t dentry_lock = 0;

#define FS_ROOT "/"

void dentry_manager_init() {
	kprintf("Initializing dentries...");
	dentry_map = hash_map_create();
}

void destroy_dentry(dentry_t* dentry) {
	kfree(dentry->name);
	kfree(dentry);
}

bool free_dentry(dentry_t* dentry) {
	dentry->inode->dentry = NULL;
	if (!hash_map_remove(dentry_map, dentry->name)) {
		return false;
	}
	destroy_dentry(dentry);
	return true;
}

dentry_t* create_dentry(char* name) {
	dentry_t* de = kmalloc(sizeof(dentry_t));

	de->name = strdup(name);
	de->refcount = 1;
	de->inode = get_inode_from_path(de->name);
	char* tmp_path = strdup(de->name);
	if (drop_after_last('/', tmp_path, false) == NULL) {
		kfree(tmp_path);
		kfprintf(kstderr, "Something went very wrong during string formatting!!!");
		return NULL;
	}
	if (strcmp(tmp_path, FS_ROOT) != 0) {
		de->parent = get_or_create_dentry_unsafe(tmp_path);
	}
	return de;
}

dentry_t* get_or_create_dentry_unsafe(char* name) {

	if (dentry_map == NULL) {
		kfprintf(kstderr, "dentry_list not initialized, cannot get dentry\n");
		asm("hlt");
	}

	dentry_t* de = get_by_string(dentry_map, name);
	if (de == NULL) {
		de = create_dentry(name);
		hash_map_put(dentry_map, name, de);
	}
	return de;
}

dentry_t* get_or_create_dentry(char* name) {
	SpinLock(&dentry_lock);
	dentry_t* ent = get_or_create_dentry_unsafe(name);
	SpinUnlock(&dentry_lock);
	return ent;
}

void dentry_update() {

	hash_map_bucket_t** bucket = dentry_map->buckets;
	size_t capacity = dentry_map->capacity;
	for (size_t i = 0; i < capacity; i++) {

		hash_map_bucket_t* current_bucket_list = bucket[i];
		size_t bucket_list_i = 0;
		do {
			hash_map_bucket_t current_bucket = current_bucket_list[bucket_list_i];
			if (current_bucket.key == NULL) {
				bucket_list_i++;
				if (current_bucket_list->next != NULL) {
					continue;
				}
				break;
			}

			dentry_t* de = current_bucket.value;
			// we dont want to free root as it probably gets accessed quite a lot
			// todo: Recursively search the children for ones with refcount = 0 if
			//  the parent's refcount = child count. This is an expensive operation
			//  but it can be done lazily over time.
			if (de->refcount == 0 && strcmp(de->name, "/") != 0) {
				if (!free_dentry(de)) {
					kfprintf(kstderr, "Unable to free dentry!");
				}
			}

			bucket_list_i++;
		} while (current_bucket_list->next != NULL);
	}
}

int dentry_init(dentry_t* dentry, inode_t* inode) {
	inode->dentry = dentry;
	dentry->refcount = 1;
	return 0;
}

int dentry_delete(dentry_t* dentry) {
	return free_dentry(dentry);
}

dentry_t* dentry_alloc(dentry_t* parent, char* name) {
	if (parent == NULL) {
		// root dentry
	} else {
		parent->refcount++;
	}

	dentry_t* de = kmalloc(sizeof(dentry_t));

	de->name = strdup(name);

	return de;
}
