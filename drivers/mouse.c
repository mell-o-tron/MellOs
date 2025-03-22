#include "mouse.h"
#include "../utils/typedefs.h"

#include "port_io.h"
#include "../cpu/interrupts/irq.h"
#include "../cpu/interrupts/idt.h"
#include "../utils/conversions.h"
#include "../cpu/timer/timer.h"
#ifdef VGA_VESA
#include "vesa/vesa_text.h"
#else
#include "vga_text.h"
#endif

typedef union MousePacket {
    struct {
        uint8_t y_overflow: 1;
        uint8_t x_overflow: 1;
        uint8_t y_sign:     1;
        uint8_t x_sign:     1;
        uint8_t constant:   1;
        uint8_t mouse_3:    1;
        uint8_t mouse_2:    1;
        uint8_t mouse_1:    1;

        uint8_t x_delta:    8;
        uint8_t y_delta:    8;
    };
    uint8_t bytes[3];
} MousePacket;

void mouse_handler(struct regs *r)
{
    // while(1);
    MousePacket packet;
    // packet.bytes[0] = inb(0x60);
    // packet.bytes[1] = inb(0x60);
    // packet.bytes[2] = inb(0x60);

    // kprint("Hi");
    // char buf[10];
    // tostring(packet.y_delta, 16, buf);
    // kprint(buf);
};

void mouse_install()
{   
    // keyboard_buffer = kmalloc(sizeof(cbuffer_t));
    // action_buffer =   kmalloc(sizeof(cbuffer_t));
    
    // keyboard_buffer -> size = 1000;
    // keyboard_buffer -> top = 0;
    // keyboard_buffer -> bot = 0;
    // keyboard_buffer -> array = kmalloc(keyboard_buffer -> size);
    
    // action_buffer -> size = 1000;
    // action_buffer -> top = 0;
    // action_buffer -> bot = 0;
    // action_buffer -> array = kmalloc(action_buffer -> size);

    // 0xD4: Send Command
	// 0xA8: Enable auxiliary device
    outb(0x64, 0xD4);
    outb(0x64, 0xA8);

    sleep(3);
    uint8_t response = inb(0x60);

    kprint_hex(response);
    kprint_hex(response);
    kprint_hex(response);

    // 0x20: Get compaq status
    outb(0x64, 0xD4);
    outb(0x64, 0x20);

    sleep(3);
    uint8_t compaq_status = inb(0x60);
    kprint_hex(compaq_status);
    kprint_hex(compaq_status);
    
    compaq_status |= 2;
    compaq_status &= ~0x20;

    kprint_hex(compaq_status);
    kprint_hex(compaq_status);
    
    // 0x60: Get compaq status
    outb(0x64, 0xD4);
    outb(0x64, 0x60);
    outb(0x60, compaq_status);
    
    sleep(30);
    kprint_hex(inb(0x60));
    kprint_hex(inb(0x60));
    sleep(10);
    
    // 0xF6: Use default settings
    outb(0x64, 0xD4);
    outb(0x64, 0xF6);
    sleep(3);

    // 0xF4: Start streaming packets
    outb(0x64, 0xD4);
    outb(0x64, 0xF4);
    while(1);

	irq_install_handler(12, mouse_handler);
}