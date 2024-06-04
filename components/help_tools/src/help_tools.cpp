#include "help_tools.hpp"



AutoLock::AutoLock(SemaphoreHandle_t &semaphore){
    this->_semaphore = semaphore;
    xSemaphoreTakeRecursive(semaphore, portMAX_DELAY);
}

AutoLock::~AutoLock(){
    xSemaphoreGiveRecursive(this->_semaphore);
}


int led_on(uint8_t state)
{
    const gpio_num_t GPIO_OUTPUT_LED = GPIO_NUM_4;
    gpio_set_direction(GPIO_OUTPUT_LED, GPIO_MODE_INPUT_OUTPUT);
    return gpio_set_level(GPIO_OUTPUT_LED, state);
}