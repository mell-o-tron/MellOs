[← Main README](../README.md)

# file_system

This directory contains :warning: PLACEHOLDER :warning: file system code for MellOs, including file allocation, deallocation, and directory management. 

Currently it simply stores the file descriptors in a fixed-length region in the first sectors of the disk. This is :shit: shite :shit: Eventually we will provide ext2 support.


## file_system.c / file_system.h
- **prepare_disk_for_fs(uint32_t n_sectors)**: Clears the file list in disk.
- **allocate_file(uint32_t req_sectors)**: Allocates space for a file of the requested size.
- **deallocate_file(uint32_t LBA, uint32_t num_sectors)**: Frees space for a file at the given LBA.
- **get_file_list(uint32_t disk, uint32_t LBA, uint32_t sectors)**: Retrieves a list of files from disk.
- **write_file_list(file_t \*list, uint32_t disk, uint32_t LBA, uint32_t sectors)**: Writes a file list to disk.
- **new_file(char\* name, uint32_t n_sectors)**: Creates a new file with the given name and size.
- **remove_file(char\* name)**: Removes a file by name.
