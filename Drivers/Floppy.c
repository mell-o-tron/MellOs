#include "Floppy.h"
#include <port_io.h>
#include "../Drivers/VGA_Text.h"
#include "../CPU/Timer/timer.h"
#include "../Utils/Conversions.h"
#include "../CPU/Interrupts/irq.h"
#include "../Memory/mem.h"
#include "../Utils/string.h"
#include "ISA_DMA.h"
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






// standard IRQ number for floppy controllers
static const int floppy_irq = 6;

enum FloppyRegisters
{
    FLOPPY_STATUS_REGISTER_A                = 0x3F0, // read-only
    FLOPPY_FLOPPY_STATUS_REGISTER_B         = 0x3F1, // read-only
    FLOPPY_DIGITAL_OUTPUT_REGISTER          = 0x3F2,
    FLOPPY_TAPE_DRIVE_REGISTER              = 0x3F3,
    FLOPPY_MAIN_STATUS_REGISTER             = 0x3F4, // read-only
    FLOPPY_DATARATE_SELECT_REGISTER         = 0x3F4, // write-only
    FLOPPY_DATA_FIFO                        = 0x3F5,
    FLOPPY_DIGITAL_INPUT_REGISTER           = 0x3F7, // read-only
    FLOPPY_CONFIGURATION_CONTROL_REGISTER   = 0x3F7  // write-only
};


enum FloppyCommands
{
    FLOPPY_READ_TRACK =                 2,	// generates IRQ6
    FLOPPY_SPECIFY =                    3,      // * set drive parameters
    FLOPPY_SENSE_DRIVE_STATUS =         4,
    FLOPPY_WRITE_DATA =                 5,      // * write to the disk
    FLOPPY_READ_DATA =                  6,      // * read from the disk
    FLOPPY_RECALIBRATE =                7,      // * seek to cylinder 0
    FLOPPY_SENSE_INTERRUPT =            8,      // * ack IRQ6, get status of last command
    FLOPPY_WRITE_DELETED_DATA =         9,
    FLOPPY_READ_ID =                    10,	// generates IRQ6
    FLOPPY_READ_DELETED_DATA =          12,
    FLOPPY_FORMAT_TRACK =               13,     // *
    FLOPPY_DUMPREG =                    14,
    FLOPPY_SEEK =                       15,     // * seek both heads to cylinder X
    FLOPPY_VERSION =                    16,	// * used during initialization, once
    FLOPPY_SCAN_EQUAL =                 17,
    FLOPPY_PERPENDICULAR_MODE =         18,	// * used during initialization, once, maybe
    FLOPPY_CONFIGURE =                  19,     // * set controller parameters
    FLOPPY_LOCK =                       20,     // * protect controller params from a reset
    FLOPPY_VERIFY =                     22,
    FLOPPY_SCAN_LOW_OR_EQUAL =          25,
    FLOPPY_SCAN_HIGH_OR_EQUAL =         29
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

enum FLOPPYSpeeds{
    KB500 = 0,
    MB1 = 3
};


//
// The MSR byte: [read-only]
// -------------
//
//  7   6   5    4    3    2    1    0
// RQM DIO NDMA CB ACTD ACTC ACTB ACTA
//
// MRQ is 1 when FIFO is ready (test before read/write)
// DIO tells if controller expects write (1) or read (0)
//
// NDMA tells if controller is in DMA mode (1 = no-DMA, 0 = DMA)
// CB(BUSY) tells if controller is executing a command (1=busy)
//
// ACTA, ACTB, ACTC, ACTD tell which drives position/calibrate (1=yes)
//
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

/*
 * Data rate   value   Drive Type
 * 1Mbps        3       2.88M
 * 500Kbps      0       1.44M, 1.2M
 */


/*
 * Floppy Util
 */





void lba_2_chs_f(int sectors_per_track, uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector);
void lba_2_chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector);

