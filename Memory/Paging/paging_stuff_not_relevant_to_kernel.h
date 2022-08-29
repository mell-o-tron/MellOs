#pragma once

extern void loadPageDirectory(unsigned int*);
extern void enablePaging();

unsigned int page_directory[1024] __attribute__((aligned(4096)));
unsigned int first_page_table[1024] __attribute__((aligned(4096)));