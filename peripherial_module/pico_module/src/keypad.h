#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_COLUMNS 4
#define NUM_ROWS 4




void keypad_init(void);

int main();
char keypad_scan();


#ifndef bool
    #define bool int
#endif




void uard_sendc(const char c);
void uart_init_custom();




#endif // KEYPAD_H
