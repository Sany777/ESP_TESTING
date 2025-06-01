#ifndef I2C_ASYNC_H
#define I2C_ASYNC_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#define I2C_TASK_PRIO 1
#define MAX_RETRY 3
#define I2C_QUEUE_LENGTH 10
#define I2C_MASTER_NUM 0
#define ACK_CHECK_EN 1
#define I2C_TIMEOUT_MS 1000
#define I2C_BUF_SIZE 512
#define I2C_QUEUE_LEN 5
#define I2C_TASK_STACK_SIZE 4096

typedef void (*cplt_callback_t)(esp_err_t, void * user_ctx, uint8_t *, size_t);

typedef struct {
    const uint8_t *write_data;      
    size_t write_size;    
    uint16_t reg_addr;            
    size_t read_size;   
    uint32_t post_delay;   
    cplt_callback_t op_cplt_callback;
    void * user_ctx;
} i2c_op_t;

typedef struct {
    int port;
    uint8_t device_addr;
    size_t op_count;  
    bool reg_addr_16b;
    cplt_callback_t tr_cplt_callback;
    esp_err_t res;
    i2c_op_t *ops;
} i2c_request_t;



void init_req(i2c_request_t *req, int port, uint8_t device_addr, size_t reg_addr_size, cplt_callback_t tr_cplt_callback);

void req_set_oper(i2c_request_t *req,  i2c_op_t *ops_list, uint16_t op_count);

bool i2c_init();

esp_err_t i2c_send_async(const i2c_request_t *req);

esp_err_t i2c_send_sync(const i2c_request_t *req, uint32_t ms);

void copy_callback(esp_err_t res, void *user_ctx, uint8_t *data, size_t size);


#endif // I2C_ASYNC_H