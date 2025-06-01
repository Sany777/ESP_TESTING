#include "device_gpio.h"

#include "device.h"
#include "driver/ledc.h"
#include <math.h>

#define LEDC_TIMER          LEDC_TIMER_2
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL        LEDC_CHANNEL_0
#define LEDC_DUT_RES        LEDC_TIMER_13_BIT 
#define DEFAULT_DUTY       (50) 
#define LEDC_FREQUENCY     (2000) 
#define PIN_PWM_OUT        GPIO_NUM_6
// LEDC_TIMER_13_BIT
#define MAX_TIM_VAL 8191

bool pwm_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUT_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PIN_PWM_OUT,
        .duty           = 0, 
        .hpoint         = 0
    };
    CHECK_RET_FALSE(ledc_timer_config(&ledc_timer));
    CHECK_RET_FALSE(ledc_channel_config(&ledc_channel));
    return true;
}

void pwm_set_duty(uint32_t duty_perc)
{
    if(duty_perc > 100) duty_perc = 100;
    uint32_t duty = (duty_perc * MAX_TIM_VAL) / 100;
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void pwm_start()
{
    ledc_timer_resume(LEDC_MODE, LEDC_TIMER);
}

void pwm_stop()
{
    pwm_set_duty(0);
    ledc_timer_pause(LEDC_MODE, LEDC_TIMER);
}