[← Shell README](../README.md) | [← Main README](../../README.md)

# shell functions

This directory contains additional shell helper functions for MellOs, used by the shell for command parsing, execution, and system interaction.

## Conventions
The macro `FDEF(name)` is used to define shell command functions as `void name(const char\* s)`.

## Adding a command
First, add a `FHDR(name)` (not `FDEF`) to `shell/functions/functions.h` where `name` is the name of the command to declare the command. Then add the command's syntax to the file `shell/shell_functions.c`. Now you can create a new c file or add the command to an already existing file with this:

``` c
// for kprintf
#include "kernel_stdio.h"
// for FDEF
#include "fdef.h"

FDEF(name) {
    kfprintf("Hello, world! Here is a string passed to the command: %s\n", s);
}
```
If you for any reason change the working directory, it should be done using the function `set_working_dir(inode_t\*)`. After that, call `refresh_shell()` which waits for the directory update and then updates the input to display the new working directory. Both of these are in `shell/shell.h`.

## Notable files and functions
- **displayinfo.c**: Functions for displaying system information.
- **text_editor.c**: Functions for launching and interacting with the text editor from the shell.
- **meminfo.c**: Functions for displaying memory information.
- **exec.c**: Functions for executing binaries from the shell (:shit: flat binaries loaded to a fixed location, bad, to be changed :shit:).
- **disk.c**: Functions for disk operations (read/write, info, etc.).
- **file_sys.c**: Functions for file system operations (create, remove, write, read files).
- **vell.c**: Initializes the virtual shell.
