#include "shell.h"
#include "../utils/typedefs.h"
#include "../drivers/vga_text.h"
#include "../misc/colours.h"
#include "../utils/string.h"
#include "shell_functions.h"
#include "../memory/mem.h"
#include "../utils/conversions.h"
#include "../drivers/keyboard.h"




char command_buffer[128];

void load_shell(){
	refreshShell();

    uint32_t i = 0;
    while (true){
        char c = get_from_kb_buffer();
		switch (c) {
			case 0: break; // buffer is empty
            case '\n':
                kprint("\n");
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
        else {kprint("\""); kprint(command_buffer); kprint("\" is not a command");}
    }

    //SetCursorPosRaw(1920);
    refreshShell();
    
}
