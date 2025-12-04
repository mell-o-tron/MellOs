/* RTC driver */

#include "rtc.h"
#include "port_io.h"

#include "mellos/kernel/kernel_stdio.h"

uint8_t bcd_to_bin(uint8_t bcd) {
    return (bcd & 0x0F) + ((bcd >> 4) * 10);
}

int is_leap_year(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

uint32_t days_in_month(int year, int month) {
    static const uint8_t days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (month == 1 && is_leap_year(year)) return 29;
    return days[month];
}

// Determines the day of the week using the Doomsday algorithm
const char* doomsday_algorithm(int year, int month, int day) {
    static const char* weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    static const int doomsday_months[] = {3, 0, 3, 2, 5, 0, 3, 5, 1, 3, 6, 1}; // Offsets for each month

    int century_anchor[] = {2, 0, 5, 3}; // Anchor days for centuries (e.g., 1900s -> 3, 2000s -> 2)
    int century = year / 100;
    int year_in_century = year % 100;

    // Calculate the anchor day for the century
    int anchor_day = century_anchor[century % 4];

    // Calculate the doomsday for the year
    int doomsday = (anchor_day + year_in_century + (year_in_century / 4)) % 7;

    // Adjust for leap years
    if (is_leap_year(year) && (month == 0 || month == 1)) {
        day--;
    }

    // Calculate the day of the week
    int weekday = (doomsday + day - doomsday_months[month]) % 7;
    if (weekday < 0) {
        weekday += 7;
    }

    return weekdays[weekday];
}

// Reads and prints the current time from the RTC
// Originally written by @DragonIvanRussia
void print_time(const char* s){
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

    kprintf("RTC time:  %02u:%02u:%02u %02u/%02u/20%02u, today is a %s!\n", hvalue, mvalue, svalue, dvalue, movalue, yvalue, doomsday_algorithm(2000 + yvalue, movalue - 1, dvalue));
    // printf("POSIX time: %u\n", posix_time());
}

uint32_t posix_time() {
    outb(0x70, 0x0A);
    while(inb(0x71) & 0x80);
    
    outb(0x70,0x00);
    unsigned char svalue = bcd_to_bin(inb(0x71));

    outb(0x70,0x02);
    unsigned char mvalue = bcd_to_bin(inb(0x71));

    outb(0x70,0x04);
    unsigned char hvalue = bcd_to_bin(inb(0x71));

    outb(0x70,0x07);
    unsigned char dvalue = bcd_to_bin(inb(0x71));

    outb(0x70,0x08);
    unsigned char movalue = bcd_to_bin(inb(0x71)) - 1; // Months are 0-11

    outb(0x70,0x09);
    unsigned char yvalue = bcd_to_bin(inb(0x71));

    // Calculate days since 1970-01-01
    uint32_t days = 0;
    for (int y = 1970; y < 2000 + yvalue; y++) {
        days += is_leap_year(y) ? 366 : 365;
    }
    for (int m = 0; m < movalue; m++) {
        days += days_in_month(yvalue, m);
    }
    days += dvalue - 1;

    // Convert to seconds
    uint32_t timestamp = days * 86400 + hvalue * 3600 + mvalue * 60 + svalue;
    return timestamp;
}