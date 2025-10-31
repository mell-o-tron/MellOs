#include "autoconf.h"
#include <errno.h>
#include <string.h>

#include "stdio.h"
#include "stdint.h"
#include "stddef.h"
#include "init.h"
#include "unistd.h"

extern char ker_tty[4000];

#define ERROR_COLOUR 0x000000
static void (*assert_clscolor)(uint8_t) = NULL;
static void (*assert_set_cursor_pos_raw)(uint16_t) = NULL;
static void (*assert_kprint)(const char*) = NULL;
static void (*assert_kclear_screen)(void) = NULL;



void init_assertions(void (*clscolorptr)(uint8_t), void (*set_cursor_pos_rawptr)(uint16_t),
    void (*kclear_screenptr)(void)) {
    assert_kclear_screen = kclear_screenptr;
    assert_clscolor = clscolorptr;
    assert_set_cursor_pos_raw = set_cursor_pos_rawptr;
}

void __assert_fail(const char* expr, const char* file, int line) { // NOLINT(*-reserved-identifier)
    fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", expr, file, line);
    syscall_exit(1);
}

void __assert_fail_msg(const char* expr, const char* message, const char* file, int line) { // NOLINT(*-reserved-identifier)
    fprintf(stderr, "Message: %s", message);
    fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", expr, file, line);
    syscall_exit(1);
}

void assert(uint8_t condition){
    if(!condition){
        #ifdef CONFIG_GFX_VESA
        if (assert_kclear_screen) assert_kclear_screen();
        #else
        if (assert_clscolor) assert_clscolor(ERROR_COLOUR);
        #endif

        if (assert_set_cursor_pos_raw) assert_set_cursor_pos_raw(0);
        
        if (assert_kprint) assert_kprint("assert failed");
    }
}
