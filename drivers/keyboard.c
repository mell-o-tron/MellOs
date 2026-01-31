#include "../utils/typedefs.h"
#include "../utils/conversions.h"
#ifdef VGA_VESA
#include "drivers/vesa/vesa_text.h"
#else
#include "drivers/vga_text.h"
#endif
#include "port_io.h"
#include "../cpu/interrupts/irq.h"
#include "../memory/dynamic_mem.h"
#include "../data_structures/circular_buffer.h"
#include "../utils/string.h"

#define MAPPED_KEYS 92

/********************FUNCTIONS*********************
* kb_install: installs keyboard IRQ handler       *
* keyboard_handler: handles interrupt requests    *
**************************************************/

typedef struct {
    char main;
    char shift;
    char altgr;
    char altgr_shift;
} keydef;

typedef struct {
    char name[4];
    keydef keys[MAPPED_KEYS];
} kblayout;

const kblayout layout_us = {"us", {
    {0},
    {27}, // ESC

    {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'}, {'9', '('}, {'0', ')'},
    {'-', '_'}, {'=', '+'}, {'\b'},

    {'\t'}, {'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'}, {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'},
    {'[', '{'}, {']', '}'}, {'\n'},
    
    {0}, // LCTRL
    {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'},
    {';', ':'}, {'\'', '"'},
    {'`', '~'},
    {0}, // LSHIFT
    {'\\', '|'},

    {'z', 'Z'}, {'x', 'X'}, {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'}, {'m', 'M'},
    {',', '<'}, {'.', '>'}, {'/', '?'},
    {0}, // RSHIFT

    {'*'}, // NUMPAD *
    {0},   // LALT
    {' '}, // SPACEBAR
    {0},   // CAPS LOCK

    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, // F1 - F10
    
    {0}, // NUM LOCK
    {0}, // SCROLL LOCK

    // NUMPAD
    {0}, {0}, {0}, {'-'},
    {0}, {0}, {0}, {'+'},
    {0}, {0}, {0},
    {0}, {0},

    {0},         // PRINT
    {0},         // DEPRECATED
    {'\\', '|'}, // INTL 3
    {0}, {0}     // F11 - F12
}};

const kblayout layout_it = {"it", {
    {0},
    {27}, // ESC

    {'1', '!'}, {'2', '"'}, {'3', 163}, {'4', '$'}, {'5', '%', 128}, {'6', '&'}, {'7', '/'}, {'8', '('}, {'9', ')'}, {'0', '='},
    {'\'', '?'}, {236, '^'}, {'\b'},

    {'\t'}, {'q', 'Q'}, {'w', 'W'}, {'e', 'E', 128}, {'r', 'R'}, {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'},
    {232, 233, '[', '{'}, {'+', '*', ']', '}'}, {'\n'},
    
    {0}, // LCTRL
    {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'},
    {242, 231, '@'}, {224, 176, '#'},
    {'\\', '|'},
    {0}, // LSHIFT
    {249, 167},

    {'z', 'Z'}, {'x', 'X'}, {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'}, {'m', 'M'},
    {',', ';'}, {'.', ':'}, {'-', '_'},
    {0}, // RSHIFT

    {'*'}, // NUMPAD *
    {0},   // LALT
    {' '}, // SPACEBAR
    {0},   // CAPS LOCK

    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, // F1 - F10
    
    {0}, // NUM LOCK
    {0}, // SCROLL LOCK

    // NUMPAD
    {0}, {0}, {0}, {'-'},
    {0}, {0}, {0}, {'+'},
    {0}, {0}, {0},
    {0}, {0},

    {0},         // PRINT
    {0},         // DEPRECATED
    {'<', '>'},  // INTL 3
    {0}, {0}     // F11 - F12
}};

const kblayout* const layouts[] = {
    &layout_us,
    &layout_it
};

const uint32_t layouts_count = sizeof(layouts) / sizeof(uint32_t);
const kblayout* selected_layout = &layout_us;

enum scan_codes 
{
    UP_ARROW_PRESSED     = 0x48,
    DOWN_ARROW_PRESSED   = 0x50,
    LEFT_ARROW_PRESSED   = 0x4b,
    RIGHT_ARROW_PRESSED  = 0x4d,

    EXTENDED_PREFIX      = 0xE0,

    CAPS_LOCK_PRESSED    = 0x3a,
    RETURN_PRESSED       = 0x1c,
    F5_PRESSED           = 0x3f,

    LEFT_SHIFT_PRESSED   = 0x2a,
    RIGHT_SHIFT_PRESSED  = 0x36,
    CTRL_PRESSED         = 0x1d,
    ALT_PRESSED          = 0x38,

    LEFT_SHIFT_RELEASED  = 0xaa,
    RIGHT_SHIFT_RELEASED = 0xB6,
    CTRL_RELEASED        = 0x9d,
    ALT_RELEASED         = 0xB8
};

/* COMMANDS */

void printLayouts() {
    kprint("Available layouts: ");
    for(int i = 0; i < layouts_count; i++) {
        kprint(layouts[i]->name);
        if(i < layouts_count - 1)
            kprint(", ");
    }
    kprint("\n");
}

void kbSetLayout(const char* command) {
    uint8_t str_start = 0;
    
    for(int i = 0; i < 10; i++) {
        if(command[i] != ' ')
            break;
        str_start++;
    }
    
    if(command[str_start] == 0) {
        kprint("Selected layout: ");
        kprint(selected_layout->name);
        kprint("\n");
    } else {
        for(int i = 0; i < layouts_count; i++) {
            uint32_t diff = strcmp(command + str_start, layouts[i]->name);
            if(diff == 0) {
                selected_layout = layouts[i];
                return;
            }
        }
        
        kprint("No such keyboard layout: \"");
        kprint(command + str_start);
        kprint("\"\n");
    }

    printLayouts();
}

/* KEYBOARD BUFFER */

cbuffer_t * keyboard_buffer;

void add_to_kb_buffer(char c, bool is_uppercase){
  add_to_cbuffer(keyboard_buffer, c, is_uppercase);
}

char get_from_kb_buffer(){
  return get_from_cbuffer(keyboard_buffer);
}

void rem_from_kb_buffer(){
  
  if(!keyboard_buffer) return;
  
  rem_from_cbuffer(keyboard_buffer);
}

/* ACTION BUFFER - used for cursor movements and the like */

cbuffer_t* action_buffer;

void add_to_act_buffer(char c){
  add_to_cbuffer(action_buffer, c, false);
}

char get_from_act_buffer(){
  return get_from_cbuffer(action_buffer);
}

void rem_from_act_buffer(){
  rem_from_cbuffer(action_buffer);
}

void keyboard_handler(struct regs *r) {
    /* static prevents these variables from being reassigned back to false after the function returns and gets called again
    * and it keeps the variables local to this function */
    static bool shift_pressed = false;
    static bool caps_lock = false;
    static bool ctrl_pressed = false;
    static bool alt_pressed = false;
    static bool altgr_pressed = false;

    static uint8_t prev_scancode = 0;
    uint8_t scancode = inb(0x60);

    if(scancode != EXTENDED_PREFIX) {
        /* If the top bit of the byte we read from the keyboard is
        *  set, that means that a key has just been released */
        if(scancode & 0x80) {
            /* You can use this one to check if the user released the
            *  shift, alt, or control keys... */
            
            // kprint(tostring_inplace((int)scancode, 16));		// for testing
            
            switch(scancode) {
                case LEFT_SHIFT_RELEASED:
                case RIGHT_SHIFT_RELEASED: shift_pressed = false; break;
                case CTRL_RELEASED:        ctrl_pressed = false;  break;
                case ALT_RELEASED: {
                    if(prev_scancode == EXTENDED_PREFIX) {
                        altgr_pressed = false;
                    } else {
                        alt_pressed = false;
                    }
                    break;
                }
            }
        } else {
            // char buf [20];
            // kprint(tostring(scancode, 16, buf));

            switch(scancode) {
                case LEFT_ARROW_PRESSED:  add_to_act_buffer('L'); break;
                case RIGHT_ARROW_PRESSED: add_to_act_buffer('R'); break;
                case DOWN_ARROW_PRESSED:  add_to_act_buffer('D'); break;
                case UP_ARROW_PRESSED:    add_to_act_buffer('U'); break;

                case LEFT_SHIFT_PRESSED:
                case RIGHT_SHIFT_PRESSED: shift_pressed = true;   break;
                case CTRL_PRESSED:        ctrl_pressed = true;    break;
                case CAPS_LOCK_PRESSED:   caps_lock = !caps_lock; break;
                case ALT_PRESSED: {
                    if(prev_scancode == EXTENDED_PREFIX) {
                        altgr_pressed = true;
                    } else {
                        alt_pressed = true;
                    }
                    break;
                }

                default: {
                    if(scancode >= MAPPED_KEYS)
                        break;

                    keydef key = selected_layout->keys[scancode];

                    if(ctrl_pressed) {
                        add_to_act_buffer(key.main);
                    } else {
                        bool shift = shift_pressed || caps_lock;
                        char c = altgr_pressed ? (shift ? key.altgr_shift : key.altgr) : (shift ? key.shift : key.main);

                        if(c != 0) {
                            add_to_kb_buffer(c, false);
                        }
                    }
                }
            }
        }
    }

    prev_scancode = scancode;
}

void kb_install()
{   
    keyboard_buffer = kmalloc(sizeof(cbuffer_t));
    action_buffer =   kmalloc(sizeof(cbuffer_t));
    
    keyboard_buffer -> size = 1000;
    keyboard_buffer -> top = 0;
    keyboard_buffer -> bot = 0;
    keyboard_buffer -> array = kmalloc(keyboard_buffer -> size);
    
    action_buffer -> size = 1000;
    action_buffer -> top = 0;
    action_buffer -> bot = 0;
    action_buffer -> array = kmalloc(action_buffer -> size);

	irq_install_handler(1, keyboard_handler);
}