void lba_2_chs_f(int sectors_per_track, uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector)
{
    *cyl    = lba / (2 * sectors_per_track);
    *head   = ((lba % (2 * sectors_per_track)) / 18);
    *sector = ((lba % (2 * sectors_per_track)) % sectors_per_track + 1);

}

void lba_2_chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector)
{
    lba_2_chs_f(18, lba, cyl, head, sector);
}


/*
 * https://forum.osdev.org/viewtopic.php?t=13538
 */
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

/*
 * https://wiki.osdev.org/CMOS#Register_0x10
 * https://forum.osdev.org/viewtopic.php?t=13538
 */
uint8_t get_drive_type(){
    // ask CMOS for floppy drive type
    outb(0x70, 0x10);
    uint8_t drives = inb(0x71);
    if(drives >> 4 == 0){
        return drives & 0xf;
    }
    return drives >> 4;

}

/*
 * https://wiki.osdev.org/Floppy_Disk_Controller#The_Proper_Way_to_issue_a_command
 */
void floppy_write_cmd(char cmd) {
    int i; // do timeout, 60 seconds
    for(i = 0; i < 600; i++) {
        sleep(1); // sleep 10 ms
        if(0x80 & inb(FLOPPY_MAIN_STATUS_REGISTER)) {
            return (void) outb(FLOPPY_DATA_FIFO, cmd);
        }
    }
    printError("floppy_write_cmd: timeout");
}

/*
 * https://wiki.osdev.org/Floppy_Disk_Controller#The_Proper_Way_to_issue_a_command
 */
unsigned char floppy_read_data() {

    int i; // do timeout, 60 seconds
    for(i = 0; i < 600; i++) {
        sleep(1); // sleep 10 ms
        if(0x80 & inb(FLOPPY_MAIN_STATUS_REGISTER)) {
            return inb(FLOPPY_DATA_FIFO);
        }
    }
    printError("floppy_read_data: timeout");
    return 0; // not reached
}


// Floppy Command Definitions

void floppy_configure(bool implied_seek, bool FIFO, bool drive_polling_mode, int threshold);
void floppy_lock();
void floppy_reset(bool firstTime);
void floppy_recalibrate(uint8_t  drive);
void floppy_sense_interrupt(uint8_t *st0, uint8_t *cyl);
void specify();
void drive_select(int drive);
void floppy_rw_command(int drive, int head, int cyl, int sect, int EOT, uint8_t *st0, uint8_t *st1, uint8_t *st2,
                       int *headResult, int *cylResult, int *sectResult, int command);


// Floppy Commands

/*
 * https://wiki.osdev.org/Floppy_Disk_Controller#Reinitialization
 */
int floppy_init(){
    floppy_write_cmd(FLOPPY_VERSION);
    if(floppy_read_data() != 0x90)
        return -1;

    floppy_configure(true, true, false, 8);
    floppy_lock();
    floppy_reset(true);

    // floppy_recalibrate all drives
    for(int i = 0; i < 4; i++){
        floppy_recalibrate(i);
    }

    return 0;
}

/*
 * https://wiki.osdev.org/Floppy_Disk_Controller#Drive_Selection
 */
void drive_select(int drive){
    outb(FLOPPY_CONFIGURATION_CONTROL_REGISTER, 0); // This is usually correct, even tho it changes if not using 1.44Mb drive.
    specify();

    // Select drive in DOR and turn on its motor
    uint8_t DOR = inb(FLOPPY_DIGITAL_OUTPUT_REGISTER);
    // turn off all motors | select drive | turn on drive's motor
    DOR = (DOR & 0xC) | (drive | (1 << (4 + drive)));
    outb(FLOPPY_DIGITAL_OUTPUT_REGISTER, DOR);
}

/*
 * https://wiki.osdev.org/Floppy_Disk_Controller#Specify
 */
void specify(){
    /*
     * According to the OsDev wiki, these values should change according
     * to failed operation statistics for performance.
     * but, because no1 uses floppies, performance isn't that important
     * so, we'll just use very safe values
     */
    int SRT = 8;
    int HLT = 5;
    int HUT = 0;

    floppy_write_cmd(FLOPPY_SPECIFY);
    floppy_write_cmd(SRT << 4 | HUT);
    floppy_write_cmd(HLT << 1 | 0);


}

