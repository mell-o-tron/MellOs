
[← Main README](../README.md)

# syscalls

This directory contains system call interface code for MellOs, providing the kernel's syscall entry points and handlers. :warning: THESE ARE CURRENTLY JUST STUBS :warning:

## syscalls.h / syscalls.c
- **syscall_stub(regs \*r)**: Main syscall handler stub.
- **sys_exit(regs \*r)**: Terminates the current process.
- **sys_fork(regs \*r)**: Forks the current process.
- **sys_read(regs \*r)**: Reads data from a file or device.
- **sys_write(regs \*r)**: Writes data to a file or device.
- **sys_open(regs \*r)**: Opens a file or device.
- **sys_close(regs \*r)**: Closes a file or device.
