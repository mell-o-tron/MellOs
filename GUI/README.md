[← Main README](../README.md)

# GUI

This directory contains graphical user interface code for MellOs, including window management, mouse handling, and virtual terminal support.

## gui.h / gui.c
- **Window**: Structure representing a GUI window (position, size, etc.).
- **create_window_with_size(const char\* title, uint32_t width, uint32_t height)**: Creates a new window with the specified title and size.
- **destroy_window(Window\* w)**: Destroys a window and frees resources.
- **destroy_window_preserve_fb(Window\* w)**: Destroys a window but preserves its framebuffer.
- **set_window_transparent(Window\* w, bool transparent)**: Sets window transparency.
- **set_window_dirty(Window\* w)**: Marks a window as needing redraw.

## mouse_handler.h / mouse_handler.c
- **MouseButton**: Enum for mouse buttons.
- **_init_mouse_handler()**: Initializes mouse handling.
- **_deinit_mouse_handler()**: Deinitializes mouse handling.
- **move_mouse(int x, int y)**: Moves the mouse cursor.
- **mouse_up(MouseButton button)**: Handles mouse button release.
- **mouse_down(MouseButton button)**: Handles mouse button press.

## vterm.h / vterm.c
- **_init_vterm()**: Initializes the virtual terminal.
- **_deinit_vterm()**: Deinitializes the virtual terminal.