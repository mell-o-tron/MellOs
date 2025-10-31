#pragma once
#include "mellos/fd.h"

open_file_t *open_file(char *path, int type);
int set_file_mode(open_file_t *file, int mode);
open_file_t *get_file(const char* path);
void list_dir(const char* path);
void read_file(const char* path, int size);