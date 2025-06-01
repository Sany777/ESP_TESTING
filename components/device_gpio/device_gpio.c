#include "device_gpio.h"

#include "esp_err.h"
#include "driver/gpio.h"

// mock
// extern bool gpio_get_level_mock(int pin);

GpioState  get_inp_state(GpioCtx *conf, unsigned cur_time)
{
    if (conf->start_active_time > cur_time){
        return GPIO_INACTIVE;
    }
    GpioState state;
    unsigned active_state_time = cur_time - conf->start_active_time;
    if(gpio_get_level(conf->gdata.pin_num) == conf->active_level){
        state = active_state_time > HELD_TIME 
                    ? GPIO_HELD 
                    : GPIO_ACTIVE;
    } else if(active_state_time > DEBOUNCE_TIME){
        state = GPIO_CLICK;
    } else {
        state = GPIO_INACTIVE;
        conf->start_active_time = cur_time;
    }
    return state;
}


int device_conf_gpio_in(GpioCtx * conf, int pin_num, bool active_level)
{
    conf->active_level = active_level;
    conf->gdata.pin_num = pin_num;
    conf->start_active_time = START_TIME;
    gpio_config_t in_conf = {
        .pin_bit_mask = (1ULL << conf->gdata.pin_num),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = conf->active_level ? GPIO_PULLUP_DISABLE : GPIO_PULLUP_ENABLE,
        .pull_down_en = conf->active_level ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
    };
   return gpio_config(&in_conf);
}