/*
 * https://wiki.osdev.org/Floppy_Disk_Controller#Configure
 */
void floppy_configure(bool implied_seek, bool FIFO, bool drive_polling_mode, int threshold){
    floppy_write_cmd(FLOPPY_CONFIGURE);
    floppy_write_cmd(0); // IDK why this even exists, it is always 0
    floppy_write_cmd(implied_seek << 6 | !FIFO << 5 | !drive_polling_mode << 4 | threshold - 1);
    floppy_write_cmd(0); // pre-compensation value - should always be 0

}

/*
 * https://wiki.osdev.org/Floppy_Disk_Controller#Lock
 */
void floppy_lock(){
    floppy_write_cmd(FLOPPY_LOCK);
    floppy_read_data();
}

/*
 * https://wiki.osdev.org/Floppy_Disk_Controller#Recalibrate
 */
void floppy_recalibrate(uint8_t drive){
    floppy_write_cmd(FLOPPY_RECALIBRATE);
    floppy_write_cmd(drive);

    irq_wait(floppy_irq);
    uint8_t st0 = 0;
    uint8_t cyl = 0;
    floppy_sense_interrupt(&st0, &cyl);

    if(!(st0 & 0x20))
        floppy_recalibrate(drive);
}


/*
 * https://wiki.osdev.org/Floppy_Disk_Controller#Sense_Interrupt
 */
void floppy_sense_interrupt(uint8_t *st0, uint8_t *cyl){
    floppy_write_cmd(FLOPPY_SENSE_INTERRUPT);

    uint8_t RQM;
    while(true){
        RQM = inb(FLOPPY_MAIN_STATUS_REGISTER) & 0x80;
        if(RQM)
            break;
    }

    *st0 = floppy_read_data();
    *cyl = floppy_read_data();

}

/*
 * https://wiki.osdev.org/Floppy_Disk_Controller#Controller_Reset
 */
void floppy_reset(bool firstTime){
    uint8_t DOR = inb(FLOPPY_DIGITAL_OUTPUT_REGISTER);
    outb(FLOPPY_DIGITAL_OUTPUT_REGISTER, 0);
    sleep(10);
    outb(FLOPPY_DIGITAL_OUTPUT_REGISTER, DOR & 0x8);
    if(!firstTime){ // check if IRQs were enabled
        irq_wait(floppy_irq);
    }
}



/*
 * https://wiki.osdev.org/Floppy_Disk_Controller#Read.2FWrite
 */

int floppy_write(int drive, uint32_t lba, void* address, uint16_t count){
    initFloppyDMA((uint32_t) address, count);

    drive_select(drive);

    uint16_t cyl;
    uint16_t head;
    uint16_t sector;
    lba_2_chs(lba, &cyl, &head, &sector);

    int EOT = 19;

    uint8_t st0;
    uint8_t st1;
    uint8_t st2;
    int cylOut;
    int headOut;
    int sectOut;


    for(int i = 0; i < 20; i++){

        prepare_for_floppyDMA_write();

        floppy_rw_command(drive, head, cyl, sector, EOT, &st0, &st1, &st2, &headOut, &cylOut, &sectOut, FLOPPY_WRITE_DATA);

        int error = 0;

        if(st0 >> 6 == 2){error = 1;}
        if(st1 & 0x80) {error = 1;}
        if(st0 & 0x08) {error = 1;}
        if(st0 >> 6 == 3){error = 1;}
        if(st1 & 0x20) {error = 1;}
        if(st1 & 0x10) {error = 1;}
        if(st1 & 0x04) {error = 1;}
        if((st1|st2) & 0x01) {error = 1;}
        if(st2 & 0x40) {error = 1;}
        if(st2 & 0x20) {error = 1;}
        if(st2 & 0x10) {error = 1;}
        if(st2 & 0x04) {error = 1;}
        if(st2 & 0x02) {error = 1;}
        if(st1 & 0x02) {error = 2;}
        if(!error){
            return 0;
        }
        if(error > 1){
            return -2;
        }

    }
    return -1;

}

