#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "main.h"


#define UART_ID uart0
#define BAUD_RATE 115200


static const uint COLUMN_PINS[] = {2, 3, 4, 5};
static const uint ROW_PINS[] = {6, 7, 8, 9};


static const char KEY_CODES[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


void uart_init_custom() 
{
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(0, GPIO_FUNC_UART); // TX
    gpio_set_function(1, GPIO_FUNC_UART); // RX
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(UART_ID, false);
}


void keypad_init() 
{
    for (int i = 0; i < 4; i++) {
        gpio_init(COLUMN_PINS[i]);
        gpio_set_dir(COLUMN_PINS[i], GPIO_OUT);
        gpio_put(COLUMN_PINS[i], 0);  

        gpio_init(ROW_PINS[i]);
        gpio_set_dir(ROW_PINS[i], GPIO_IN);
        gpio_pull_down(ROW_PINS[i]);  
    }
}


char keypad_scan() 
{
    for (int col = 0; col < 4; col++) {
        gpio_put(COLUMN_PINS[col], 1);  
        for (int row = 0; row < 4; row++) {
            if (gpio_get(ROW_PINS[row])) {  
                while (gpio_get(ROW_PINS[row]));  
                gpio_put(COLUMN_PINS[col], 0);  
                return KEY_CODES[col][row];  
            }
        }
        gpio_put(COLUMN_PINS[col], 0);  
    }
    return 0;  
}

int main() 
{
    stdio_init_all();
    uart_init_custom();
    keypad_init();

    while (1) {
        char key = keypad_scan();
        if (key != 0) {
            uart_putc(UART_ID, key);
            sleep_ms(250);  
        }
        sleep_ms(10); 
    }
    return 0;
}
