#include "unity.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "string.h"
#include "stdio.h"
#include "device_memory.h"


static const char *TAG = "memory";


TEST_CASE("test init", "[memory][pytest]")
{
   TEST_ASSERT_EQUAL_INT(init_nvs(), ESP_OK);
   ESP_LOGI(TAG, "%s", "test init OK");
}


TEST_CASE("test write / read memory", "[memory][pytest]")
{
   struct TestData {
      uint8_t u8d;
      uint32_t u32d;
      char str[10];
   };
   
   const char * storage_id = "test1";
   struct TestData test_r_data;
   struct TestData  test_w_data = {
      1,
      0xFF1010,
      "12345678"
   };
   TEST_ASSERT_EQUAL_INT(write_flash(storage_id, &test_w_data, sizeof(test_w_data)), ESP_OK);
   TEST_ASSERT_EQUAL_INT(read_flash(storage_id, &test_r_data, sizeof(test_r_data)), ESP_OK);
   TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&test_r_data, &test_w_data, sizeof(test_w_data), "test_r_data != test_w_data");
   ESP_LOGI(TAG, "%s", "test write / read memory OK");
}