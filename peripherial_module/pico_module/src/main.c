#include "keypad.h"

#include "pico/stdlib.h"


int main() 
{
    uart_init_custom();
    keypad_init();

    while (1) {
        char key = keypad_scan();
        if (key != 0) {
            uard_sendc(key);
            sleep_ms(250);  
        }
        sleep_ms(10); 
    }
    return 0;
}
