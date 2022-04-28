/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/

//IMPLEMENT STACK SMASHING PROTECTOR!!!!

#include "../Utils/Typedefs.h"
#include "../Drivers/VGA_Text.h"
#include "../Utils/Conversions.h"
#include "../Utils/string.h"
#include "../Memory/mem.h"
#include "../CPU/Interrupts/idt.h"
#include "../CPU/Interrupts/isr.h"
#include "../CPU/Interrupts/irq.h"
#include "../CPU/Timer/timer.h"
#include "../CPU/GDT/gdt.h"
#include "../Drivers/Keyboard.h"
#include "../Drivers/Floppy.h"
#include "../Misc/CmdMode.h"
#include "../Utils/dataStructures.h"
#include "../Shell/shell.h"

int curMode;					        // Modes:	1: CMD, 2: code, 0: dummy text, 10: shell

extern const char Fool[];			    // Test included binaries
extern const char John[];
extern const unsigned short MemSize;    // Approximate value of extended memory (under 4 GB)

extern "C" void main(){
	//asm volatile("1: jmp 1b");		// "entry breakpoint" (debug)
	
	GDT_Init();
	idt_install();
	isrs_install();
	irq_install();
	asm volatile ("sti");
	timer_install();
	kb_install();
    initializeMem();
    load_shell();
    //kprint(strDecapitate("print pal", strLen("print ")));
    //kprint("one\ntwo two \nthree three three \nfour four four four");
    //scrollPageUp();
    
	return;
}


