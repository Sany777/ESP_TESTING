
extern "C"{
    #include "freertos/FreeRTOS.h"
    #include "unity.h"
    #include "driver/gpio.h"
    #include "freertos/task.h"
    #include "freertos/semphr.h"
    #include "esp_log.h"
}

#include "help_tools.hpp"

const gpio_num_t GPIO_OUTPUT_LED = GPIO_NUM_4;

TEST_CASE("turn on led", "[gpio][true]")
{
    TEST_ESP_OK(led_on(1));
    TEST_ASSERT_EQUAL(1, gpio_get_level(GPIO_OUTPUT_LED));
}

TEST_CASE("turn off led", "[gpio][true]")
{
    TEST_ESP_OK(led_on(0));
    TEST_ASSERT_EQUAL(0, gpio_get_level(GPIO_OUTPUT_LED));
}


static SemaphoreHandle_t test_semaphore = nullptr;
const size_t CICLE_NUM = 250000;


void task_with_synch(void *vp)
{
    size_t &test_counter = *(size_t *)vp;
    vTaskDelay(10);
    for(size_t i=0;i<CICLE_NUM;++i){
        {
            ENTER_CRITICAL(test_semaphore);
            ++test_counter;
        }
    }
    vTaskDelete(NULL);
}

void task_without_synch(void *vp)
{
    size_t &test_counter = *(size_t *)vp;
    vTaskDelay(10);
    for(size_t i=0;i<CICLE_NUM;++i){
        ++test_counter;
    }
    vTaskDelete(NULL);
}


TEST_CASE("test autolock", "[autolock]")
{
    size_t test_counter_1 = 0;
    size_t test_counter_2 = 0;
    test_semaphore = xSemaphoreCreateRecursiveMutex();
    xTaskCreatePinnedToCore(task_without_synch, "task no semaphore 1", 1000, &test_counter_2, 2, NULL,0);
    xTaskCreatePinnedToCore(task_without_synch, "task no semaphore 2", 1000, &test_counter_2, 2, NULL,1);
    xTaskCreatePinnedToCore(task_with_synch, "task with semaphore 1", 1000, &test_counter_1, 2, NULL, 0);
    xTaskCreatePinnedToCore(task_with_synch, "task with semaphore 2", 1000, &test_counter_1, 2, NULL, 1);
    vTaskDelay(2000);
    TEST_ASSERT_EQUAL(CICLE_NUM*2, test_counter_1);
    TEST_ASSERT_NOT_EQUAL(CICLE_NUM*2, test_counter_2);
}
