extern "C"{
    #include "unity.h"
    #include "esp_log.h"
}
#include "MFRC522.hpp"


TEST_CASE("test init UART", "[uart]")
{
   TEST_ESP_OK(uart_init());
}

// TEST_CASE("test echo UART", "[uart]")
// {
//    const char txt_data[] = "hello!";
//    char buf[100];
//    int wb = uart_send(txt_data, sizeof(txt_data));
//    TEST_ASSERT_EQUAL(sizeof(txt_data), wb);
//    TEST_ASSERT_EQUAL(sizeof(txt_data), uart_receive(buf, sizeof(buf)));
// }

// TEST_CASE("test send hello MFRC522", "[uart]")
// {
//    const uint8_t data[] = {0x37 << 1};
//    char buf[100];
//    TEST_ASSERT_EQUAL(sizeof(data), uart_send(data, sizeof(data)));
//    TEST_ASSERT_EQUAL(sizeof(data), uart_receive(buf, sizeof(buf)));
//    ESP_LOGI("", "\n%u\n", *buf);
// }