// Written by KingUndeadCodes (https://www.github.com/KingUndeadCodes/)
// Check out my OS (https://www.github.com/KingUndeadCodes/Pumpkin-OS)

#include "../CPU/Timer/timer.h"

static void PlaySound(unsigned int nFrequence) {
    unsigned char x;
    unsigned char y;
    x = 1193180 / nFrequence;
    outb(0x43, 0xb6);
    outb(0x42, (unsigned char)(x));
    outb(0x42, (unsigned char)(x >> 8));
    y = inb(0x61);
    if (y != (y | 3)) {
 	    outb(0x61, y | 3);
    }
}

static void Quiet() {
    unsigned char x = inb(0x61) & 0xFC;
    outb(0x61, x);
}

void beep(unsigned int freq, unsigned int time) {
    if (!freq) freq = 950;
    if (!time) time = 18;
    if (time == 0) {
        kprint("beep warning: 'time' played is zero.");
        return;
    }
    PlaySound(freq);
    sleep(time);
    Quiet();
}
