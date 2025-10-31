/** MellOS UART driver
 *  With much help from https://wiki.osdev.org/Serial_Ports
 **/
#include "autoconf.h"
#include "uart.h"
#include "port_io.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"

struct port_map {
    bool com1 : 1;
    bool com2 : 1;
    bool com3 : 1;
    bool com4 : 1;
};

static struct port_map enabled_ports;

bool port_init(COM_PORT port) {
    outb(port + 1, 0x00); // Disable all interrupts
    outb(port + 3, 0x80); // Enable Divisor Latch Access Bit (allows setting baud rate)
    outb(port + 0, 0x03); // Set divisor (lo byte)
    outb(port + 1, 0x00); // Set divisor (hi byte)
    outb(port + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 4, 0x0B); // IRQs enabled, RTS/DSR set
    outb(port + 4, 0x1E); // Set in loopback mode, test the serial chip
    outb(port + 0, 0xAE); // Test serial chip (send byte 0xAE and check if serial returns same byte)
    if (inb(port + 0) != 0xAE) {
        // Serial returned a different byte, probably faulty
        return false;
    }

    // Set the serial port in normal mode, (no loopback, IRQs on, OUT1 and OUT2 enabled)
    outb(port + 4, 0x0F);
    return true;
}

void uart_init() {
#ifdef CONFIG_COM1
    enabled_ports.com1 = port_init(COM1);
#endif
#ifdef CONFIG_COM2
    enabled_ports.com2 = port_init(COM2);
#endif
#ifdef CONFIG_COM3
    enabled_ports.com3 = port_init(COM3);
#endif
#ifdef CONFIG_COM4
    enabled_ports.com4 = port_init(COM4);
#endif
}

int serial_received(COM_PORT port) {
    return inb(port + 5) & 1;
}

char read_serial(COM_PORT port) {
    while (serial_received(port) == 0)
        ;

    return inb(port);
}

char read_serial_non_blocking(COM_PORT port) {
    if (!serial_received(port)) {
        return 0;
    }
    return inb(port);
}

int is_transmit_empty(COM_PORT port) {
    return inb(port + 5) & 0x20;
}

void write_serial(char a, COM_PORT port) {
    while (is_transmit_empty(port) == 0)
        ;

    outb(port, a);
}

void uart_print(const char* s, COM_PORT port) {
    size_t len = strlen(s);
    for (size_t i = 0; i < len; i++) {
        write_serial(s[i], port);
    }
}

void uart_print_all(const char* s) {
    if (enabled_ports.com1) {
        uart_print(s, COM1);
    }
    if (enabled_ports.com2) {
        uart_print(s, COM2);
    }
    if (enabled_ports.com3) {
        uart_print(s, COM3);
    }
    if (enabled_ports.com4) {
        uart_print(s, COM4);
    }
}

char read_any_serial_non_blocking() {
    char out = 0;
    if (enabled_ports.com1) {
        out = read_serial_non_blocking(COM1);
    }
    if (out != 0)
        return out;
    if (enabled_ports.com2) {
        out = read_serial_non_blocking(COM2);
    }
    if (out != 0)
        return out;
    if (enabled_ports.com3) {
        out = read_serial_non_blocking(COM3);
    }
    if (out != 0)
        return out;
    if (enabled_ports.com4) {
        out = read_serial_non_blocking(COM4);
    }
    return out;
}
