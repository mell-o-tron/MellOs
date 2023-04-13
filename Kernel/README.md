## Kernel

### Kernel Entry

The journey begins at `kernel_entry.asm`. Here we can see:

- The `MemSize` hack thing used to store whatever was in bx in the bootloader (in that particular point described in the bootloader docs) to a variable. This does not necessarily have to be the memory size. The name will be adapted with the usage.
- The inclusion of binaries
- The definition of the `extern main` function, which is where the actual kernel begins
- Other stuff, to be documented

The main function is called, before starting to loop forever on a `jmp $`.

### Main

To be documented...
