#pragma once

#include "stdbool.h"
#include <stdint.h>
#include "esp_err.h"



typedef enum {
    GPIO_INACTIVE,
    GPIO_ACTIVE,
    GPIO_CLICK,
    GPIO_HELD,
} GpioState;

#define BUT1_CLICK BIT0
#define BUT1_HOLD BIT1
#define BUT2_CLICK BIT2
#define BUT2_HOLD BIT3
#define BUT3_CLICK BIT4
#define BUT3_HOLD BIT5
#define BUT_MULTI_PRESS BIT6



enum LatencyMS {
    START_TIME = 1000,
    DEBOUNCE_TIME = 75,
    HELD_TIME = 800
};

typedef struct {
    int pin_num;
    GpioState state;
}GpioData;

typedef struct {
    GpioData gdata;
    bool active_level;
    unsigned start_active_time;
} GpioCtx;


GpioState get_inp_state(GpioCtx *conf, unsigned cur_time);
int device_conf_gpio_in(GpioCtx * conf, int pin_num, bool active_level);

bool isr_gpio_init(void);
uint16_t get_tacho_value(void);

bool pwm_init(void);
void pwm_set_duty(uint32_t duty_perc);
void pwm_start(void);
void pwm_stop(void);


void device_gpio_init(void);