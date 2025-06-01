#include "device_gpio.h"

#include <stdio.h>
#include "esp_log.h"
// #include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "device.h"
#include "stdbool.h"
#include "sdkconfig.h"
#include "event_manager.h"

#define ADC_CHANNEL    ADC_CHANNEL_6  
#define ADC_ATTEN      ADC_ATTEN_DB_12  
#define ADC_WIDTH      ADC_BITWIDTH_12  
#define SAMPLE_COUNT   100  
#define ADC_V_REF      3.3f 
#define ADC_MAX_VALUE  4095  

static adc_oneshot_unit_handle_t adc_handle;  


#define HIGH true
#define LOW false

#define GPIO_QUEUE_SIZE 3
#define TO_GPIO_STATE_DEV 1000

typedef struct { int pin; bool level; } GpioConf;

static const GpioConf gpio_init_data_list[] = {
    { CONFIG_BUT_LEFT_PIN, LOW, },
    { CONFIG_BUT_RIGHT_PIN, LOW, },
    { CONFIG_BUT_CENTER_PIN, LOW, },
 };
 static GpioCtx but_list[ARR_LEN(gpio_init_data_list)];

 
 
 void device_read_but_handler(unsigned ms, void*unused) 
 {
    unsigned bits = 0;
    GpioState but_val;
    unsigned held = 0, click = 0;
    for(int i=0; i< ARR_LEN(but_list); i++){
        if((but_val = get_inp_state(&but_list[i], ms)) != GPIO_INACTIVE){
            if(but_val == GPIO_HELD){
                bits |= 1<<(i*2 + 1);
                held++;
            } else if(but_val == GPIO_CLICK || held || click){
                bits |= 1<<(i*2);
                click++;
            }
        }
    }
    if(bits){
        unsigned debounce_time = held ? HELD_TIME : DEBOUNCE_TIME;
        for(int i=0; i< ARR_LEN(but_list); i++){
            but_list[i].start_active_time = ms + debounce_time;
        }
        if((held + click) > 1){
            bits |= BUT_MULTI_PRESS;   
        }
        set_event(EVN_BUT_PRESS, CTX_U32(bits));
    }
}

void device_gpio_adc_init()
{
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc_handle));
    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_WIDTH,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &chan_cfg));
}

void device_gpio_init() 
{
    for(int i=0; i< ARR_LEN(gpio_init_data_list); i++){
        ESP_ERROR_CHECK(device_conf_gpio_in(&but_list[i], 
                            gpio_init_data_list[i].pin, 
                            gpio_init_data_list[i].level));
    }
    register_event_handler(EVN_READ_GPIO, device_read_but_handler);
    start_periodic_event(EVN_READ_GPIO, NULL, 75, FOREVER);
}