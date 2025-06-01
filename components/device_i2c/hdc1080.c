#include "hdc1080.h"

#include "device_i2c.h"
#include "event_manager.h"
#include <string.h>
#include "esp_log.h"

static uint8_t config_data[] = {0x10, 0x00}; 
static i2c_op_t ops[3];
static i2c_request_t req;
bool is_init;

static void hum_cplt_callback(esp_err_t res, void *user_ctx, uint8_t *rx_data, size_t rx_data_size);
static void temp_cplt_callback(esp_err_t res, void *user_ctx, uint8_t *rx_data, size_t rx_data_size);


static
void hum_cplt_callback(esp_err_t res, void *user_ctx, uint8_t *rx_data, size_t rx_data_size)
{
    if(res == ESP_OK && user_ctx != NULL && rx_data != NULL){
        int *hum = (int *)user_ctx;
        uint32_t raw_hum = ((uint32_t)rx_data[0] << 8) | rx_data[1];
        *hum = (raw_hum * 1000) / 65536;
    }
}

static
void temp_cplt_callback(esp_err_t res, void *user_ctx, uint8_t *rx_data, size_t rx_data_size)
{
    if(res == ESP_OK && user_ctx != NULL && rx_data != NULL){
        int *temp = (int *)user_ctx;
        uint32_t raw_temp = ((uint32_t)rx_data[0] << 8) | rx_data[1];
        *temp = (raw_temp * 1650) / 65536 - 400;
    }
}


static 
void hdc1080_polling_handler(unsigned int now_ms, void *ctx)
{
    i2c_request_t *req = (i2c_request_t *)ctx;
    if(req == NULL || req->ops == NULL){
        ESP_LOGE("", "hdc1080_polling_handler()");
        return;
    }
    if(is_init){
        is_init = true;
        req->ops++;
        req->op_count = 2;
    }
    i2c_send_async(req);
}


void hdc1080_init(int *hum, int *temp)
{
    memset(ops, 0, sizeof(ops));
    init_req(&req, I2C_MASTER_NUM, HDC1080_ADDR, sizeof(uint8_t), NULL);
    ops[0].post_delay = 150;
    ops[0].write_data = config_data;
    ops[0].write_size = sizeof(config_data);
    ops[0].reg_addr = HDC1080_REG_CONFIG;
    
    ops[1].user_ctx = hum;
    ops[1].op_cplt_callback = hum_cplt_callback;
    ops[1].post_delay = 50;
    ops[1].read_size = 2;
    ops[1].reg_addr = HDC1080_REG_HUMID;
    
    ops[2].user_ctx = temp;
    ops[2].op_cplt_callback = temp_cplt_callback;
    ops[2].post_delay = 25;
    ops[2].read_size = 2;
    ops[2].reg_addr = HDC1080_REG_TEMP;
    req.ops = ops;
    req.op_count = 1;
    register_event_handler(EVN_HDC1080_POLLING, hdc1080_polling_handler);
    start_periodic_event(EVN_HDC1080_POLLING, &req, 1000, FOREVER);
}