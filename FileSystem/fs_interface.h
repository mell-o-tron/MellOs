#pragma once

maybe_void change_directory (const file_mmd* file);
void list_files_debug (file_mmd ** mmd, int number); 
maybe_void make_directory (uint8_t disk, char* name, bitmap_t disk_bitmap, int bitmap_size);
