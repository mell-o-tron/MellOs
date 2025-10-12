[← Main README](../README.md)

# drivers

This directory contains hardware driver implementations for MellOs, including disk, keyboard, mouse, and VGA text drivers.

## mouse.h / mouse.c
- **MousePacket**: Union representing a mouse data packet (button states, deltas, overflow, etc.).
- **mouse_handler(struct regs *r)**: Handles mouse interrupts.
- **mouse_install()**: Installs the mouse interrupt handler.

## keyboard.h / keyboard.c
- **keyboard_handler(struct regs *r)**: Handles keyboard interrupts.
- **kb_install()**: Installs the keyboard interrupt handler.
- **add_to_kb_buffer(char c, bool is_uppercase)**: Adds a character to the keyboard buffer.
- **get_from_kb_buffer()**: Retrieves a character from the keyboard buffer.
- **rem_from_kb_buffer()**: Removes a character from the keyboard buffer.


## Subdirectories
- [VESA Graphics Driver](vesa/README.md)

