extern "C"{
    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/uart.h"

}

#include "help_tools.hpp"
#include "MFRC522.hpp"
#include <cstring>
#define TXD_PIN (UART_PIN_NO_CHANGE) // GPIO1 - TXD
#define RXD_PIN (UART_PIN_NO_CHANGE) // GPIO3 - RXD
#define   DRV_BUF_SIZE (1000)

int uart_init() 
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    CHECK_ESPERR_RET_ERR(uart_param_config(UART_NUM_0, &uart_config));
    CHECK_ESPERR_RET_ERR(uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    return uart_driver_install(UART_NUM_0, DRV_BUF_SIZE * 2, 0, 0, NULL, 0);
}

int uart_send(const void* data, const uint32_t data_size) 
{
    return uart_write_bytes(UART_NUM_0, data, data_size);
}

int uart_receive(void *buf, const uint32_t buf_size) 
{
    return uart_read_bytes(UART_NUM_0, buf, MIN(buf_size, DRV_BUF_SIZE), 100 / portTICK_PERIOD_MS);
}