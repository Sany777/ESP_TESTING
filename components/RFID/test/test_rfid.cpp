extern "C"{
    #include "unity.h"
    #include "esp_log.h"
    #include "freertos/FreeRTOS.h"
    #include "esp_log.h"
    #include "string.h"
    #include "stdio.h"
    #include "esp_err.h"
}
#include "rfid.hpp"
#include "serial.hpp"

static const char*TAG = "rfid";

// TEST_CASE("echo serial", "[uart][rfid][hardware]")
// {
//    const char data[] = "esp:data";
//    char buf[100] = {0};
//    EspUart serial;
//    TEST_ASSERT_EQUAL(ESP_OK, serial.begin(9600, UART_NUM_2, GPIO_NUM_0, GPIO_NUM_16));
//    TEST_ASSERT_EQUAL(sizeof(data)-1, serial.write(data));
//    size_t rb = serial.read(buf, sizeof(buf));
//    TEST_ASSERT_EQUAL(sizeof(data)-1, rb);
//    TEST_ASSERT_TRUE(strcmp(buf, data) == 0);
// }


// TEST_CASE("initialisation mfrc", "[rfid]")
// {
//     EspUart espUart;
//     TEST_ASSERT_EQUAL(ESP_OK, espUart.begin(9600, UART_NUM_2, GPIO_NUM_16, GPIO_NUM_0));
//     RFID rf(&espUart);
//     TEST_ASSERT_EQUAL(RFID::STATUS_OK, rf.PCD_Init());
// }

TEST_CASE("check new card", "[rfid]")
{
    EspUart espUart;
    TEST_ASSERT_EQUAL(ESP_OK, espUart.begin(9600, UART_NUM_2, GPIO_NUM_16, GPIO_NUM_0));
    RFID rf(&espUart);
    ESP_LOGI(TAG, "put card on device\n");
    bool res = false;
    for(int i=200; i>0 && !res; --i){
        TEST_ASSERT_EQUAL(RFID::STATUS_OK, rf.PCD_Init());
        res = rf.PICC_IsNewCard();
    }
    TEST_ASSERT_TRUE(res);
}


