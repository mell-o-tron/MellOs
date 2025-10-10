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

/********************FUNCTIONS*********************
* kb_install: installs keyboard IRQ handler       *
* keyboard_handler: handles interrupt requests    *
**************************************************/

const char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

enum scan_codes 
{
    LEFT_ARROW_PRESSED = 0x4b,
    RIGHT_ARROW_PRESSED = 0x4d,
    DOWN_ARROW_PRESSED = 0x50,
    UP_ARROW_PRESSED = 0x48,
    LEFT_SHIFT_PRESSED = 0x2a,
    CAPS_LOCK_PRESSED = 0x3a,
    RETURN_PRESSED = 0x1c,
    F5_PRESSED = 0x3f,
    LEFT_SHIFT_RELEASED = 0xaa,
    CTRL_PRESSED = 0x1d,
    CTRL_RELEASED = 0x9d
};

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

void keyboard_handler(struct regs *r)
{
    /* static prevents these variables from being reassigned back to false after the function returns and gets called again
    * and it keeps the variables local to this function */
    static bool shift_pressed = false;
    static bool caps_lock = false;
    static bool ctrl_pressed = false;

    unsigned char scancode;

    scancode = inb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to check if the user released the
        *  shift, alt, or control keys... */
       
    	// kprint(tostring_inplace((int)scancode, 16));		// for testing
    	
    	switch(scancode){
    		case LEFT_SHIFT_RELEASED: shift_pressed = false; break;
            case CTRL_RELEASED: ctrl_pressed = false; break;
    	}
	
    }
    else
    {
        // char buf [20];
        // kprint(tostring(scancode, 16, buf));
      
    	switch(scancode){
    		case LEFT_ARROW_PRESSED:  add_to_act_buffer('L'); break;
    		case RIGHT_ARROW_PRESSED: add_to_act_buffer('R'); break;
    		case DOWN_ARROW_PRESSED:  add_to_act_buffer('D'); break;
    		case UP_ARROW_PRESSED:    add_to_act_buffer('U'); break;
    		case LEFT_SHIFT_PRESSED:  shift_pressed = true; break;
            case CTRL_PRESSED:        ctrl_pressed = true; break;
    		case CAPS_LOCK_PRESSED:   caps_lock = !caps_lock; break;
    		case RETURN_PRESSED:      add_to_kb_buffer('\n', 0); break;

    		//ITALIAN KEYBOARD, might not work on others:
    		case 0x56: add_to_kb_buffer(shift_pressed ? '>' : '<', 0); break;
            case 0xE: add_to_kb_buffer(8, 0); break;
    		default:
              if (ctrl_pressed)
                add_to_act_buffer(kbdus[scancode]);
              else {
                add_to_kb_buffer(kbdus[scancode], shift_pressed | caps_lock);
              }
    	}
        
    }
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
