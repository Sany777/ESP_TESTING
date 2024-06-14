extern "C"{
    #include "unity.h"
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "esp_log.h"
    #include "string.h"
    #include "stdio.h"
}
#include "serial.hpp"

static const char *TAG = "uart";

TEST_CASE("test restore serial monitor settings", "[uart][pytest][1]")
{
   {
      EspUart serial;
      TEST_ASSERT_TRUE(serial.begin(9600, UART_NUM_0) == ESP_OK);
   }
   ESP_LOGI(TAG, "serial monitor settings was restored");
}

TEST_CASE("write serial", "[uart][pytest][2]")
{
   const char data[] = "esp:data";
   const char mes[] = "esp:send";
   ESP_LOGI(TAG, "%s", mes);
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(115200) == ESP_OK);
   TEST_ASSERT_EQUAL(sizeof(mes)-1, serial.write(mes));
   TEST_ASSERT_EQUAL(sizeof(data)-1, serial.write(data));
}

TEST_CASE("read serial", "[uart][pytest][3]")
{
   const char mes[] = "esp:expect data";
   const char data[] = "pytest:data";
   char buf[100] = {0};
   size_t rb = 0;
   {
      EspUart serial;
      TEST_ASSERT_TRUE(serial.begin(115200) == ESP_OK);
      int atempts = 100;
      ESP_LOGI(TAG, "%s", mes);
      do{
         vTaskDelay(100);
         rb = serial.read(buf, sizeof(buf));
      }while(rb != sizeof(data)-1 && atempts--);
   }
   TEST_ASSERT_EQUAL(sizeof(data)-1, rb);
   TEST_ASSERT_TRUE(strcmp(buf, data) == 0);
   ESP_LOGI(TAG, "esp:ok");
}

TEST_CASE("test UART initialisation: baud rate=115200", "[uart][4]")
{
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(115200) == ESP_OK);
}

TEST_CASE("test UART initialisation: baud rate=9600", "[uart][5]")
{
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(9600) == ESP_OK);
}

TEST_CASE("test UART initialisation: port changed", "[uart][6]")
{
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(9600, UART_NUM_1) == ESP_OK);
}


TEST_CASE("test UART initialisation: reinstallation baud rate", "[uart][7]")
{
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(115200, UART_NUM_1) == ESP_OK);
   TEST_ASSERT_TRUE(serial.begin(9600, UART_NUM_1) == ESP_OK);
}

TEST_CASE("test UART initialisation: pins changed", "[uart][8]")
{
   const gpio_num_t TX_PIN = GPIO_NUM_12;
   const gpio_num_t RX_PIN = GPIO_NUM_13;
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(115200, UART_NUM_1, TX_PIN, RX_PIN) == ESP_OK);
}
