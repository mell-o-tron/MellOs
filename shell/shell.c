#include "shell.h"
#include "utils/typedefs.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa_text.h"
#else
#include "drivers/vga_text.h"
#endif
#include "misc/colours.h"
#include "utils/string.h"
#include "shell_functions.h"
#include "memory/mem.h"
#include "utils/conversions.h"
#include "drivers/keyboard.h"
#include "data_structures/circular_buffer.h"
#include "file_system/file_system.h"
#include "memory/dynamic_mem.h"
#include "drivers/uart.h"

#define COMMAND_BUFFER_LENGTH 128
#define COMMAND_HISTORY_SIZE  32

char command_buffer[COMMAND_BUFFER_LENGTH];
uint32_t command_buffer_index = 0;

char command_history[COMMAND_HISTORY_SIZE][COMMAND_BUFFER_LENGTH];
uint8_t command_history_index = 0xFF;

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
    
    while (true){
        handleHistoryKeys(get_from_act_buffer());

        char c = get_from_kb_buffer();
        if(c == 0) {c = read_any_serial_non_blocking();}
		switch (c) {
			case 0: break; // buffer is empty
            case '\n':
                parseCommand();
                command_buffer_index = 0;
                for (uint32_t j = 0; j < COMMAND_BUFFER_LENGTH; j++)
                    command_buffer[j] = 0;
                break;
			case 8: // backspace
                if(command_buffer_index > 0) {
                    command_buffer[command_buffer_index] = 0;
                    command_buffer_index--;
                    move_cursor_LR(-1);
                    kprint_char(' ', 0);
                    move_cursor_LR(-1);
                }
                break;
			default:
                kprint_char(c, 0);
                command_buffer[command_buffer_index] = c;
                if(command_buffer_index < COMMAND_BUFFER_LENGTH) command_buffer_index++;
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
        
        saveCommand(command_buffer, command_buffer_index);
    }

    //SetCursorPosRaw(1920);
    refreshShell();
    
}

void eraseCurrentCommand() {
    for(int i = 0; i < command_buffer_index; i++) {
        move_cursor_LR(-1);
        kprint_char(' ', 0);
        move_cursor_LR(-1);
    }

    command_buffer_index = 0;
}

void typeCommand(char* command, uint32_t length) {
    for(int i = 0; i < length; i++) {
        kprint_char(command[i], 0);
    }
}

void saveCommand(char* command, uint32_t length) {
    for(int i = COMMAND_HISTORY_SIZE - 1; i >= 1; i--) {
        memcpy(command_history[i], command_history[i - 1], COMMAND_BUFFER_LENGTH);
    }
    memset(command_history[0], 0, COMMAND_BUFFER_LENGTH);
    memcpy(command_history[0], command, length);
    command_history_index = 0xFF;
}

void restoreCommand(uint8_t index) {
    eraseCurrentCommand();

    memcpy(command_buffer, command_history[index], COMMAND_BUFFER_LENGTH);
    command_buffer_index = strlen(command_buffer);
    
    typeCommand(command_buffer, strlen(command_buffer));
}

void handleHistoryKeys(char action) {
    bool do_restore = false;

    if(action == 'U') {
        if(command_history_index < COMMAND_HISTORY_SIZE - 1) {
            if(command_history[command_history_index + 1][0]) {
                command_history_index++;
                do_restore = true;
            }
        } else if(command_history_index == 0xFF) {
            if(command_history[0][0]) {
                command_history_index = 0;
                do_restore = true;
            }
        }
    } else if(action == 'D') {
        if(command_history_index > 0 && command_history_index != 0xFF) {
            command_history_index--;
            do_restore = true;
        }
    }

    if(do_restore) {
        restoreCommand(command_history_index);
    }
}
