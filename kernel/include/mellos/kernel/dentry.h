#pragma once
#include "mellos/fs.h"

dentry_t* get_or_create_dentry_unsafe(char* name);
dentry_t* get_or_create_dentry(char* name);
void dentry_manager_init(void);
dentry_t* dentry_alloc(dentry_t* parent, char* name);
int dentry_init(dentry_t* dentry, inode_t* inode, char* path);
void dentry_manager_add(char* path, dentry_t* entry);