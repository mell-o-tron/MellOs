#include "../Utils/Typedefs.h"
#include "../Utils/Conversions.h"
#include <VGA_Text.h>
#include <port_io.h>
#include "../CPU/Interrupts/irq.h"
#include "../Shell/shell.h"

/********************FUNCTIONS*********************
* kb_install: installs keyboard IRQ handler       *
* keyboard_handler: handles interrupt requests    *
**************************************************/

extern int curMode;					// CURRENT MODE
unsigned char scancode;
bool kb_ready;
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
    DOWN_ARROW_PRESSED = 0x48,
    UP_ARROW_PRESSED = 0x50,
    LEFT_SHIFT_PRESSED = 0x2a,
    CAPS_LOCK_PRESSED = 0x3a,
    RETURN_PRESSED = 0x1c,
    F5_PRESSED = 0x3f,

    LEFT_SHIFT_RELEASED = 0xaa
};

void keyboard_handler(struct regs *r)
{
    scancode = inb(0x60);
    kb_ready = true;
}
void keyboard_proc(){
    static bool shift_pressed = false;
    static bool caps_lock = false;
	if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
       
    	//kprint(toString((int)scancode, 16));		// for testing
    	
    	
    	switch(scancode){
    		case LEFT_SHIFT_RELEASED: shift_pressed = false; break;
    		
    	}
	
    }
    else
    {
    	
    	switch(scancode){
    		case LEFT_ARROW_PRESSED: MoveCursorLR(-1); break;
    		case RIGHT_ARROW_PRESSED: MoveCursorLR(1); break;
    		case DOWN_ARROW_PRESSED: MoveCursorUD(-1); break;
    		case UP_ARROW_PRESSED: MoveCursorUD(1); break;
    		case LEFT_SHIFT_PRESSED: shift_pressed = true; break;
    		case CAPS_LOCK_PRESSED: caps_lock = !caps_lock; break;
    		case RETURN_PRESSED: 
    			switch(curMode){
    				case 0: kprintChar('\n', 0); break;
                    case 10: 
                        parseCommand();
                        
    			}
    			break;
    		//case 0x3b: SetCmdMode(); break;
            case F5_PRESSED: load_shell();  break;
    									//ITALIAN KEYBOARD, might not work on others:
    		case 0x56: kprintChar(shift_pressed ? '>' : '<', 0); break;	
    		default: kprintChar(kbdus[scancode], shift_pressed | caps_lock);
    	}
        
    }
    kb_ready = false;
}	
void get_keyboard()
{
        while(1){
        	if (scancode != 0 && kb_ready){
        		kprint(toString(scancode, 16)); // get the output printed, to be replaced
        		break; 
        	}
        }
}
void await(unsigned char trigger){
        while(1){
        	if (scancode == trigger && kb_ready){
        		break; 
        	}
        }
}
void keyboard_old(){ // here to simulate how old versions worked, to be replaced
    while(1){
        	if (kb_ready){
        		keyboard_proc();
        	}
        }
}
void kb_install()
{
	irq_install_handler(1, keyboard_handler);
}
