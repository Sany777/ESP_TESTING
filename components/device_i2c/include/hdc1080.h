#ifndef HDC1080_H
#define HDC1080_H

#include "stdint.h"

#define HDC1080_ADDR                0x40
#define HDC1080_REG_TEMP            0x00
#define HDC1080_REG_HUMID           0x01
#define HDC1080_REG_CONFIG          0x02

void hdc1080_init(int *hum, int *temp);



#endif //HDC1080