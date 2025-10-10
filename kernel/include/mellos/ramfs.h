#pragma once
#include "mellos/fd.h"

fd_t *get_file(const char* path);
void list_dir(const char* path);
void read_file(const char* path, int size);