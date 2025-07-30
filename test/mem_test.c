#include "../file_system/file_system.h"
#include "../utils/string.h"
#ifdef VGA_VESA
#include "../drivers/vesa/vesa.h"
#include "../drivers/vesa/vesa_text.h"
#include "../drivers/mouse.h"
#else
#include "../drivers/vga_text.h"
#endif

int malloc_test(){
    void * code_loc = (void*) kmalloc(10);

    if (code_loc == NULL){                          // null check
        return 1;
    }

    return 0;
}

int run_all_mem_tests () {
    return malloc_test() /* + other tests ....  */;
}