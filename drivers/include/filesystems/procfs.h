#pragma once
#include "mellos/kernel/kernel.h"
#include "mellos/fs.h"

#define PROCFS_NAME "procfs"
#define PROCFS_IDENTIFIER 0xafafbf21

typedef struct {
  dentry_t* parent_dentry;
} procfs_mount_data_t;


fs_type_t* procfs_getfs();
super_ops_t* procfs_get_file_ops();