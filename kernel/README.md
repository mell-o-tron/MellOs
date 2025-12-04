[← Main README](../README.md)

# kernel

This directory contains the MellOs kernel source code and its entry point. It also provides a kernel panic (with cute frogs :frog:) for horrible fatal errors.

## kernel.c / kernel.h
- **kpanic(struct regs \*r)**: Kernel panic handler :frog: for fatal errors.
- **kpanic_message(const char\* msg)**: Kernel panic :frog: with custom message.

## memory_mapper.c / memory_mapper.h
- **get_multiboot_framebuffer_addr(const MultibootTags\* mb)**: Gets the framebuffer address from multiboot tags.
- **init_memory_mapper(MultibootTags \*multiboot_tags, void \*framebuffer_addr, uint8_t bpp)**: Initializes the memory mapper.

## Entry Point
- **kernel_entry.asm**: Kernel entry point in assembly.
