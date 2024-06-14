#include "help_tools.hpp"

gpio_num_t LED_PIN = GPIO_NUM_4;
gpio_num_t RFID_RST_PIN = GPIO_NUM_2;

AutoLock::AutoLock(SemaphoreHandle_t &semaphore){
    this->_semaphore = semaphore;
    xSemaphoreTakeRecursive(semaphore, portMAX_DELAY);
}

AutoLock::~AutoLock(){
    xSemaphoreGiveRecursive(this->_semaphore);
}


int set_pin(gpio_num_t pin, uint8_t state)
{
    gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
    return gpio_set_level(pin, state);
}