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

int set_pin(gpio_num_t pin, uint8_t state);

#define CHECK_RES_RET(expect_status, act_status)    \
    do{ if((expect_status) != (act_status)) return; }while(0)
    
#define CHECK_RES_RET_ERR(expect_status, act_status_expr) \
    do {                                                  \
        const int act_status = (act_status_expr);         \
        if ((expect_status) != (act_status))              \
            return (act_status);                          \
    } while (0)

#ifndef MIN
    #define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

extern gpio_num_t LED_PIN;
extern gpio_num_t RFID_RST_PIN;