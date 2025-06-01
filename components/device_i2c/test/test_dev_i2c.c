#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "stdio.h"

#include "device.h"
#include "event_manager.h"
#include "driver/i2c.h"
#include "device_i2c.h"
#include "hdc1080.h"


static const char *TAG = "device_i2c";

#define TEST_EVENT 123

#define TEST_I2C_PORT I2C_NUM_0
#define TEST_I2C_ADDR 0x40


static volatile int event_counter = 0;

void setUp(void) 
{
    device_init();
    event_counter = 0;
}

void tearDown(void) 
{
    
}

void test_config_cplt_callback(esp_err_t res, void *ctx, uint8_t *rx_data, size_t rx_data_size)
{
    event_counter++;
    ESP_LOGI(TAG, "%s", "set config complete\n");
}

void test_hum_cplt_callback(esp_err_t res, void *ctx, uint8_t *rx_data, size_t rx_data_size)
{
    unsigned *hum = (unsigned *)ctx;
    event_counter++;
    uint32_t raw_hum = ((uint32_t)rx_data[0] << 8) | rx_data[1];
    *hum = (raw_hum * 1000) / 65536;
    
}

void test_temp_cplt_callback(esp_err_t res, void *ctx, uint8_t *rx_data, size_t rx_data_size)
{
    unsigned *temp = (unsigned *)ctx;
    event_counter++;
    uint32_t raw_temp = ((uint32_t)rx_data[0] << 8) | rx_data[1];
    *temp = (raw_temp * 1650) / 65536 - 400;
}



uint8_t config_data[] = {0x10, 0x00}; 
i2c_op_t ops_read[2];
i2c_op_t op_config;


void i2c_scan()
{
    printf("Scanning I2C bus...\n");
    for (uint8_t addr = 1; addr < 127; ++addr) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(10));
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK) {
            printf("Found device at 0x%02X\n", addr);
        }
    }
}

TEST_CASE("I2C initialization", "[i2c_task]") 
{
    init_event_manager();
    TEST_ASSERT_TRUE(i2c_init());
    i2c_scan();
}

TEST_CASE("I2C write operation", "[i2c_task]") 
{
    i2c_request_t req;
    op_config.op_cplt_callback = test_config_cplt_callback;
    op_config.post_delay = 150;
    op_config.write_data = config_data;
    op_config.write_size = sizeof(config_data);
    op_config.reg_addr = HDC1080_REG_CONFIG;
    init_req(&req, I2C_MASTER_NUM, HDC1080_ADDR, sizeof(uint8_t), NULL);
    req_set_oper(&req, &op_config, 1);
    i2c_send_async(&req);
    vTaskDelay(pdMS_TO_TICKS(1000));
    TEST_ASSERT_EQUAL(1, event_counter);
}


TEST_CASE("I2C read operation", "[i2c_task]") 
{
    unsigned temp = 0, hum = 0;
    i2c_request_t req;
    ops_read[0].user_ctx = &hum;
    ops_read[0].op_cplt_callback = test_hum_cplt_callback;
    ops_read[0].post_delay = 50;
    ops_read[0].read_size = 2;
    ops_read[0].reg_addr = HDC1080_REG_HUMID;
    ops_read[1].user_ctx = &temp;
    ops_read[1].op_cplt_callback = test_temp_cplt_callback;
    ops_read[1].post_delay = 25;
    ops_read[1].read_size = 2;
    ops_read[1].reg_addr = HDC1080_REG_TEMP;
    init_req(&req, I2C_MASTER_NUM, HDC1080_ADDR, sizeof(uint8_t), NULL);
    req_set_oper(&req, ops_read, 2);
    TEST_ASSERT_EQUAL(ESP_OK, i2c_send_sync(&req, 1500));
    TEST_ASSERT_EQUAL(2, event_counter);
    ESP_LOGI(TAG, "T: %u H: %u\n", temp,  hum); 
}




TEST_CASE("Start meassuring", "[i2c_task]") 
{
    hdc1080_init(&device.status.humidity, NULL);
}