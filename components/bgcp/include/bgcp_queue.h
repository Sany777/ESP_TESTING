/*
 * The module is designed to create a chain of requests and responses.
 * Setting the required commands (such as page selection, data size, and new function) is handled automatically.
 * To initialize a new queue, call reset_req_queue().
 * It is possible to queue data using the following functions:
 * queue_push_param_req() – for requests without data,
 * queue_push_param_uint16_t_data() – for parameters with uint16_t data,
 * queue_push_param_uint8_t_data() – for parameters with uint8_t data.
  * To retrieve the length of the data to be transmitted, use get_req_queue_size(),
 * and to access the data itself, use get_req_queue_data().
 */
#ifndef BGCP_BGCP_REQUEST_QUEUE_H_
#define BGCP_BGCP_REQUEST_QUEUE_H_

#include "stdint.h"
#include "stdbool.h"


#define QUEUE_BUF_SIZE 512

typedef struct {
	uint8_t cur_func;
	uint8_t cur_page;
	uint16_t queue_size;
	uint8_t queue_buf[QUEUE_BUF_SIZE];
}BGCPQueue;

void queue_push_param_data(uint8_t func_num, uint16_t param_num, void *param_data, uint8_t size);
uint16_t get_req_queue_size(void);
void reset_req_queue(void);
uint8_t * get_req_queue_data(void);
void queue_push_param_req(uint8_t func_num, uint16_t param_num);
void queue_push_param_uint16_t_data(uint8_t func_num, uint16_t param_num, uint16_t param_data);
void queue_push_param_uint8_t_data(uint8_t func_num, uint16_t param_num, uint8_t param_data);
void queue_push_func_num(uint8_t func);
void queue_push_no_sup_param(uint16_t param_num);
void queue_push_page(uint16_t param_num);
void queue_push_byte(uint8_t data);








#endif /* BGCP_BGCP_REQUEST_QUEUE_H_ */
