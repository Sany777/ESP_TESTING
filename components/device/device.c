#include "device.h"

#include "event_manager.h"
#include "device_memory.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"



const uint8_t val_list_speed1[] = {OFF, 15, 20, 30, 40, 60};
const uint8_t val_list_speed2[] = {15, 20, 30, 40, 60, 100};
const uint8_t val_list_voc_threshold[] = {OFF, 20, 30, 50, 60, 70, 80, 90};
const uint8_t val_list_rh_threshold[] = {OFF, 40, 50, 60, 70, 80, 90};
const uint8_t val_list_interval_tim[] = {OFF, 3, 6, 12};
const uint8_t val_list_on_timer[] = {OFF, 1, 2, 5};
const uint8_t val_list_off_timer[] = {OFF, 5, 15, 30};



device_data_t device;

void set_speed(int speed1, int speed2)
{
    if(speed1 >= 0 && speed1 < ARR_LEN(val_list_speed1)){
        device.config.speed1 = speed1;
    }
    if(speed2 >= 0 && speed2 < ARR_LEN(val_list_speed2)){
        device.config.speed2 = speed2;
    }
}


void set_humidity(int val)
{
    if(val >= 0 && val < ARR_LEN(val_list_rh_threshold)){
        device.config.humidity = val;
    }
}


void set_inter_timer(int val)
{
    if(val >= 0 && val <= ARR_LEN(val_list_interval_tim)){
        device.config.inter_timer = val;
    }
}

void set_timer(int on_timer, int off_timer)
{
    if(on_timer >= 0 && on_timer <= ARR_LEN(val_list_on_timer)){
        device.config.on_timer = on_timer;
    }
    if(off_timer >= 0 && off_timer <= ARR_LEN(val_list_off_timer)){
        device.config.off_timer = off_timer;
    }
}




static EventGroupHandle_t device_evn_group;


unsigned device_set_bits(unsigned val)
{
    return xEventGroupSetBits(device_evn_group, (EventBits_t) val);
}

unsigned device_clear_bits(unsigned val)
{
    return xEventGroupClearBits(device_evn_group, (EventBits_t) val);
}

unsigned device_get_bits()
{
    return xEventGroupGetBits(device_evn_group);
}

unsigned device_wait_bits(unsigned bits, unsigned wait_ms)
{
    return xEventGroupWaitBits(device_evn_group, (EventBits_t) bits,
                                pdFALSE,
                                pdFALSE,
                                pdMS_TO_TICKS(wait_ms));
}



void set_sta_pwd(const char *str)
{
    const int len = strnlen(str, MAX_PWD_LEN);
    memcpy(device.settings.wifi_conf.sta.password, str, len);
    device.settings.wifi_conf.sta.password[len] = 0;  
}

void set_sta_ssid(const char *str)
{
    const int len = strnlen(str, MAX_SSID_LEN);
    memcpy(device.settings.wifi_conf.sta.ssid, str, len);
    device.settings.wifi_conf.sta.ssid[len] = 0; 
}

void device_init()
{
    if(device_evn_group != NULL) return;
    device_evn_group = xEventGroupCreate();
    assert(device_evn_group != NULL);
    init_event_manager();
    init_nvs();
}
