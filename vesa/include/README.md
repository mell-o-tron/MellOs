[← Drivers README](../README.md) | [← Main README](../../README.md)

# vesa

This directory contains VESA graphics driver code for MellOs, including VBE initialization and framebuffer graphics routines.

## vesa.h / vesa.c
- **Framebuffer**: Structure representing a VESA framebuffer (width, height, pitch, pointer, etc.).
- **_vesa_framebuffer_init(PIXEL pointer)**: Initializes the VESA framebuffer.
- **allocate_framebuffer(uint32_t width, uint32_t height)**: Allocates a new framebuffer.
- **deallocate_framebuffer(Framebuffer\* fb)**: Frees a framebuffer.
- **blit, blit_all_at, blit_all_at_only, blit_all_at_only_square**: Functions for copying (blitting) framebuffer regions.
- **clear_screen(), clear_screen_col_VESA(VESA_Colour col), clear_screen_col(Colour col)**: Clears the screen with a color.
- **fill_square, fill_rect, fill_circle**: Draws filled shapes on the framebuffer.
- **Hres, Vres, Pitch**: Global variables for screen resolution and pitch.