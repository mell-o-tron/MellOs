#include "mellos/kernel/dentry.h"
#include "assert.h"
#include "dynamic_mem.h"
#include "errno.h"
#include "mellos/fs.h"
#include "spinlock.h"
#include "stddef.h"
#include "string.h"
#include "hash_map.h"
#include "kernel_stdio.h"

hash_map_t* dentry_map;
volatile int32_t dentry_lock = 0;

#define FS_ROOT "/"

void dentry_manager_init() {
	kprintf("Initializing dentries...\n");
	dentry_map = hash_map_create();
}

void destroy_dentry(dentry_t* dentry) {
	kfree(dentry->name);
	kfree(dentry);
}

void dentry_manager_add(char* path, dentry_t* entry) {
	kassert(path);
	kassert(entry);
	SpinLock(&dentry_lock);
	hash_map_put_string(dentry_map, path, entry);
	SpinUnlock(&dentry_lock);
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
	kprintf("creating dentry: %s\n", name);
	dentry_t* de = kmalloc(sizeof(dentry_t));

	de->name = kstrdup(name);
	de->refcount = 1;
	de->inode = get_inode_from_path(name);

	//if (kstrcmp(name, "/proc") == 0) asm("hlt");

	kassert(de->inode != NULL);
	char* tmp_path = kstrdup(de->name);

	if (drop_after_last('/', tmp_path, false) == NULL) {
		kfree(tmp_path);
		kprintf("Something went very wrong during string formatting!!!");
		return NULL;
	}
	if (kstrcmp(tmp_path, FS_ROOT) != 0) {
		de->parent = get_or_create_dentry_unsafe(tmp_path);
	}
	return de;
}

dentry_t* get_or_create_dentry_unsafe(char* name) {

	if (dentry_map == NULL) {
		kprintf("dentry_list not initialized, cannot get dentry\n");
		asm("hlt");
	}

	dentry_t* de = get_by_string(dentry_map, name);

	kprintf("%s\n", name);
	if (de == NULL) {
		de = create_dentry(name);
		hash_map_put_string(dentry_map, name, de);
	}
	kassert(de != NULL);
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
			if (de->refcount == 0 && kstrcmp(de->name, "/") != 0) {
				if (!free_dentry(de)) {
					kfprintf(kstderr, "Unable to free dentry!");
				}
			}

			bucket_list_i++;
		} while (current_bucket_list->next != NULL);
	}
}

int dentry_init(dentry_t* dentry, inode_t* inode, char* path) {
	inode->dentry = dentry;
	dentry->refcount = 1;
	dentry->name = path;
	if (!dentry->name) {
		errno = ENOMEM;
		return -1;
	}
	hash_map_put_string(dentry_map, path, dentry);
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
	kassert(de != NULL);

	de->name = kstrdup(name);
	return de;
}
