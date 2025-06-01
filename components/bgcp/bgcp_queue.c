#include <bgcp_queue.h>

#include "bgcp_protocol_const.h"
#include "stdlib.h"
#include "string.h"

static BGCPQueue rq;

static void queue_push_req_data(uint8_t *data, uint8_t  size);

/**
 * Reset the request queue to its initial state.
 */
void reset_req_queue()
{
	rq.cur_page = 0;
	rq.cur_func = 0;
	rq.queue_size = 0;
}

/**
 * Get the current size of the request queue.
 * @return Queue size.
 */
uint16_t get_req_queue_size()
{
	return rq.queue_size;
}

/**
 * Get a pointer to the request queue buffer.
 * @return Pointer to the queue buffer.
 */
uint8_t * get_req_queue_data()
{
	return rq.queue_buf;
}

/**
 * Push a single byte to the request queue.
 * @param data Byte to add.
 */
void queue_push_byte(uint8_t data)
{
	queue_push_req_data(&data, 1);
}

/**
 * Push an array of bytes to the request queue.
 * @param data Pointer to data array.
 * @param size Number of bytes to add.
 */
static
void queue_push_req_data(uint8_t *data, uint8_t size)
{
	if(QUEUE_BUF_SIZE > rq.queue_size + size){
		for(int i = 0; i < size; i++){
			rq.queue_buf[rq.queue_size++] = data[i];
		}
	}
}

/**
 * Push a page number to the request queue if it has changed.
 * @param param_num Parameter number determining the page.
 */
void queue_push_page(uint16_t param_num)
{
	uint8_t page = param_num >> 8;
	if(page != rq.cur_page){
		queue_push_byte(BGCP_CMD_PAGE);
		queue_push_req_data(&page, 1);
		rq.cur_page = page;
	}
}

/**
 * Push a parameter with associated data to the request queue.
 * @param func_num Function number.
 * @param param_num Parameter number.
 * @param param_data Pointer to parameter data.
 * @param size Size of parameter data.
 */
void queue_push_param_data(uint8_t func_num, uint16_t param_num, void *param_data, uint8_t size)
{
	if(param_data == NULL && size != 0) return;
	uint8_t param_lbyte = param_num;
	queue_push_func_num(func_num);
	queue_push_page(param_num);
	if(size > 1){
		queue_push_byte(BGCP_CMD_SIZE);
		queue_push_req_data(&size, 1);
	}
	queue_push_req_data(&param_lbyte, 1);
	if(size){
		queue_push_req_data(param_data, size);
	}
}

/**
 * Push a request for a parameter without sending data.
 * @param func Function number.
 * @param param_num Parameter number.
 */
void queue_push_param_req(uint8_t func_num, uint16_t param_num)
{
	queue_push_param_data(func_num, param_num, NULL, 0);
}

/**
 * Push a 16-bit parameter value to the request queue.
 * @param func Function number.
 * @param param_num Parameter number.
 * @param param_data 16-bit parameter value.
 */
void queue_push_param_uint16_t_data(uint8_t func_num, uint16_t param_num, uint16_t param_data)
{
	queue_push_param_data(func_num, param_num, (uint8_t *)&param_data, sizeof(uint16_t));
}

/**
 * Push an 8-bit parameter value to the request queue.
 * @param func Function number.
 * @param param_num Parameter number.
 * @param param_data 8-bit parameter value.
 */
void queue_push_param_uint8_t_data(uint8_t func_num, uint16_t param_num, uint8_t param_data)
{
	queue_push_param_data(func_num, param_num, &param_data, sizeof(uint8_t));
}

/**
 * Push a function number to the request queue if it has changed.
 * @param func Function number.
 */
void queue_push_func_num(uint8_t func_num)
{
	if(func_num != rq.cur_func){
		if(rq.queue_size != 0){
			queue_push_byte(BGCP_CMD_FUNC);
		}
		queue_push_req_data(&func_num, 1);
		rq.cur_func = func_num;
	}
}

void queue_push_no_sup_param(uint16_t param_num)
{
	queue_push_func_num(BGCP_FUNC_RESP);
	queue_push_page(param_num);
	queue_push_byte(BGCP_CMD_NOT_SUP);
	queue_push_byte(param_num);
}
