#pragma once
#include "autoconf.h"
#ifdef CONFIG_GFX_VESA
#include "../../cpu/include/cpu/irq.h"
#include "stdint.h"
#include "stdbool.h"

typedef union MousePacket {
    struct {
        uint8_t mouse_1:    1;
        uint8_t mouse_2:    1;
        uint8_t mouse_3:    1;
        uint8_t constant:   1;
        uint8_t x_sign:     1;
        uint8_t y_sign:     1;
        uint8_t x_overflow: 1;
        uint8_t y_overflow: 1;

        uint8_t x_delta:    8;
        uint8_t y_delta:    8;
    };
    uint8_t bytes[3];
} MousePacket;

void mouse_handler(struct regs *r);
void mouse_install();
#endif