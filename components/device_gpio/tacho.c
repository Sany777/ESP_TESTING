#include "device_gpio.h"

#include "event_manager.h"
#include "esp_err.h"
#include <stdint.h>
#include "driver/gpio.h"
#include "device.h"
#include "stddef.h"

// #define PIN_HALL_IN GPIO_NUM_15


// static volatile uint16_t tacho_counter;
// static uint16_t tacho_value;


// static 
// void IRAM_ATTR hall_isr_handler(void* arg) 
// {
//     tacho_counter++;
// }


// static 
// void IRAM_ATTR update_tacho_value_handler()
// {
//     tacho_value = tacho_counter;
//     tacho_counter = 0;
// }

// uint16_t get_tacho_value()
// {
//     return tacho_value;
// }

// bool isr_gpio_init() 
// {
//     gpio_config_t io_conf = {
//         .pin_bit_mask = (1ULL << PIN_HALL_IN),
//         .mode = GPIO_MODE_INPUT,
//         .pull_up_en = GPIO_PULLUP_ENABLE,
//         .intr_type = GPIO_INTR_NEGEDGE
//     };
//     CHECK_RET_FALSE(gpio_config(&io_conf));
//     CHECK_RET_FALSE(gpio_install_isr_service(0));
//     CHECK_RET_FALSE(gpio_isr_handler_add(PIN_HALL_IN, hall_isr_handler, NULL));
//     return true;
// }