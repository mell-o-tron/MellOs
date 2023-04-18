#include "../Utils/Typedefs.h"
#include "../Drivers/VGA_Text.h"
#include "../Utils/string.h"
#include "../Utils/Conversions.h"
#include "../Drivers/port_io.h"
#include "../Shell/shell.h"
#include "../Misc/colors.h"
#include "../Drivers/Keyboard.h"
#include "../CPU/Timer/timer.h"
extern int seconds;

void time(const char* s){
    outb(0x70, 0x0A);
    while(inb(0x71) & 0x80);
    
    outb(0x70,0x00);
    unsigned char svalue = inb(0x71);
    svalue = ((svalue / 16) * 10) + (svalue & 0xf);
    outb(0x70,0x02);
    unsigned char mvalue = inb(0x71);
    mvalue = ((mvalue / 16) * 10) + (mvalue & 0xf);
    outb(0x70,0x04);
    unsigned char hvalue = inb(0x71);
    hvalue = ((hvalue / 16) * 10) + (hvalue & 0xf);
    outb(0x70,0x07);
    unsigned char dvalue = inb(0x71);
    dvalue = ((dvalue / 16) * 10) + (dvalue & 0xf);
    outb(0x70,0x08);
    unsigned char movalue = inb(0x71);
    movalue = ((movalue / 16) * 10) + (movalue & 0xf);
    outb(0x70,0x09);
    unsigned char yvalue = inb(0x71);
    yvalue = ((yvalue / 16) * 10) + (yvalue & 0xf);
    kprint("CMOS time:  ");
    kprint(toString(hvalue,10));
    kprint(":");
    kprint(toString(mvalue,10));
    kprint(":");
    kprint(toString(svalue,10));
    kprint("   ");
    kprint(toString(movalue,10));
    kprint("/");
    kprint(toString(dvalue,10));
    kprint("/");
    kprint(toString(yvalue,10));
}
void boottime(){
	kprint("Uptime:");
	kprint(toString(seconds,10));
	kprint("\nPress Space to continue...");
	await(0x39);
	kprint("\nContinuing...");
}
