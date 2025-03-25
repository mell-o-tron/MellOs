#include "stage_3_miniheader.h"

// https://wiki.osdev.org/Floppy_Disk_Controller#The_Floppy_Subsystem_is_Ugly
enum FloppyRegisters
{
   STATUS_REGISTER_A                = 0x3F0, // read-only
   STATUS_REGISTER_B                = 0x3F1, // read-only
   DIGITAL_OUTPUT_REGISTER          = 0x3F2,
   TAPE_DRIVE_REGISTER              = 0x3F3,
   MAIN_STATUS_REGISTER             = 0x3F4, // read-only
   DATARATE_SELECT_REGISTER         = 0x3F4, // write-only
   DATA_FIFO                        = 0x3F5,
   DIGITAL_INPUT_REGISTER           = 0x3F7, // read-only
   CONFIGURATION_CONTROL_REGISTER   = 0x3F7  // write-only
};

enum FloppyCommands
{
   READ_TRACK =                 2,	// generates IRQ6
   SPECIFY =                    3,      // * set drive parameters
   SENSE_DRIVE_STATUS =         4,
   WRITE_DATA =                 5,      // * write to the disk
   READ_DATA =                  6,      // * read from the disk
   RECALIBRATE =                7,      // * seek to cylinder 0
   SENSE_INTERRUPT =            8,      // * ack IRQ6, get status of last command
   WRITE_DELETED_DATA =         9,
   READ_ID =                    10,	// generates IRQ6
   READ_DELETED_DATA =          12,
   FORMAT_TRACK =               13,     // *
   DUMPREG =                    14,
   SEEK =                       15,     // * seek both heads to cylinder X
   VERSION =                    16,	// * used during initialization, once
   SCAN_EQUAL =                 17,
   PERPENDICULAR_MODE =         18,	// * used during initialization, once, maybe
   CONFIGURE =                  19,     // * set controller parameters
   LOCK =                       20,     // * protect controller params from a reset
   VERIFY =                     22,
   SCAN_LOW_OR_EQUAL =          25,
   SCAN_HIGH_OR_EQUAL =         29,
   MT =                         0x80,	// Multitrack
   LOCK_ON =                    0x80,	// Or'ed with the command code
   MFM =                        0x40,	// MFM
   SK =                         0x20,	// Skip deleted data
};

// DOR = Digital Output Register
enum DOR_Flags {
   MOTD           = 0x80, // Motor Drive 3
   MOTC           = 0x40, // Motor Drive 2
   MOTB           = 0x20, // Motor Drive 1
   MOTA           = 0x10, // Motor Drive 0
   IRQ            = 0x08, // IRQ/DMA mode
   RESET          = 0x04, // Clear = reset
   DSEL1          = 0x02, // Drive Select 1
   DSEL0          = 0x01  // Drive Select 0
};

enum MSR_Flags {
   RQM            = 0x80, // Request for Master
   DIO            = 0x40, // Data Input/Output
   NDMA           = 0x20, // No-DMA
   CB             = 0x10, // Command Busy
   ACTD           = 0x08, // Drive 3 is seeking
   ACTC           = 0x04, // Drive 2 is seeking
   ACTB           = 0x02, // Drive 1 is seeking
   ACTA           = 0x01  // Drive 0 is seeking
};

enum FloppyConfigureFlags {
   IMPLIED_SEEK      = 1 << 6,
   FIFO_DISABLE      = 1 << 5,
   POLLING_DISABLE   = 1 << 4
};

enum FloppyDatarates {
   RATE_500KBPS      = 0,
   RATE_300KBPS      = 1,
   RATE_250KBPS      = 2,
   RATE_1000KBPS     = 3
};

typedef struct Diskinfo {
   uint8_t drive_type;
   uint8_t max_cylinders;
   uint8_t max_sectors;
   uint8_t max_heads;
   uint8_t num_drives;
   uint8_t boot_drive_number;
} Diskinfo;

Diskinfo* diskinfo = (uint8_t*)0x5200; // This was set in stage_2 by drive_parameters in disk.asm

void wait_ready(){
   while(!(inb(MAIN_STATUS_REGISTER) & RQM &~ DIO));
}

void wait_command_done(){
   uint32_t i = 0;
   while(!(inb(MAIN_STATUS_REGISTER) & RQM) && i < 10000000){
      i++;
   }
   if (i == 10000000){
      kprint("Floppy command timed out\n");
   }
}

