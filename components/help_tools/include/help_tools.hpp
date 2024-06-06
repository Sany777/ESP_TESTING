#pragma once

extern "C"{
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/gpio.h"
}

class AutoLock{
    SemaphoreHandle_t _semaphore;
public:
    AutoLock(SemaphoreHandle_t &semaphore);
    ~AutoLock();
};

#define ENTER_CRITICAL(semaphore)    \
    AutoLock _al(semaphore)

int led_on(uint8_t state);

#define CHECK_ESPERR_RET_VOID(_e)  do{if(_e != ESP_OK) return;}while(0)
#define CHECK_ESPERR_RET_ERR(_e)   do{if(_e != ESP_OK) return ESP_FAIL;}while(0)

#ifndef MIN
    #define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif