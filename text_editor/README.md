
[← Main README](../README.md)

# text_editor

This directory contains a very simple, very stupid and slightly broken text editor implementation for MellOs.

## text_editor.h / text_editor.c
- **init_text_editor(char\* filename)**: Initializes the text editor with the given file.
- **text_editor_loop()**: Main loop for the text editor, handling user input and editing.
- **save_file()**: Saves the current document to disk.
- **document_buffer, document_string**: Buffers for the document being edited.
- **document_index**: Current position in the document.