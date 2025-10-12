[← Shell README](../README.md) | [← Main README](../../README.md)

# shell functions

This directory contains additional shell helper functions for MellOs, used by the shell for command parsing, execution, and system interaction.

## Conventions
The macro `FDEF(name)` is used to define shell command functions as `void name(const char\* s)`.

## Notable files and functions
- **displayinfo.c**: Functions for displaying system information.
- **text_editor.c**: Functions for launching and interacting with the text editor from the shell.
- **meminfo.c**: Functions for displaying memory information.
- **exec.c**: Functions for executing binaries from the shell (:shit: flat binaries loaded to a fixed location, bad, to be changed :shit:).
- **disk.c**: Functions for disk operations (read/write, info, etc.).
- **file_sys.c**: Functions for file system operations (create, remove, write, read files).
- **vell.c**: Initializes the virtual shell.