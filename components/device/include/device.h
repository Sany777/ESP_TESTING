#ifndef DEVICE_H
#define DEVICE_H

#include "stdbool.h"
#include "device_def.h"
#include "comm_types.h"
#include "global_helpers.h"

#define IGNORE_VAL -1


void device_init();
unsigned device_set_bits(unsigned val);
unsigned device_clear_bits(unsigned val);
unsigned device_get_bits();
unsigned device_wait_bits(unsigned bits, unsigned wait_ms);




void set_sta_pwd(const char *str);
void set_sta_ssid(const char *str);





void set_speed(int speed1, int speed2);
void set_humidity(int val);
void set_inter_timer(int val);
void set_timer(int on_timer, int off_timer);

// extern device_data_t device;



















#endif // DEVICE_H