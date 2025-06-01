#include "unity.h"
#include "esp_log.h"
#include "string.h"
#include "stdio.h"
#include "device_gpio.h"
#include "event_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "device.h"


static
void print_but(unsigned ms, void *ctx)
{
    unsigned bits = CTX_TO_U32(ctx);
    if(bits == 0) return;
    char print[150] = { 0 }, buf[20];
    if(bits & BUT_MULTI_PRESS){
        strcat(print, "Multipress");
    }
    for(int i=0; i<3; i++){
        if(bits & 1<<(i*2)){
            sprintf(buf, " but%i click", i);
            strcat(print, buf);
        } else if(bits & (1<<(i*2 + 1))){
            sprintf(buf, " but%i held", i);
            strcat(print, buf);
        }
    }
    ESP_LOGI("but", "%s", print);
}

TEST_CASE("but read", "[gpio][but]") 
{
    init_event_manager();
    TEST_ASSERT_TRUE(register_event_handler(EVN_BUT_PRESS, print_but));
    device_gpio_init();
    ESP_LOGI("gpio", "%s", "start read");
}