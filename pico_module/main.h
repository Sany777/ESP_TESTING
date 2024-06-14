#ifndef RP_MODULE
#define RP_MODULE

#include "hardware/uart.h"

#ifndef uint
    #define uint unsigned
#endif

#ifndef bool
    #define bool int
#endif

char keypad_scan();
void uart_putc(uart_inst_t *uart, char c);
bool gpio_get(uint gpio);



#endif