int floppy_read(int drive, uint32_t lba, void* address, uint16_t count){
    initFloppyDMA((uint32_t) address, count);

    drive_select(drive);

    uint16_t cyl;
    uint16_t head;
    uint16_t sector;
    lba_2_chs(lba, &cyl, &head, &sector);

    int EOT = 19;

    uint8_t st0;
    uint8_t st1;
    uint8_t st2;
    int cylOut;
    int headOut;
    int sectOut;


    for(int i = 0; i < 20; i++){

        prepare_for_floppyDMA_read();

        floppy_rw_command(drive, head, cyl, sector, EOT, &st0, &st1, &st2, &headOut, &cylOut, &sectOut, FLOPPY_READ_DATA);

        int error = 0;

        if(st0 >> 6 == 2){error = 1;}
        if(st1 & 0x80) {error = 1;}
        if(st0 & 0x08) {error = 1;}
        if(st0 >> 6 == 3){error = 1;}
        if(st1 & 0x20) {error = 1;}
        if(st1 & 0x10) {error = 1;}
        if(st1 & 0x04) {error = 1;}
        if((st1|st2) & 0x01) {error = 1;}
        if(st2 & 0x40) {error = 1;}
        if(st2 & 0x20) {error = 1;}
        if(st2 & 0x10) {error = 1;}
        if(st2 & 0x04) {error = 1;}
        if(st2 & 0x02) {error = 1;}
        if(st1 & 0x02) {error = 2;}
        if(!error){
            return 0;
        }
        if(error > 1){
            return -2;
        }

    }
    return -1;

}


void floppy_rw_command(int drive, int head, int cyl, int sect, int EOT, uint8_t *st0, uint8_t *st1, uint8_t *st2,
                       int *headResult, int *cylResult, int *sectResult, int command) {
    int MT = 0x80; // set to 0x80 to enable multi-track, or 0 to disable
    int MFM = 0x40; //set to 0x40 to enable magnetic-encoding-mode, or 0 to disable. According to the wiki this should always be on

    // Read command = MT bit | MFM bit | 0x6
    floppy_write_cmd( MFM | MT | command);

    // First parameter byte = (head number << 2) | drive number (the drive number must match the currently selected drive!)
    floppy_write_cmd((head << 2) | drive);

    // Second parameter byte = cylinder number
    floppy_write_cmd(cyl);

    // Third parameter byte = head number (yes, this is a repeat of the above value)
    floppy_write_cmd(head);

    // Fourth parameter byte = starting sector number
    floppy_write_cmd(sect);

    // Fifth parameter byte = 2 (all floppy drives use 512bytes per sector)
    floppy_write_cmd(2);

    // Sixth parameter byte = EOT (end of track, the last sector number on the track)
    floppy_write_cmd(EOT);

    // Seventh parameter byte = 0x1b (GAP1 default size)
    floppy_write_cmd(0x1b);

    // Eighth parameter byte = 0xff (all floppy drives use 512bytes per sector)
    floppy_write_cmd(0xff);

    uint8_t RQM;
    uint8_t MSR;
    while(true){
        MSR = inb(FLOPPY_MAIN_STATUS_REGISTER);
        RQM = (MSR & 0x80) > 1;
        //kprint(toString(MSR, 2));
        sleep(10);
        if(RQM)
            break;
    }

    // First result byte = st0 status register
    *st0 = floppy_read_data();

    // Second result byte = st1 status register
    *st1 = floppy_read_data();

    // Third result byte = st2 status register
    *st2 = floppy_read_data();

    // Fourth result byte = cylinder number
    *cylResult = floppy_read_data();


    // Fifth result byte = ending head number
    *headResult = floppy_read_data();

    // Sixth result byte = ending sector number
    *sectResult = floppy_read_data();

    // Seventh result byte = 2
    floppy_read_data();
}
