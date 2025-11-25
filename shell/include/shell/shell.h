#pragma once
#include "stdint.h"

#include "mellos/fs.h"

void load_shell(void);
void refreshShell(void);
void parseCommand(void);
void add_filewrite_task(char* str, char* filename, uint32_t len);
inode_t* get_working_dir(void);
void set_working_dir(inode_t* dir);
