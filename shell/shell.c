#include "shell/shell.h"
#include "stddef.h"
#ifdef VGA_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "colours.h"
#include "string.h"
#include "shell/shell_functions.h"
#include "mem.h"
#include "conversions.h"
#include "keyboard.h"
#include "circular_buffer.h"
#include "file_system.h"
#include "dynamic_mem.h"
#include "uart.h"


char command_buffer[128];

cbuffer_t shell_tasks;

void add_filewrite_task(char* str, char* filename, uint32_t len){
    add_to_cbuffer(&shell_tasks, 'W', false);
    
    add_to_cbuffer(&shell_tasks, (uint32_t)filename & 0xFF, false);
    add_to_cbuffer(&shell_tasks, ((uint32_t)filename >> 8) & 0xFF, false);
    add_to_cbuffer(&shell_tasks, ((uint32_t)filename >> 16) & 0xFF, false);
    add_to_cbuffer(&shell_tasks, ((uint32_t)filename >> 24) & 0xFF, false);
    
    add_to_cbuffer(&shell_tasks, (uint32_t)str & 0xFF, false);
    add_to_cbuffer(&shell_tasks, ((uint32_t)str >> 8) & 0xFF, false);
    add_to_cbuffer(&shell_tasks, ((uint32_t)str >> 16) & 0xFF, false);
    add_to_cbuffer(&shell_tasks, ((uint32_t)str >> 24) & 0xFF, false);
    
    add_to_cbuffer(&shell_tasks, (uint32_t)len & 0xFF, false);
    add_to_cbuffer(&shell_tasks, ((uint32_t)len >> 8) & 0xFF, false);
    add_to_cbuffer(&shell_tasks, ((uint32_t)len >> 16) & 0xFF, false);
    add_to_cbuffer(&shell_tasks, ((uint32_t)len >> 24) & 0xFF, false);
    
}

void load_shell(){
	refreshShell();
    shell_tasks.size = 1000;
    shell_tasks.array = kmalloc(shell_tasks.size);
    shell_tasks.top = 0;
    shell_tasks.bot = 0;

    // char buf = "vell\n";
    // shellfunction* cmd = TryGetCMD(buf);
    // if(cmd != 0){
    //     // execute command
    //     cmd->fptr("");
    // }
    
    uint32_t i = 0;
    while (true){
        char c = get_from_kb_buffer();
        if(c == 0) {c = read_any_serial_non_blocking();}
		switch (c) {
			case 0: break; // buffer is empty
            case '\n':
                parseCommand();
                i = 0;
                for (uint32_t j = 0; j < 128; j++)
                    command_buffer[j] = 0;
                break;
			case 8: // backspace
                if(i > 0) {
                    command_buffer[i] = 0;
                    i--;
                    move_cursor_LR(-1);
                    kprint_char(' ', 0);
                    move_cursor_LR(-1);
                }
                break;
			default:
                kprint_char(c, 0);
                command_buffer[i] = c;
                if(i < 128) i++;
		}
		
		while (shell_tasks.bot != shell_tasks.top){
            char task = get_from_cbuffer(&shell_tasks);
            if (task == 'W') {
                uint32_t a = get_from_cbuffer(&shell_tasks) & 0xFF;
                uint32_t b = get_from_cbuffer(&shell_tasks) & 0xFF;
                uint32_t c = get_from_cbuffer(&shell_tasks) & 0xFF;
                uint32_t d = get_from_cbuffer(&shell_tasks) & 0xFF;
                
                char* filename = (char*) (a | b << 8 | c << 16 | d << 24);
                
                a = get_from_cbuffer(&shell_tasks) & 0xFF;
                b = get_from_cbuffer(&shell_tasks) & 0xFF;
                c = get_from_cbuffer(&shell_tasks) & 0xFF;
                d = get_from_cbuffer(&shell_tasks) & 0xFF;
                
                char* str = (char*) (a | b << 8 | c << 16 | d << 24);
                
                
                a = get_from_cbuffer(&shell_tasks) & 0xFF;
                b = get_from_cbuffer(&shell_tasks) & 0xFF;
                c = get_from_cbuffer(&shell_tasks) & 0xFF;
                d = get_from_cbuffer(&shell_tasks) & 0xFF;
                
                uint32_t len = (a | b << 8 | c << 16 | d << 24);

                
                remove_file(filename);
                new_file(filename, (len / 512 + 1));
                
                write_string_to_file(str, filename);
            }
        }
    }

    
	return;
}

void refreshShell(){
	kprint("> ");
	return;
}


void parseCommand(){
    kprint("\n");
    if(command_buffer[0] == 0){;}
    else{
        shellfunction* cmd = TryGetCMD(command_buffer);
        if(cmd != 0){
            // execute command
            cmd->fptr(str_decapitate(command_buffer, strlen(cmd->alias)));
        }
        else {kprint("\""); kprint(command_buffer); kprint("\" is not a command\n");}
    }

    //SetCursorPosRaw(1920);
    refreshShell();
    
}
