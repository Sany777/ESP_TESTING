#include "device_i2c.h"

#include "event_manager.h"
#include <string.h>
#include "device.h"
#include "driver/i2c.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "string.h"



// check the data size of i2c_cmd_link_t and i2c_cmd_desc_t before use
#define RESERVE 100
#define MAX_CMD_SUPPORT 10
#define CMD_DESC_SIZE (5*sizeof(uint32_t))
#define CMD_LINK_SIZE (5*sizeof(uint32_t))
#define I2C_TRANS_BUF_MINIMUM_SIZE (CMD_DESC_SIZE + CMD_LINK_SIZE * MAX_CMD_SUPPORT + RESERVE)

static uint8_t i2c_cmd_buf[I2C_TRANS_BUF_MINIMUM_SIZE];
static StaticQueue_t i2c_queue_buffer; 
static StaticTask_t task_buffer; 
static StackType_t task_stack[I2C_TASK_STACK_SIZE];
static uint8_t i2c_queue_storage[I2C_QUEUE_LEN * sizeof(i2c_request_t)]; 

static uint8_t i2c_buf[I2C_BUF_SIZE];
static QueueHandle_t i2c_queue;

static esp_err_t device_i2c_master_write_to_device(i2c_port_t i2c_num, 
                                                    uint8_t device_address, 
                                                    uint16_t reg_addr, 
                                                    bool reg_16b_format, 
                                                    const uint8_t* write_buffer, 
                                                    size_t write_size);
    
static esp_err_t device_i2c_master_read_from_device(i2c_port_t i2c_num, 
                                                    uint8_t device_address, 
                                                    uint16_t reg_addr, 
                                                    bool reg_16b_format, 
                                                    bool two_step_reading,
                                                    uint8_t* read_buffer, 
                                                    size_t read_size);
        
extern i2c_cmd_handle_t i2c_cmd_link_create_static(uint8_t* i2c_cmd_buf, uint32_t size);


static
void notification_callback(esp_err_t res, i2c_request_t *req, i2c_op_t *op)
{
    size_t len = (res == ESP_OK) ? op->read_size : 0;
    if(op->op_cplt_callback != NULL){
        op->op_cplt_callback(res, op->user_ctx, i2c_buf, len);
    } else if(req->tr_cplt_callback != NULL){
        req->tr_cplt_callback(res, op->user_ctx, i2c_buf, len);
    }
}

static
void i2c_task(void *arg) 
{
    i2c_request_t req;

    for (;;) {
        if (xQueueReceive(i2c_queue, &req, portMAX_DELAY) == pdTRUE){

            int fail_count = 0;
            i2c_op_t * op = req.ops;
            const i2c_op_t * end = req.ops + req.op_count;
            esp_err_t result = ESP_OK;
            while(op < end){

                size_t read_data_size = op->read_size;
                
                if(read_data_size > I2C_BUF_SIZE 
                        || (op->write_size != 0 && op->write_data == NULL)){
                    notification_callback(ESP_ERR_INVALID_ARG, &req, op);
                    device_set_bits(BIT_I2C_OPER_ERR);
                    op++;
                    continue;
                }

                bool two_step_reading = op->post_delay > 0 && read_data_size != 0;

                if(two_step_reading || op->write_size != 0){
                    result = device_i2c_master_write_to_device(req.port, 
                                                            req.device_addr,
                                                            op->reg_addr,
                                                            req.reg_addr_16b,
                                                            op->write_data,
                                                            op->write_size);
                }
                if (op->post_delay > 0){
                    vTaskDelay(pdMS_TO_TICKS(op->post_delay));
                }
                if(read_data_size != 0){
                    result = device_i2c_master_read_from_device(req.port, 
                                                            req.device_addr,
                                                            op->reg_addr,
                                                            req.reg_addr_16b,
                                                            two_step_reading,
                                                            i2c_buf,
                                                            read_data_size);
                }
    
                if(result == ESP_OK || fail_count >= MAX_RETRY){
                    notification_callback(result, &req, op);
                }
                    
                if(result == ESP_OK){
                    op++;
                    fail_count = 0;
                } else  if(fail_count < MAX_RETRY){
                    fail_count++;
                } else {
                    break; 
                } 
            }
            if(uxQueueMessagesWaiting(i2c_queue) == 0){
                device_set_bits(BIT_I2C_OPER_CPLT);
            }
        }
    }
}

void init_req(i2c_request_t *req, int port, uint8_t device_addr, size_t reg_addr_size, cplt_callback_t tr_cplt_callback)
{
    req->port = port;
    req->device_addr = device_addr;
    req->tr_cplt_callback = tr_cplt_callback;
    req->op_count = 0;
    req->ops = NULL;
    req->reg_addr_16b = reg_addr_size == sizeof(uint16_t);
}

void req_set_oper(i2c_request_t *req,  i2c_op_t *ops_list, uint16_t op_count)
{
    req->ops = ops_list;
    req->op_count = op_count;  
}


