#include "unity.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include <stdio.h>
#include <driver/i2c.h>
#include <st25dv_registers.h>
#include <st25dv_ndef.h>
#include <string.h>
#include "st25dv_registers.h"

#include "device_i2c.h"
#include "device.h"


#define APP_PKG_NAME "com.blauberg.universal"


bool st25dv_wait_for_phone_write(uint32_t timeout_ms) 
{
    uint8_t mb_ctrl = 0;
    uint32_t elapsed = 0;
    const uint32_t poll_interval = 50; 

    printf("Waiting for phone write via NFC...\n");

    while (elapsed < timeout_ms) {
        st25dv_read_byte(ST25DV_SYSTEM_ADDRESS, REG_MB_CTRL_DYN, &mb_ctrl);

        if (mb_ctrl & 0x01) { 
            printf("Phone write detected!\n");
            return true;
        }

        vTaskDelay(poll_interval / portTICK_PERIOD_MS);
        elapsed += poll_interval;
    }

    printf("Timeout waiting for phone write.\n");
    return false;
}

TEST_CASE("st25dv: init and open session", "[st25][init]")
{
    device_init();
    st25dv_init_bus(0);
    TEST_ASSERT_EQUAL(ESP_OK, st25dv_open_session(ST25DV_SYSTEM_ADDRESS, 0x0000000000000000));

    bool session = false;
    TEST_ASSERT_EQUAL(ESP_OK, st25dv_is_session_opened(ST25DV_USER_ADDRESS, &session));
    printf("I2C session : 0x%X\n", session);
}

TEST_CASE("st25dv: read/write memory", "[st25][memory]")
{
    uint8_t data[512] = {0};
    TEST_ASSERT_EQUAL(ESP_OK, st25dv_read(ST25DV_USER_ADDRESS, 0x00, data, sizeof(data)));

    printf("Read 512 bytes:\n");
    for (int i = 0; i < sizeof(data); i++) {
        printf("[%u]", (unsigned)data[i]);
        data[i] = i;
    }
    TEST_ASSERT_EQUAL(ESP_OK, st25dv_write(ST25DV_USER_ADDRESS, 0x00, data, sizeof(data)));
    printf("Write 0x00 to 0x200\n");
}

TEST_CASE("st25dv: read GPO1 enable bit", "[st25][reg]")
{
    bool bit_value = 0;
    TEST_ASSERT_EQUAL(ESP_OK, st25dv_read_bit(ST25DV_SYSTEM_ADDRESS, REG_GPO1, BIT_GPO1_GPO_EN, &bit_value));
    printf("REG_GPO1 EN bit : %u\n", bit_value);
}

TEST_CASE("st25dv: random write/read test", "[st25][rw-test]")
{
    uint8_t test_write_data[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE };
    uint8_t test_read_data[sizeof(test_write_data)] = {0};
    uint16_t test_address = 0x100;

    TEST_ASSERT_EQUAL(ESP_OK, st25dv_write(ST25DV_USER_ADDRESS, test_address, test_write_data, sizeof(test_write_data)));
    vTaskDelay(50 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(ESP_OK, st25dv_read(ST25DV_USER_ADDRESS, test_address, test_read_data, sizeof(test_read_data)));

    TEST_ASSERT_EQUAL_UINT8_ARRAY(test_write_data, test_read_data, sizeof(test_write_data));
    printf("Random data test PASSED.\n");
}

/*NDEF (NFC Data Exchange Format) — 
це стандартний формат збереження 
та обміну даними в NFC-пристроях що
використовується для передачі структурованої інформації, 
такої як URL, текст, Wi-Fi-конфігурація, контакти */
TEST_CASE("st25dv: NDEF write and read", "[st25][ndef]")
{
    st25dv_config cfg = { ST25DV_USER_ADDRESS, ST25DV_SYSTEM_ADDRESS };
    uint16_t address = CCFILE_LENGTH;

    st25dv_ndef_write_ccfile(0x00040000010040E2);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    st25dv_ndef_write_app_launcher_record(cfg, &address, true, false, APP_PKG_NAME);

    cJSON *cj = cJSON_CreateObject();
    cJSON_AddNumberToObject(cj, "height", 720);
    cJSON_AddNumberToObject(cj, "width", 1280);
    st25dv_ndef_write_json_record(cfg, &address, false, true, monitor);
    cJSON_Delete(cj);

    vTaskDelay(100 / portTICK_PERIOD_MS);

    std25dv_ndef_record read = { 0 };
    uint8_t record_num = 2;
    uint8_t record_count = 0;

    st25dv_ndef_read(cfg, record_num, &read, &record_count);
    printf("Record %d type : %s\n", record_num, read.type);
}


TEST_CASE("st25dv: phone writing", "[st25][nfc][wait]")
{
    uint8_t data[512] = {0};

    printf("Use NFC Tools to write new data to the tag...\n");
    if (st25dv_wait_for_phone_write(30000)) {
        st25dv_read(ST25DV_USER_ADDRESS, 0x00, data, sizeof(data));
        printf("Data after writing:\n");
        for (int i = 0; i < sizeof(data); i++) {
            char c = data[i];
            if (c >= 32 && c <= 126)
                printf("%c", c);
        }
    }
}


TEST_CASE("st25dv: read single byte", "[st25][byte]")
{
    uint8_t value = 0;
    TEST_ASSERT_EQUAL(ESP_OK, st25dv_read_byte(ST25DV_USER_ADDRESS, 0x00, &value));
    printf("0x00 value : 0x%02X\n", value);
}

