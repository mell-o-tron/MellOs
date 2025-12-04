[← CPU README](../README.md) | [← Main README](../../README.md)

# gdt

This directory contains code related to the Global Descriptor Table (GDT) for MellOs, including initialization and management routines.

## gdt.c / gdt.h
- **GDTEntry**: Structure representing a GDT entry (segment descriptor fields: limit, base, access, flags).
- **GDTDescriptor**: Structure holding the address and size of the GDT.
- **gdt_init()**: Initializes the GDT and loads it into the CPU.
- **gdt_load(GDTDescriptor\* descriptor, uint16_t codeSegment, uint16_t dataSegment)**: Loads the GDT using the provided descriptor and sets segment registers.
- **GDT_CODE_SEGMENT, GDT_DATA_SEGMENT**: Segment selector constants.
- **ENTRY(base, limit, access, flags)**: Macro to create a GDT entry.