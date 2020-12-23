#include "../UsefulStuff/Typedefs.h"
#include "../Drivers/port_io.h"
#include "../Drivers/VGA_Text.h"
#include "../UsefulStuff/Conversions.h"
#include <irq.h>
int timer_ticks = 0;
int seconds = 0;


void timer_handler(struct regs *r)
{
    /* Increment our 'tick count' */
    timer_ticks++;


    if (timer_ticks % 18 == 0)
    {
    	seconds++;
	print(toString(seconds, 10));
    }
}


void timer_install()
{
    irq_install_handler(0, timer_handler);
}
