/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/

//IMPLEMENT STACK SMASHING PROTECTOR!!!!

#include "../UsefulStuff/Typedefs.h"
#include "../Drivers/VGA_Text.h"
#include "../UsefulStuff/Conversions.h"
#include "../UsefulStuff/mem.h"
#include "../CPU/idt.h"
#include "../CPU/isr.h"
#include "../CPU/irq.h"
#include "../CPU/timer.h"
#include "../Drivers/Keyboard.h"
#include "../Misc/CmdMode.h"


int curMode;					// Modes: 1 = CMD, 2: code, 0: dummy text

extern const char Fool[];			// Test included binaries
extern const char John[];


extern "C" void main(){
	//asm volatile("1: jmp 1b");		// "entry breakpoint" (debug)
	
	
	idt_install();
	isrs_install();
	irq_install();
	asm volatile ("sti");
	timer_install();
	kb_install();
	SetCmdMode();
	
	return;
}


