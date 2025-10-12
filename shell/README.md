[← Main README](../README.md)

# shell

This directory contains shell and command interface code for MellOs, including command parsing and shell task management.

## shell.h / shell.c
- **load_shell()**: Loads and initializes the shell.
- **refreshShell()**: Refreshes the shell display.
- **parseCommand()**: Parses and executes a command.
- **add_filewrite_task(char\* str, char\* filename, uint32_t len)**: Adds a file-write task to the shell task queue.


## Subdirectories
- [Shell Functions](functions/README.md)
