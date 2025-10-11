#pragma once

#include "utils/typedefs.h"

typedef uint16_t COM_PORT;

static const COM_PORT COM1 = 0x3F8;
static const COM_PORT COM2 = 0x2F8;
static const COM_PORT COM3 = 0x3E8;
static const COM_PORT COM4 = 0x2E8;

void uart_init();
void uart_print(const char *str, COM_PORT port);
void uart_print_all(const char *str);
char read_serial(COM_PORT port);
char read_serial_non_blocking(COM_PORT port);
char read_any_serial_non_blocking();