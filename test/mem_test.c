#include "file_system/file_system.h"
#include "utils/string.h"
#include "memory/dynamic_mem.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa.h"
#include "drivers/vesa/vesa_text.h"
#include "drivers/mouse.h"
#else
#include "drivers/vga_text.h"
#endif

#define TEST_SIZE 10

int malloc_test(){
    void * code_loc = kmalloc(TEST_SIZE);

    if (code_loc == NULL){                          // null check
        return 1;
    } else {
        kfree(code_loc, TEST_SIZE);
    }

    return 0;
}

int run_all_mem_tests() {
    return malloc_test() /* + other tests ....  */;
}