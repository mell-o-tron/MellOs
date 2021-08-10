void floppy_detect_drives();
void floppy_write_cmd(int base, char cmd);
unsigned char floppy_read_data(int base);
void floppy_check_interrupt(int base, int *st0, int *cyl);
int floppy_calibrate(int base);
int floppy_reset(int base);
