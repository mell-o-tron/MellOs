// Written by KingUndeadCodes (https://www.github.com/KingUndeadCodes/)
// Check out my OS (https://www.github.com/KingUndeadCodes/Pumpkin-OS)

#ifdef AUDIO_ENABLED

#include "drivers/pc_speaker.h"

#include "utils/typedefs.h"
#include "cpu/timer/timer.h"
#include "drivers/port_io.h"
#include "utils/format.h"
#include "utils/bit_manip.h"

static void PlaySound(unsigned int nFrequence) {
    uint32_t divisor = 1193180u / (nFrequence ? nFrequence : 1u);
    outb(0x43, 0xb6);
    outb(0x42, LO_8(divisor));
    outb(0x42, HI_8(divisor));
    uint8_t y = inb(0x61);
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
        printf("beep warning: 'time' played is zero.");
        return;
    }
    PlaySound(freq);
    sleep(time);
    Quiet();
}

#endif