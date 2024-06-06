#pragma once

class MFRC522{


};



int uart_init() ;
int uart_send(const void* data, const uint32_t data_size);
int uart_receive(void *buf, const uint32_t buf_size);