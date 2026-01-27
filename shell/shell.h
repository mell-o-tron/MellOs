#pragma once
#include "../utils/typedefs.h"

void load_shell();
void refreshShell();
void parseCommand();
void eraseCurrentCommand();
void typeCommand(char* command, uint32_t length);
void saveCommand(char* command, uint32_t length);
void restoreCommand(uint8_t index);
void handleHistoryKeys(char action);
void add_filewrite_task(char* str, char* filename, uint32_t len);
