#pragma once
#include "stdint.h"

void load_shell();
void refreshShell();
void parseCommand();
void add_filewrite_task(char* str, char* filename, uint32_t len);
