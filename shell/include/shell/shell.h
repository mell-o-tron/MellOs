#pragma once
#include "stdint.h"

#include "mellos/fs.h"

void load_shell(void);
void refresh_shell(void);
void parseCommand(void);
void add_filewrite_task(char* str, char* filename, uint32_t len);
inode_t* get_working_dir(void);
void set_working_dir(inode_t* dir);
void load_shell();
void refreshShell();
void parseCommand();
void eraseCurrentCommand();
void typeCommand(char* command, uint32_t length);
void saveCommand(char* command, uint32_t length);
void restoreCommand(uint8_t index);
void handleHistoryKeys(char action);
void add_filewrite_task(char* str, char* filename, uint32_t len);
