/** MellOS UART driver
 *  With much help from https://wiki.osdev.org/Serial_Ports
 **/

#include "uart.h"
#include "utils/typedefs.h"
#include "drivers/port_io.h"
#include "kernel/kernel.h"
#include "utils/string.h"

static const uint16_t COM1_PORT = 0x3F8;

void uart_init() {
    outb(COM1_PORT + 1, 0x00);    // Disable all interrupts
    outb(COM1_PORT + 3, 0x80);    // Enable Divisor Latch Access Bit (allows setting baud rate)
    outb(COM1_PORT + 0, 0x03);    // Set divisor (lo byte)
    outb(COM1_PORT + 1, 0x00);    // Set divisor (hi byte)
    outb(COM1_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(COM1_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(COM1_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(COM1_PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(COM1_PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

    if (inb(COM1_PORT + 0) != 0xAE) {
        // Serial returned a different byte, probably faulty
        kpanic_message("Faulty serial port, returned wrong byte");
    }

    // Set the serial port in normal mode, (no loopback, IRQs on, OUT1 and OUT2 enabled)
    outb(COM1_PORT + 4, 0x0F);
}

int serial_received() {
   return inb(COM1_PORT + 5) & 1;
}

char read_serial() {
   while (serial_received() == 0);

   return inb(COM1_PORT);
}

char read_serial_non_blocking() {
    if (!serial_received()) {
        return 0;
    }
    return inb(COM1_PORT);
}

int is_transmit_empty() {
   return inb(COM1_PORT + 5) & 0x20;
}

void write_serial(char a) {
   while (is_transmit_empty() == 0);

   outb(COM1_PORT,a);
}

void uart_print(const char* s){
    size_t len = strlen(s);
    for (size_t i = 0; i < len; i++) {
        write_serial(s[i]);
    }
}