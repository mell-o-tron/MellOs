[← Main README](../README.md)

# disk_interface

This directory contains disk interface code for MellOs, allowing to read and write strings directly from the disk.

## diskinterface.c / diskinterface.h
Implements basic disk read/write operations.

- **read_string_from_disk(uint8_t disk, uint32_t LBA, uint32_t num_sectors)**: Reads a string of bytes from the specified disk, starting at the given Logical Block Address (LBA) for a number of sectors. Returns a pointer to the data.
- **write_string_to_disk(char* str, uint8_t disk, uint32_t LBA, uint32_t num_sectors)**: Writes a string of bytes to the specified disk at the given LBA for a number of sectors.
