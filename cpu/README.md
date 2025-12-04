[← Main README](../README.md)

# cpu

This directory contains CPU-related source code and utilities for MellOs, including feature detection, model-specific register (MSR) access, and CPU table management.

## cpuid.c / cpuid.h
Implements CPU feature detection and querying.

- **cpuid(int code, uint32_t *a, uint32_t *d)**: Inline function to execute the CPUID instruction with the given code, returning results in `a` and `d`.
- **cpuid_string(int code, uint32_t where[4])**: Retrieves a string from the CPUID instruction.
- **cpuid_has_mmx(void)**: Returns nonzero if the CPU supports MMX instructions.
- **cpuid_has_sse(void)**: Returns nonzero if the CPU supports SSE instructions.

## msr.c / msr.h
Implements access to Model-Specific Registers (MSRs).

- **cpu_get_MSR(uint32_t msr, uint32_t *lo, uint32_t *hi)**: Reads the MSR at the given address into `lo` and `hi`.
- **cpu_set_MSR(uint32_t msr, uint32_t lo, uint32_t hi)**: Writes values to the MSR at the given address.


## Subdirectories
- [GDT](gdt/README.md)
- [Interrupts](interrupts/README.md)
- [Timer](timer/README.md)
Subdirectories for Global Descriptor Table, interrupt handling, and timer code.
