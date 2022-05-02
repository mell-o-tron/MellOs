#include "../Utils/Typedefs.h"
#include "port_io.h"
#include "../Drivers/VGA_Text.h"
#include "../CPU/Timer/timer.h"
#include "../Utils/Conversions.h"
#include "../CPU/Interrupts/irq.h"
// Adaptation of Teemu Voipio's driver:

/******************************************************************************
* Copyright (c) 2007 Teemu Voipio                                             *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL    *
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
******************************************************************************/



/********************************************* To be adapted, not tested yet *********************************************/

// standard base address of the primary floppy controller
static const int floppy_base = 0x03f0;
// standard IRQ number for floppy controllers
static const int floppy_irq = 6;
enum { floppy_motor_off = 0, floppy_motor_on, floppy_motor_wait };

enum floppy_registers {
   FLOPPY_DOR  = 2,  // digital output register
   FLOPPY_MSR  = 4,  // master status register, read only
   FLOPPY_FIFO = 5,  // data FIFO, in DMA operation for commands
   FLOPPY_CCR  = 7   // configuration control register, write only
};

// The commands of interest. There are more, but we only use these here.
enum floppy_commands {
   CMD_SPECIFY = 3,            // SPECIFY
   CMD_WRITE_DATA = 5,         // WRITE DATA
   CMD_READ_DATA = 6,          // READ DATA
   CMD_RECALIBRATE = 7,        // RECALIBRATE
   CMD_SENSE_INTERRUPT = 8,    // SENSE INTERRUPT
   CMD_SEEK = 15,              // SEEK
};

static const char *  const drive_types[8] = {
    "none",
    "360kB 5.25\"",
    "1.2MB 5.25\"",
    "720kB 3.5\"",

    "1.44MB 3.5\"",
    "2.88MB 3.5\"",
    "unknown type",
    "unknown type"
};


void floppy_detect_drives() {
   outb(0x70, 0x10);
   unsigned drives = inb(0x71);
   kprint(" - Floppy drive 0: ");
   
   kprint(drive_types[drives >> 4]);
   kprint("\n");
   kprint(" - Floppy drive 1: ");
   kprint(drive_types[drives & 0xf]);
   kprint("\n");

}

//
// The MSR byte: [read-only]
// -------------
//
//  7   6   5    4    3    2    1    0
// MRQ DIO NDMA BUSY ACTD ACTC ACTB ACTA
//
// MRQ is 1 when FIFO is ready (test before read/write)
// DIO tells if controller expects write (1) or read (0)
//
// NDMA tells if controller is in DMA mode (1 = no-DMA, 0 = DMA)
// BUSY tells if controller is executing a command (1=busy)
//
// ACTA, ACTB, ACTC, ACTD tell which drives position/calibrate (1=yes)
//
//

void floppy_motor(int base, int onoff);

void floppy_write_cmd(int base, char cmd) {
    int i; // do timeout, 60 seconds
    for(i = 0; i < 600; i++) {
        sleep(1); // sleep 10 ms
        if(0x80 & inb(base+FLOPPY_MSR)) {
            return (void) outb(base+FLOPPY_FIFO, cmd);
        }
    }
    printError("floppy_write_cmd: timeout");   
}

unsigned char floppy_read_data(int base) {

    int i; // do timeout, 60 seconds
    for(i = 0; i < 600; i++) {
        sleep(1); // sleep 10 ms
        if(0x80 & inb(base+FLOPPY_MSR)) {
            return inb(base+FLOPPY_FIFO);
        }
    }
    printError("floppy_read_data: timeout");
    return 0; // not reached
}

void floppy_check_interrupt(int base, int *st0, int *cyl) {
   
    floppy_write_cmd(base, CMD_SENSE_INTERRUPT);

    *st0 = floppy_read_data(base);
    *cyl = floppy_read_data(base);
}

