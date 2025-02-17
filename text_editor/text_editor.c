#include "text_editor.h"
#include "../utils/typedefs.h"
#include "../drivers/keyboard.h"
#include "../drivers/vga_text.h"
#include "../misc/colours.h"
#include "../utils/conversions.h"
#include "../file_system/file_system.h"

#define VGA_WIDTH			80
#define VGA_HEIGHT  		25



char document_buffer [1840];
int document_index = 0;
char* cur_file;

void init_text_editor(char* filename){
    cur_file = filename;
    clear_screen_col(DEFAULT_COLOUR);
    clear_line_col(0, DARK_INVERSE);
    clear_line_col(24, DARK_INVERSE);
    set_cursor_pos_raw(0);
    kprint_col("MellOS Text Editor - ", DARK_INVERSE);
    kprint_col(filename, DARK_INVERSE);
    set_cursor_pos_raw(1920);
    kprint_col("ctrl-s to save", DARK_INVERSE);

    set_cursor_pos_raw(80);
    text_editor_loop();
}

void save_file(){
    write_string_to_file(document_buffer, cur_file);
}


void text_editor_loop(){
    while(true){
        char c = get_from_kb_buffer();
		switch (c) {
			case 0: break; // buffer is empty
			case 8: if(document_index > 0) {
                        document_index--;
                        document_buffer[document_index] = ' ';
                        set_cursor_pos_raw(document_index + 80);
                        kprint_char(' ', 0);
                    }
                    break;
			default:
                    kprint_char(c, 0);
                    document_buffer[document_index] = c;
                    int posraw = get_cursor_pos_raw() - 80;
                    if(posraw > 0 && posraw < 1840)
                        document_index = posraw;
		}

		char a = get_from_act_buffer();
		switch (a) {
			case 'D':   if(document_index + VGA_WIDTH < 1840){
                            document_index += VGA_WIDTH;
                        } 
                        break;

			case 'U':   if(document_index - VGA_WIDTH > 0)
                            document_index -= VGA_WIDTH;

                        break;

			case 'L':   if(document_index > 0) {
                            document_index--;
                        }
                        break;

			case 'R':   if(document_index < 1840 - 1) {
                            document_index++;
                        }
                        break;
            case 's':
                        clear_screen_col(DEFAULT_COLOUR);
                        set_cursor_pos_raw(0);
                        save_file();
                        return;
		}
		set_cursor_pos_raw(document_index + 80);
    }
}

