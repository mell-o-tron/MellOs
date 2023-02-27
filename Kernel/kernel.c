/*********************
* TEXT MODE: 0xB8000 *
* GR.  MODE: 0xA000  *
*********************/

//IMPLEMENT STACK SMASHING PROTECTOR!!!!


#include "../Utils/Typedefs.h"
#include "../Misc/colors.h"
#include "../Drivers/VGA_Text.h"
#include "../Utils/Conversions.h"
#include "../Utils/string.h"
#include "../Memory/mem.h"
#include "../Memory/cmos_memsize.h"
#include "../Memory/Paging/paging.h"
#include "../CPU/Interrupts/idt.h"
#include "../CPU/Interrupts/isr.h"
#include "../CPU/Interrupts/irq.h"
#include "../CPU/Timer/timer.h"
#include "../CPU/GDT/gdt.h"
#include "../Drivers/Keyboard.h"
#include "../Drivers/Floppy.h"
#include "../Drivers/port_io.h"
#include "../Utils/dataStructures.h"
#include "../Shell/shell.h"

#include "../Drivers/Disk.h"
#include "../Drivers/PCSpeaker.h"

volatile int curMode = 0;					        // Modes:	0: dummy text, 10: shell


extern const char Fool[];			    // Test included binaries
extern const char KPArt[];
extern const unsigned short MemSize;    // Approximate value of extended memory (under 4 GB)
extern int curColor;
char ker_tty[4000];

// This function has to be self contained - no dependencies to the rest of the kernel!
extern  void kpanic(struct regs *r){
	#define ERRCOL 0x47 // Lightgrey on Lightred
	#define VGAMEM (unsigned char*)0xB8000;

	char panicscreen[4000];
	const char* components[] = {
		KPArt,
        "Exception message: ",
		exception_messages[r->int_no],
	};
	int psidx = 0; //Index to access panicscreen
	int idx = 0;

	for(int x = 0; x < sizeof(components)/sizeof(char*); x++){
		idx = 0;
		while(components[x][idx] != 0){
			if(components[x][idx] == '\n'){
				do{
					panicscreen[psidx] = ' ';
					psidx++;
				} while((psidx+1) % 80 != 0);
			} else panicscreen[psidx] = components[x][idx];
			psidx++;
			idx++;
		}
	}

	unsigned char *write = VGAMEM;

	for(int i = 0; i < 4000; i++){
		*write++ = panicscreen[i];
		*write++ = ERRCOL;
	}

	// Disables the flashing cursor because that's annoying imo
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);

	for(;;);
}


extern  void main(){
	//asm volatile("1: jmp 1b");		// "entry breakpoint" (debug)
	
	init_paging();
	GDT_Init();
	idt_install();
	isrs_install();
	irq_install();
	asm volatile ("sti");
	timer_install();
	initializeMem();


	clear_tty(DEFAULT_COLOR, ker_tty);
	display_tty(ker_tty);
	clear_tty(DEFAULT_COLOR, ker_tty);
	display_tty_line(ker_tty, 1);
	int curColor = DEFAULT_COLOR;
	SetCursorPosRaw(0);
	kprintCol(Fool, DEFAULT_COLOR);
	SetCursorPosRaw(1920);
	kb_install();
	
	return;
}