// Move to cylinder 0, which calibrates the drive..
int floppy_calibrate(int base) {

    int i, st0, cyl = -1; // set to bogus cylinder

    floppy_motor(base, floppy_motor_on);

    for(i = 0; i < 10; i++) {
        // Attempt to positions head to cylinder 0
        floppy_write_cmd(base, CMD_RECALIBRATE);
        floppy_write_cmd(base, 0); // argument is drive, we only support 0

        irq_wait(floppy_irq);
        floppy_check_interrupt(base, &st0, &cyl);
       
        if(st0 & 0xC0) {
            static const char * status[] =
            { 0, "error", "invalid", "drive" };
            kprint("floppy_calibrate: status =\n"); kprint(status[st0 >> 6]);
            continue;
        }

        if(!cyl) { // found cylinder 0 ?
            floppy_motor(base, floppy_motor_off);
            return 0;
        }
    }

    kprint("floppy_calibrate: 10 retries exhausted\n");
    floppy_motor(base, floppy_motor_off);
    return -1;
}


int floppy_reset(int base) {

    outb(base + FLOPPY_DOR, 0x00); // disable controller
    outb(base + FLOPPY_DOR, 0x0C); // enable controller

    irq_wait(floppy_irq); // sleep until interrupt occurs

    {
        int st0, cyl; // ignore these here..
        floppy_check_interrupt(base, &st0, &cyl);
    }

    // set transfer speed 500kb/s
    outb(base + FLOPPY_CCR, 0x00);

    //  - 1st byte is: bits[7:4] = steprate, bits[3:0] = head unload time
    //  - 2nd byte is: bits[7:1] = head load time, bit[0] = no-DMA
    //
    //  steprate    = (8.0ms - entry*0.5ms)*(1MB/s / xfer_rate)
    //  head_unload = 8ms * entry * (1MB/s / xfer_rate), where entry 0 -> 16
    //  head_load   = 1ms * entry * (1MB/s / xfer_rate), where entry 0 -> 128
    //
    floppy_write_cmd(base, CMD_SPECIFY);
    floppy_write_cmd(base, 0xdf); /* steprate = 3ms, unload time = 240ms */
    floppy_write_cmd(base, 0x02); /* load time = 16ms, no-DMA = 0 */

    // it could fail...
    if(floppy_calibrate(base)) return -1;
    
    
    return 0;
}

//
// The DOR byte: [write-only]
// -------------
//
//  7    6    5    4    3   2    1   0
// MOTD MOTC MOTB MOTA DMA NRST DR1 DR0
//
// DR1 and DR0 together select "current drive" = a/00, b/01, c/10, d/11
// MOTA, MOTB, MOTC, MOTD control motors for the four drives (1=on)
//
// DMA line enables (1 = enable) interrupts and DMA
// NRST is "not reset" so controller is enabled when it's 1
//

static volatile int floppy_motor_ticks = 0;
static volatile int floppy_motor_state = 0;

void floppy_motor(int base, int onoff) {

    if(onoff) {
        if(!floppy_motor_state) {
            // need to turn on
            outb(base + FLOPPY_DOR, 0x1c);
            sleep(50); // wait 500 ms = hopefully enough for modern drives
        }
        floppy_motor_state = floppy_motor_on;
    } else {
        if(floppy_motor_state == floppy_motor_wait) {
            kprint("floppy_motor: strange, fd motor-state already waiting..\n");
        }
        floppy_motor_ticks = 300; // 3 seconds, see floppy_timer() below
        floppy_motor_state = floppy_motor_wait;
    }
}

void floppy_motor_kill(int base) {
    outb(base + FLOPPY_DOR, 0x0c);
    floppy_motor_state = floppy_motor_off;
}

//
// THIS SHOULD BE STARTED IN A SEPARATE THREAD.
//
//
void floppy_timer() {
    while(1) {
        // sleep for 500ms at ainb time, which gives around half
        // a second jitter, but that's hardly relevant here.
        sleep(50);
        if(floppy_motor_state == floppy_motor_wait) {
            floppy_motor_ticks -= 50;
            if(floppy_motor_ticks <= 0) {
                floppy_motor_kill(floppy_base);
            }
        }
    }
}
