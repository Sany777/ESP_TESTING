#include <memory.h>
#include <st25dv_registers.h>
#include "st25dv.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "device_i2c.h"


static i2c_request_t req;

extern esp_err_t i2c_send_sync(const i2c_request_t *req, uint32_t ms);


void st25dv_init_bus(int port)
{
    i2c_init();
    i2c_set_timeout(port, 0x0000001FU);
    init_req(&req, port, 0, sizeof(uint16_t), copy_callback);
}


esp_err_t st25dv_write_byte(uint8_t st25_address, uint16_t address, uint8_t byte) 
{
    i2c_op_t i2c_oper = {
        .reg_addr = address,
        .write_data = &byte,
        .write_size = 1,
    };
    req.device_addr = st25_address;
    req_set_oper(&req, &i2c_oper, 1);
    return i2c_send_sync(&req, 2000);
}

esp_err_t st25dv_read_byte(uint8_t st25_address, uint16_t address, uint8_t *byte) 
{
    i2c_op_t i2c_oper = {
        .reg_addr = address,
        .user_ctx = byte,
        .read_size = sizeof(uint8_t),
    };
    req.device_addr = st25_address;
    req_set_oper(&req, &i2c_oper, 1);
    return i2c_send_sync(&req, 2000);
}

esp_err_t st25dv_write_bit(uint8_t st25_address, uint16_t address, uint8_t bit_mask, bool bit) 
{
    uint8_t byte = 0;
    esp_err_t ret;
    ret = st25dv_read_byte(st25_address, address, &byte);
    if (ret != ESP_OK) {
        return ret;
    }
    if (bit) {
        byte |= bit_mask;
    } else {
        byte &= ~bit_mask;
    }
    ret = st25dv_write_byte(st25_address, address, byte);
    return ret;
}

esp_err_t st25dv_read_bit(uint8_t st25_address, uint16_t address, uint8_t bit_mask, bool *bit) 
{
    uint8_t byte = 0;
    esp_err_t ret;
    ret = st25dv_read_byte(st25_address, address, &byte);

    if (ret != ESP_OK) {
        return ret;
    }

    *bit = (byte & bit_mask);
    return ESP_OK;
}


esp_err_t st25dv_write(uint8_t st25_address, uint16_t reg_addr, uint8_t *data, size_t size) 
{
    i2c_op_t i2c_oper = {
        .reg_addr = reg_addr,
        .write_data = data,
        .write_size = size,
    };
    req.device_addr = st25_address;
    req_set_oper(&req, &i2c_oper, 1);
    return i2c_send_sync(&req, 2000);
}

esp_err_t st25dv_read(uint8_t st25_address, uint16_t address, uint8_t *data, size_t size) 
{
    i2c_op_t i2c_oper = {
        .reg_addr = address,
        .read_size = size,
        .user_ctx = data,
    };
    req.device_addr = st25_address;
    req_set_oper(&req, &i2c_oper, 1);
    return i2c_send_sync(&req, 2000);
}

esp_err_t st25dv_open_session(uint8_t st25_address, uint64_t password)
{
    uint8_t buffer[8 * 2 + 1] = {0};
    uint8_t *buffer_ptr = buffer + 9;
    memcpy(buffer, &password, sizeof(password));
    buffer[8] = 0x09;
    memcpy(buffer_ptr, &password, sizeof(password));
    return st25dv_write(st25_address, REG_I2C_PASSWD_BASE, buffer, 17);
}

esp_err_t st25dv_is_session_opened(uint8_t st25_address, bool *bit)
{
    return st25dv_read_bit(st25_address, REG_I2C_SSO_DYN, BIT_I2C_SSO_DYN_I2C_SSO, bit);
}