void wait_disk_active(uint8_t disknum){
   while(inb(MAIN_STATUS_REGISTER) & (disknum & 0x0F));
}

int version() {
   wait_ready();
   outb(DATA_FIFO, VERSION);
   wait_command_done();
   return inb(DATA_FIFO);
}

void configure(bool perp_mode){
   wait_ready();
   outb(DATA_FIFO, CONFIGURE);
   wait_ready();
   outb(DATA_FIFO, 0 /*Protocol requires empty byte*/);
   wait_ready();
   outb(DATA_FIFO, IMPLIED_SEEK | POLLING_DISABLE | 0x8 /*Threshold value*/);
   wait_ready();
   outb(DATA_FIFO, 0 /*Write precompensation. 0 = as specified by manufacturer*/);
   wait_command_done();

   if (perp_mode){   
      outb(DATA_FIFO, PERPENDICULAR_MODE);
      wait_ready();
      outb(DATA_FIFO, 1 << 2); // Enable for drive 0
      wait_ready();
   }
}

void lock(){ // Locks settings so that reset does not clear them
   wait_ready();
   outb(DATA_FIFO, LOCK | LOCK_ON);
   wait_command_done();
   uint8_t res = inb(DATA_FIFO); // Read the result
   if (res != 1 << 4) {
      kprint("Lock failed\n");
   }
}

uint16_t sense_interrupt(){
   wait_ready();
   outb(DATA_FIFO, SENSE_INTERRUPT);
   wait_command_done();
   uint8_t st0 = inb(DATA_FIFO);
   wait_ready();
   uint8_t st1 = inb(DATA_FIFO);
   wait_ready();
   return st1 << 8 | st0;
}

void set_data_rate(enum FloppyDatarates rate){
   wait_ready();
   uint8_t dsr = inb(DATARATE_SELECT_REGISTER) &~ 0x03;
   wait_ready();
   outb(DATARATE_SELECT_REGISTER, dsr | rate);
   wait_ready();
   uint8_t ccr = inb(CONFIGURATION_CONTROL_REGISTER) &~ 0x03;
   wait_ready();
   outb(CONFIGURATION_CONTROL_REGISTER, ccr | rate);
   wait_ready();
}

void specify(){
   wait_ready();
   outb(DATA_FIFO, SPECIFY);
   wait_ready();
   outb(DATA_FIFO, 8 << 4 | 15); // SRT = 8, HUT = 15
   wait_ready();
   outb(DATA_FIFO, 5 << 1 | 1); // HLT = 5, NDMA = 1
   wait_ready();
}

void set_motor(uint8_t drive, uint8_t on){
   wait_ready();
   uint8_t dor = inb(DIGITAL_OUTPUT_REGISTER) & 0x0F; // Unset motor bits
   wait_ready();
   dor = dor | (on << (drive + 4));
   outb(DIGITAL_OUTPUT_REGISTER, dor);
   wait_ready();
}

void set_dsel(uint8_t drive){
   wait_ready();
   uint8_t dor = inb(DIGITAL_OUTPUT_REGISTER) &~ 0x03; // Unset drive select bits
   wait_ready();
   dor = dor | (drive & 0x03);
   outb(DIGITAL_OUTPUT_REGISTER, dor);
   wait_ready();
}

void drive_select(uint8_t drive, enum FloppyDatarates rate){
   set_data_rate(rate);
   specify();
   set_motor(drive, 1);
   set_dsel(drive);
}

void drive_reset(){
   uint8_t status = inb(DIGITAL_OUTPUT_REGISTER) &~ IRQ;
   // outb(DIGITAL_OUTPUT_REGISTER, (status &~ RESET) &~ IRQ);
   outb(DIGITAL_OUTPUT_REGISTER, 0);
   wait_command_done();
   outb(DIGITAL_OUTPUT_REGISTER, status);
   wait_command_done();

   char buf[256];
   tostring(status, 16, buf);
   // kprint("Drive reset status: ");
   // kprint(buf);
   // kprint("\n");

   // for (int i = 0; i < 4; i++){
   //    uint8_t res = sense_interrupt() & 0x0F;
   //    tostring(res, 16, buf);
   //    kprint("Drive reset result: ");
   //    kprint(buf);
   //    kprint("\n");
   // }
}

