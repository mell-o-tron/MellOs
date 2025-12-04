#include "shell/shell_functions.h"
#include "processes.h"
#include "conversions.h"
#include "string.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa_text.h"
#else
#include "vga_text.h"
#endif

void kill(const char* s) {
    if (strlen(s) == 0) {
        kprint("Usage: kill [PID]\n");
        kprint("Kills the process with the specified PID\n");
        return;
    }
    
    // Parse the PID from the argument
    uint32_t pid = 0;
    const char* arg = s;
    
    // Ignore the spaces at the beginning
    while (*arg == ' ') arg++;
    
    // Convert the string to a number
    if (*arg == '\0') {
        kprint("Error: No PID specified\n");
        return;
    }
    
    // Simple string to integer conversion
    while (*arg >= '0' && *arg <= '9') {
        pid = pid * 10 + (*arg - '0');
        arg++;
    }
    
    // Check that there are only digits
    while (*arg == ' ') arg++;
    if (*arg != '\0') {
        kprint("Error: Invalid PID format\n");
        return;
    }
    
    // Call the kill_task function
    kill_task(pid);
}

void ps(const char* s) {
    list_processes();
}
