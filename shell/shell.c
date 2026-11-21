#include "shell/shell.h"
#include "autoconf.h"

#include "linked_list.h"

#include "mellos/kernel/kernel.h"
#include "mellos/ramfs.h"
#include "processes.h"

#include "stddef.h"

#include "kernel_stdio.h"

#include "mellos/kernel/mount_manager.h"
#ifdef CONFIG_GFX_VESA
#include "vesa_text.h"
#else
#include "vga_text.h"
#endif
#include "circular_buffer.h"
#include "dynamic_mem.h"
#include "keyboard.h"
#include "shell/shell_functions.h"
#include "string.h"
#include "uart.h"

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

inode_t *current_dir = NULL;

inode_t* get_working_dir() {
	return current_dir;
}

void set_working_dir(inode_t* dir) {
	current_dir = dir;
}



void load_shell() {
	if (get_root_mount() == NULL) {
		kfprintf(kstderr, "root mount is null");
		kpanic_message("root mount is null");
	} else {
		current_dir = get_root_mount()->sb->root;
	}

	shell_tasks.size = 1000;
    shell_tasks.array = kmalloc(shell_tasks.size);
    shell_tasks.top = 0;
    shell_tasks.bot = 0;

	refresh_shell();

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
                    command_buffer[--command_buffer_index] = 0;
                    move_cursor_LR(-1);
                    kprint_char(' ', 0);
                    move_cursor_LR(-1);
                }
                break;
			default:
                kprint_char(c, 0);
                command_buffer[command_buffer_index] = c;
		        if (command_buffer_index < COMMAND_BUFFER_LENGTH)
			        command_buffer_index++;
			}
				process_t* current = get_current_process();
				if (current && current->children_list) {
					for (int child_idx = 0; child_idx < linked_list_size(current->children_list); child_idx++) {
						process_t* child = linked_list_get(current->children_list, child_idx);
						if (!child) {
							continue;
						}
						if (child->stdout) {
							char pipe_buffer[256];
							ssize_t bytes =
								pipe_read_nonblocking(child->stdout, pipe_buffer, sizeof(pipe_buffer));
							for (ssize_t j = 0; j < bytes; j++) {
								kprint_char(pipe_buffer[j], 0);
							}
						}
						if (child->stderr) {
							char pipe_buffer[256];
							ssize_t bytes =
								pipe_read_nonblocking(child->stderr, pipe_buffer, sizeof(pipe_buffer));
							ssize_t j = 0;
							bool found = false;
							for (; j < bytes; j++) {
								if (pipe_buffer[j] == '\0') {
									found = true;
									break;
								}
							}
							if (!found) {
								pipe_buffer[bytes - 1] = '\0';
							}

							kprint_col(pipe_buffer, 0);
						}
					}
		        }


		/*
		while (shell_tasks.bot != shell_tasks.top) {
			char task = get_from_cbuffer(&shell_tasks);
			if (task == 'W') {
				uint32_t a = get_from_cbuffer(&shell_tasks) & 0xFF;
				uint32_t b = get_from_cbuffer(&shell_tasks) & 0xFF;
				uint32_t c = get_from_cbuffer(&shell_tasks) & 0xFF;
				uint32_t d = get_from_cbuffer(&shell_tasks) & 0xFF;

				char* filename = (char*)(a | b << 8 | c << 16 | d << 24);

				a = get_from_cbuffer(&shell_tasks) & 0xFF;
				b = get_from_cbuffer(&shell_tasks) & 0xFF;
				c = get_from_cbuffer(&shell_tasks) & 0xFF;
				d = get_from_cbuffer(&shell_tasks) & 0xFF;

				char* str = (char*)(a | b << 8 | c << 16 | d << 24);

				a = get_from_cbuffer(&shell_tasks) & 0xFF;
				b = get_from_cbuffer(&shell_tasks) & 0xFF;
				c = get_from_cbuffer(&shell_tasks) & 0xFF;
				d = get_from_cbuffer(&shell_tasks) & 0xFF;

				uint32_t len = (a | b << 8 | c << 16 | d << 24);

				remove_file(filename);
				new_file(filename, (len / 512 + 1));

				write_string_to_file(str, filename);
			}
		}*/
	}

	return;
}

void refresh_shell() {
	while (current_dir == NULL) {
		__builtin_ia32_pause();
	}
	kprintf("%s> ", current_dir->dentry->name);
	return;
}

void parseCommand() {
	kprint("\n");
	if (command_buffer[0] == 0) {
		;
	} else {
		saveCommand(command_buffer, command_buffer_index);
		char* ptr = kstrchr(command_buffer, ' ');

		shellfunction* cmd = NULL;
		char* buffer_copy = NULL;
		size_t space_offset;
		if (ptr != NULL) {
			space_offset = ptr - command_buffer;
			if (space_offset <= 0) {
				kfprintf(kstderr, "Invalid first character");
				kfree(buffer_copy);
				goto exit;
			}
			buffer_copy = kstrdup(command_buffer);
			// TODO: static buffer copy instance for each shell instance so no need to allocate each
			// time we parse a new command
			buffer_copy[space_offset] = '\0';
			cmd = TryGetCMD(buffer_copy);
			if (cmd == NULL) {
				kprintf("Command not found: %s\n", buffer_copy);
				kfree(buffer_copy);
				goto exit;
			}
		} else {

			cmd = TryGetCMD(command_buffer);
			if (cmd == NULL) {
				kprintf("Command not found: %s\n", command_buffer);
				goto exit;
			}
		}
		// execute command


		if (buffer_copy == NULL) {
			cmd->fptr("");
		} else {
			cmd->fptr(&(buffer_copy[space_offset + 1]));
		}
		if (buffer_copy != NULL) {
			kfree(buffer_copy);
		}
	}
exit:

	// SetCursorPosRaw(1920);
	refresh_shell();
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
        kmemcpy(command_history[i], command_history[i - 1], COMMAND_BUFFER_LENGTH);
    }
    kmemset(command_history[0], 0, COMMAND_BUFFER_LENGTH);
    kmemcpy(command_history[0], command, length);
    command_history_index = 0xFF;
}

void restoreCommand(uint8_t index) {
    eraseCurrentCommand();

    kmemcpy(command_buffer, command_history[index], COMMAND_BUFFER_LENGTH);
    command_buffer_index = kstrlen(command_buffer);
    
    typeCommand(command_buffer, kstrlen(command_buffer));
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