void recalibrate(uint8_t disknum){
   wait_ready();
   outb(DATA_FIFO, RECALIBRATE);
   wait_ready();
   outb(DATA_FIFO, disknum);
   wait_disk_active(disknum);
   uint8_t res = sense_interrupt() >> 0x0F;
   
   char buf[256];
   tostring(res, 16, buf);
   kprint("Recalibrate result: ");
   kprint(buf);
   kprint("\n");
}

void seek(uint8_t disknum, uint8_t cylinder, uint8_t head){
   wait_ready();
   outb(DATA_FIFO, SEEK);
   wait_ready();
   outb(DATA_FIFO, head << 2 | disknum);
   wait_ready();
   outb(DATA_FIFO, cylinder);
   wait_disk_active(disknum);
   uint8_t res = sense_interrupt() >> 0x0F;

   char buf[256];
   tostring(res, 16, buf);
   kprint("Seek result: ");
   kprint(buf);
   kprint("\n");
}

void read(uint8_t drive, uint8_t cylinder, uint8_t head, uint8_t sector){
   wait_ready();
   outb(DATA_FIFO, READ_DATA | MT | MFM);
   wait_ready();
   outb(DATA_FIFO, head << 2 | drive);
   wait_ready();
   outb(DATA_FIFO, cylinder);
   wait_ready();
   outb(DATA_FIFO, head);
   wait_ready();
   outb(DATA_FIFO, sector);
   wait_ready();
   outb(DATA_FIFO, 2); // Sector size
   wait_ready();
   outb(DATA_FIFO, 1);
   wait_ready();
   outb(DATA_FIFO, 0x1B); // "GAP1 default size"
   wait_ready();
   outb(DATA_FIFO, 0xFF); // Sector size

   uint8_t buff[1024];
   for (int i = 0; i < 1024; i++){
      uint8_t msr = inb(MAIN_STATUS_REGISTER);
      while(!(msr & (RQM | NDMA))){
         msr = inb(MAIN_STATUS_REGISTER);
      }
      buff[i] = inb(DATA_FIFO);
   }

   wait_ready();
   uint8_t st0 = inb(DATA_FIFO);
   wait_ready();
   uint8_t st1 = inb(DATA_FIFO);
   wait_ready();
   uint8_t st2 = inb(DATA_FIFO);
   wait_ready();
   uint8_t end_cyl = inb(DATA_FIFO);
   wait_ready();
   uint8_t end_hd = inb(DATA_FIFO);
   wait_ready();
   uint8_t end_sec = inb(DATA_FIFO);
   wait_ready();
   uint8_t two = inb(DATA_FIFO);
   
   wait_ready();
   char buf[256];
   tostring(st0, 16, buf);
   kprint("st0: ");
   kprint(buf);
   kprint("\n");
   tostring(st1, 16, buf);
   kprint("st1: ");
   kprint(buf);
   kprint("\n");
   tostring(st2, 16, buf);
   kprint("st2: ");
   kprint(buf);
   kprint("\n");
   tostring(end_cyl, 16, buf);
   kprint("end_cyl: ");
   kprint(buf);
   kprint("\n");
   tostring(end_hd, 16, buf);
   kprint("end_hd: ");
   kprint(buf);
   kprint("\n");
   tostring(end_sec, 16, buf);
   kprint("end_sec: ");
   kprint(buf);
   kprint("\n");
   tostring(two, 16, buf);
   kprint("two: ");
   kprint(buf);
   kprint("\n");

   for (int i = 0; i < 1024; i++){
      tostring(buff[i], 16, buf);
      kprint(buf);
      kprint(" ");
   }
}


void init_floppy(){
   char buf[256];
   tostring(version(), 16, buf);
   // kprint("Floppy version: ");
   // kprint(buf);
   // kprint("\n");
   configure(true);
   lock();
   drive_reset();
   // kprint("Floppy initialized\n");
   drive_select(diskinfo->boot_drive_number, RATE_1000KBPS);

   uint32_t timeout = 100000000;
   while(timeout--);

   tostring(diskinfo->boot_drive_number, 16, buf);
   kprint("Disk num: ");
   kprint(buf);
   kprint("\n");

   recalibrate(diskinfo->boot_drive_number);
   recalibrate(diskinfo->boot_drive_number);

   // seek(diskinfo->boot_drive_number, 0, 0);
   read(diskinfo->boot_drive_number, 0, 0, 1);
   // timeout = 100000000;
   // while(timeout--);
   // seek(diskinfo->boot_drive_number, 0, 0);
   // read(diskinfo->boot_drive_number, 0, 0, 1);
   while(1);
}