esp_err_t i2c_send_async(const i2c_request_t *req) 
{
    if (!req || !req->ops || req->op_count == 0) return ESP_ERR_INVALID_ARG;
    BaseType_t task_woken;
    if (xQueueSendFromISR(i2c_queue, req, &task_woken) != pdTRUE) return ESP_FAIL;
    portYIELD_FROM_ISR(task_woken);
    return ESP_OK;
}


esp_err_t i2c_send_sync(const i2c_request_t *req, uint32_t ms)
{
    device_clear_bits(BIT_I2C_OPER_CPLT|BIT_I2C_OPER_ERR);
    esp_err_t e = i2c_send_async(req);
    if(e == ESP_OK){
        unsigned bits = device_wait_bits(BIT_I2C_OPER_CPLT, ms);
        if(! (bits &BIT_I2C_OPER_CPLT )) e = ESP_ERR_TIMEOUT;
    }
    return e;
}

bool i2c_init() 
{
    if(i2c_queue != NULL) return true;
    i2c_queue = xQueueCreateStatic(I2C_QUEUE_LEN, sizeof(i2c_request_t), i2c_queue_storage, &i2c_queue_buffer);
    assert(i2c_queue != NULL);
    xTaskCreateStatic(
		i2c_task,  
		"i2c_task",   
		I2C_TASK_STACK_SIZE,     
		NULL, 
		I2C_TASK_PRIO,
		task_stack,
		&task_buffer
	);
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_PIN,
        .scl_io_num = I2C_MASTER_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = CONFIG_I2C_CLK_SPEED_HZ,
    };
    CHECK_RET_FALSE(i2c_param_config(I2C_MASTER_NUM, &conf));
    CHECK_RET_FALSE(i2c_driver_install(
                        I2C_MASTER_NUM, 
                        conf.mode, 
                        I2C_MASTER_RX_BUF_DISABLE, 
                        I2C_MASTER_TX_BUF_DISABLE, 
                        0)
                    );
    return true;
}


static
esp_err_t device_i2c_master_write_to_device(i2c_port_t i2c_num, 
                                        uint8_t device_address, 
                                        uint16_t reg_addr, 
                                        bool reg_16b_format, 
                                        const uint8_t* write_buffer, 
                                        size_t write_size)
{
    i2c_cmd_handle_t handle = i2c_cmd_link_create_static(i2c_cmd_buf, sizeof(i2c_cmd_buf));
    if (handle == NULL) return ESP_ERR_NO_MEM;
    CHECK_RET_ERR(i2c_master_start(handle));
    CHECK_RET_ERR(i2c_master_write_byte(handle, (device_address << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN));
    if (reg_16b_format){
        CHECK_RET_ERR(i2c_master_write_byte(handle, reg_addr >> 8, ACK_CHECK_EN));
    }
    CHECK_RET_ERR(i2c_master_write_byte(handle, reg_addr & 0xFF, ACK_CHECK_EN));
    if (write_buffer != NULL && write_size > 0) {
        CHECK_RET_ERR(i2c_master_write(handle, write_buffer, write_size, ACK_CHECK_EN));
    }
    CHECK_RET_ERR(i2c_master_stop(handle));
    return i2c_master_cmd_begin(i2c_num, handle, pdMS_TO_TICKS(1000));
}


static
esp_err_t device_i2c_master_read_from_device(i2c_port_t i2c_num, 
                                          uint8_t device_address, 
                                          uint16_t reg_addr, 
                                          bool reg_16b_format, 
                                          bool two_step_reading,
                                          uint8_t* read_buffer, size_t read_size)
{
    i2c_cmd_handle_t handle = i2c_cmd_link_create_static(i2c_cmd_buf, sizeof(i2c_cmd_buf));
    if (handle == NULL) return ESP_ERR_NO_MEM;
    CHECK_RET_ERR(i2c_master_start(handle));
    if (!two_step_reading) {
        CHECK_RET_ERR(i2c_master_write_byte(handle, (device_address << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN));
        if (reg_16b_format) {
            CHECK_RET_ERR(i2c_master_write_byte(handle, reg_addr >> 8, ACK_CHECK_EN));
        }
        CHECK_RET_ERR(i2c_master_write_byte(handle, reg_addr & 0xFF, ACK_CHECK_EN));
        CHECK_RET_ERR(i2c_master_start(handle));  
    }
    CHECK_RET_ERR(i2c_master_write_byte(handle, (device_address << 1) | I2C_MASTER_READ, ACK_CHECK_EN));
    CHECK_RET_ERR(i2c_master_read(handle, read_buffer, read_size, I2C_MASTER_LAST_NACK));
    CHECK_RET_ERR(i2c_master_stop(handle));
    return i2c_master_cmd_begin(i2c_num, handle, pdMS_TO_TICKS(1000));
}


void copy_callback(esp_err_t res, void *user_ctx, uint8_t *data, size_t size)
{
    if(res == ESP_OK && user_ctx != NULL && data != NULL){
        memcpy(user_ctx, data, size);
    }
}