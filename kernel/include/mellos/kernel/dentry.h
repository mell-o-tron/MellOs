#pragma once
#include "mellos/fs.h"

dentry_t* get_or_create_dentry_unsafe(char* name);
dentry_t* get_or_create_dentry(char* name);
void dentry_manager_init(void);