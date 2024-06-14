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

TEST_CASE("test restore serial monitor settings", "[uart][pytest][ignore]")
{
   {
      EspUart serial;
      TEST_ASSERT_TRUE(serial.begin(9600, UART_NUM_0) == ESP_OK);
   }
   ESP_LOGI(TAG, "serial monitor settings was restored");
}

TEST_CASE("send text", "[uart][pytest][ignore]")
{
   const char mes[] = "esp:write text";
   const char text[] = "esp:text";
   ESP_LOGI(TAG, "%s", mes);
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(115200) == ESP_OK);
   TEST_ASSERT_EQUAL(sizeof(text)-1, serial.write(text));
}

TEST_CASE("send bin data", "[uart][pytest][ignore]")
{
   const char mes[] = "esp:write bin data";
   const uint8_t bin_data[] = {'0','1','3'};
   ESP_LOGI(TAG, "%s", mes);
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(115200) == ESP_OK);
   TEST_ASSERT_EQUAL(sizeof(bin_data), serial.write(bin_data, sizeof(bin_data)));
}


TEST_CASE("send a byte", "[uart][pytest][ignore]")
{
   const char mes[] = "esp:write a byte";
   const uint8_t byte_data = '0';
   ESP_LOGI(TAG, "%s", mes);
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(115200) == ESP_OK);
   TEST_ASSERT_EQUAL(sizeof(byte_data), serial.write(byte_data));
}

TEST_CASE("uart receive", "[uart][pytest][ignore]")
{
   const char mes[] = "esp:expect data";
   const char data[] = "pytest:data";
   char buf[100] = {0};
   size_t rb = 0;
   ESP_LOGI(TAG, "%s", mes);
   {
      EspUart serial;
      TEST_ASSERT_TRUE(serial.begin(115200) == ESP_OK);
      int atempts = 100;
      do{
         vTaskDelay(100);
         rb = serial.read(buf, sizeof(buf));
      }while(rb != sizeof(data)-1 && atempts--);
   }
   TEST_ASSERT_EQUAL(sizeof(data)-1, rb);
   TEST_ASSERT_TRUE(strcmp(buf, data) == 0);
   ESP_LOGI(TAG, "esp:ok");
}

TEST_CASE("test UART initialisation: baud rate=115200", "[uart]")
{
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(115200) == ESP_OK);
}

TEST_CASE("test UART initialisation: baud rate=9600", "[uart]")
{
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(9600) == ESP_OK);
}

TEST_CASE("test UART initialisation: port changed", "[uart]")
{
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(9600, UART_NUM_1) == ESP_OK);
}


TEST_CASE("test UART possibility reinstallation baud rate", "[uart]")
{
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(115200) == ESP_OK);
   TEST_ASSERT_TRUE(serial.begin(9600)== ESP_OK);
}

TEST_CASE("test send data until changing baud rate", "[uart][ignore]")
{
   const char text1[] = "esp:text 1";
   const char text2[] = "esp:text 2";
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(115200) == ESP_OK);
   TEST_ASSERT_EQUAL(sizeof(text1)-1, serial.write(text1));
   TEST_ASSERT_TRUE(serial.begin(9600) == ESP_OK);
   TEST_ASSERT_EQUAL(sizeof(text2)-1, serial.write(text2));
}

TEST_CASE("test UART initialisation: change pins", "[uart]")
{
   const gpio_num_t TX_PIN = GPIO_NUM_12;
   const gpio_num_t RX_PIN = GPIO_NUM_13;
   EspUart serial;
   TEST_ASSERT_TRUE(serial.begin(115200, UART_NUM_1, TX_PIN, RX_PIN) == ESP_OK);
}
