#include "shell/shell.h"
#include "autoconf.h"

#include "linked_list.h"

#include "processes.h"

#include "stddef.h"

#include "mellos/kernel/kernel_stdio.h"
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

char command_buffer[128];

cbuffer_t shell_tasks;

inode_t *current_dir = NULL;

inode_t* get_working_dir() {
	return current_dir;
}

void set_working_dir(inode_t* dir) {
	current_dir = dir;
}

void add_filewrite_task(char* str, char* filename, uint32_t len) {
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

void load_shell() {
	current_dir = get_root_mount()->root;
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
	while (true) {
		char c = get_from_kb_buffer();
		if (c == 0) {
			c = read_any_serial_non_blocking();
		}
		switch (c) {
		case 0:
			break; // buffer is empty
		case '\n':
			parseCommand();
			i = 0;
			for (uint32_t j = 0; j < 128; j++)
				command_buffer[j] = 0;
			break;
		case 8: // backspace
			if (i > 0) {
				command_buffer[i - 1] = 0;
				i--;
				move_cursor_LR(-1);
				kprint_char(' ', 0);
				move_cursor_LR(-1);
			}
			break;
		default:
			kprint_char(c, 0);
			command_buffer[i] = c;
			if (i < 128)
				i++;
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

void refreshShell() {
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
		shellfunction* cmd = TryGetCMD(command_buffer);
		if (cmd != 0) {
			// execute command
			cmd->fptr(str_decapitate(command_buffer, strlen(cmd->alias)));
		} else {
			kprint("\"");
			kprint(command_buffer);
			kprint("\" is not a command\n");
		}
	}

	// SetCursorPosRaw(1920);
	refreshShell();
}
