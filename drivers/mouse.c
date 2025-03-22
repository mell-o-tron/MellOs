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
#include "../data_structures/circular_buffer.h"
#include "../memory/dynamic_mem.h"
#include "../GUI/mouse_handler.h"

#define MOUSE_CMD_PORT 0x64
#define MOUSE_DATA_PORT 0x60
#define MOUSE_SUCCESS 0xFA

#define MOUSE_CMD_BYTE 0xD4
#define MOUSE_CMD_ENABLE_AUX 0xA8
#define MOUSE_CMD_GET_COMPAQ_STATUS 0x20
#define MOUSE_CMD_SET_COMPAQ_STATUS 0X60
#define MOUSE_CMD_USE_DEFAULT_SETTINGS 0xF6
#define MOUSE_CMD_START_STREAMING 0xF4

#define MOUSE_WAIT_DATA 0
#define MOUSE_WAIT_SIGNAL 1

static cbuffer_t * mouse_buffer;
static MousePacket current_packet;
static uint8_t current_packet_byte = 0;

void mouse_handler(struct regs *r);
void send_mouse_command(uint8_t cmd);
void send_mouse_data(uint8_t data);
void mouse_wait(uint8_t type);
uint8_t receive_mouse_data();
void mouse_install();

void mouse_handler(struct regs *r)
{
    switch(current_packet_byte){
        case 0:
        case 1: {
            current_packet.bytes[current_packet_byte] = inb(MOUSE_DATA_PORT);
            break;
        }
        case 2: {
            current_packet.bytes[2] = inb(MOUSE_DATA_PORT);
            if (current_packet.x_overflow || current_packet.y_overflow){
                return;
            }
            // add_to_cbuffer(mouse_buffer, current_packet.bytes[0], 0);
            // add_to_cbuffer(mouse_buffer, current_packet.bytes[1], 0);
            // add_to_cbuffer(mouse_buffer, current_packet.bytes[2], 0);
            // kprint_hex(current_packet.bytes[0]);
            // kprint_hex(current_packet.bytes[1]);
            // kprint_hex(current_packet.bytes[2]);
            move_mouse(
                current_packet.x_sign == 0 ? ((int)current_packet.x_delta) : ((int)current_packet.x_delta | 0xFFFFFF00),
                current_packet.y_sign == 0 ? -((int)current_packet.y_delta) : -((int)current_packet.y_delta | 0xFFFFFF00));
                // 0);

            // kprint_dec(current_packet.x_sign);
            // kprint(" ");
            // kprint_dec(current_packet.x_sign == 0 ? ((int)current_packet.x_delta) : (((uint32_t)current_packet.x_delta) | 0xFFFFFF00));
            // kprint_dec(current_packet.bytes[0]);
            // kprint(" ");
            // kprint_dec(current_packet.y_overflow);
            // kprint(" ");
            // kprint_dec(current_packet.x_overflow);
            // kprint(" ");
            // kprint_dec(current_packet.y_sign);
            // kprint(" ");
            // kprint_dec(current_packet.x_sign);
            // kprint(" ");
            // kprint_dec(current_packet.constant);
            // kprint(" ");
            // kprint_dec(current_packet.mouse_3);
            // kprint(" ");
            // kprint_dec(current_packet.mouse_2);
            // kprint(" ");
            // kprint_dec(current_packet.mouse_1);
            // kprint("\n");
        }
    }
    current_packet_byte = (current_packet_byte + 1) % 3;
};

void send_mouse_command(uint8_t cmd){
    // First, signal that we are sending a command
    // mouse_wait(1);
    // outb(MOUSE_CMD_PORT, MOUSE_CMD_BYTE);
    // Then, send the command
    mouse_wait(1);
    outb(MOUSE_CMD_PORT, cmd);
}

void send_mouse_data(uint8_t data){
    // First, signal that we are sending data
    mouse_wait(1);
    outb(MOUSE_CMD_PORT, MOUSE_CMD_BYTE);
    // Then, send the data
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, data);
}

void mouse_wait(uint8_t type) {
    // Check if the mouse is available for writing to it
    uint32_t timeout = 100000;
    while(timeout--){
        if((inb(MOUSE_CMD_PORT) & (type == MOUSE_WAIT_DATA ? 0x1 : 0x2)) == 0) return;
    }
    return;
}

uint8_t receive_mouse_data(){
    mouse_wait(MOUSE_WAIT_DATA);
    return inb(MOUSE_DATA_PORT);
}

void mouse_install() {
    send_mouse_command(MOUSE_CMD_ENABLE_AUX);

    uint8_t response = receive_mouse_data();

    kprint_hex(response);

    send_mouse_command(MOUSE_CMD_GET_COMPAQ_STATUS);

    uint8_t compaq_status = receive_mouse_data();
    kprint_hex(compaq_status);
    
    // Set bit 2 (enable IRQ 12)
    compaq_status |= 2;
    // Unset bit 5, disable mouse clock
    // compaq_status &= ~0x20;

    kprint_hex(compaq_status);
    
    send_mouse_command(MOUSE_CMD_SET_COMPAQ_STATUS);
    outb(MOUSE_DATA_PORT, compaq_status);
    
    kprint_hex(receive_mouse_data());
    
    // 0xF6: Use default settings
    send_mouse_data(MOUSE_CMD_USE_DEFAULT_SETTINGS);
    // Acknowledge
    kprint_hex(receive_mouse_data());

    // 0xF4: Start streaming packets
    send_mouse_data(MOUSE_CMD_START_STREAMING);
    // Acknowledge
    kprint_hex(receive_mouse_data());


    mouse_buffer = kmalloc(sizeof(cbuffer_t));    
    mouse_buffer -> size = 1000;
    mouse_buffer -> top = 0;
    mouse_buffer -> bot = 0;
    mouse_buffer -> array = kmalloc(mouse_buffer -> size);

	irq_install_handler(12, mouse_handler);
}