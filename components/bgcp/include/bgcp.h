/*
*The module provides functions to initialize the parameter table for the BGP protocol parser
* and implements handlers for reading and response functions.
* To bind parameters and data, use set_uint8_t_param(uint8_t *linked_data, int min, int max) for 8-bit values and
* set_uint16_t_param(uint16_t *linked_data, int min, int max) for 16-bit values.
 * For special handlers related to specific actions, such as resetting an error or handling custom data types, use set_action_param()
* set_func is called for BGCP_FUNC_W, BGCP_FUNC_RW, etc,
 * and get_func for reading (BGCP_FUNC_R, BGCP_FUNC_RW, etc.)
 */
#ifndef BGCP_BGCP_H_
#define BGCP_BGCP_H_

#include <bgcp_queue.h>
#include <bgcp_data_descriptor.h>
#include <bgcp_protocol_const.h>
#include "stdint.h"
#include "stdbool.h"

#define GET_TIME() 0
//#define BGCP_DEBUG

#ifdef BGCP_DEBUG
#include "bgcp_debug.h"
#endif

#define BGCP_OK 0

#define PARAM_NUM 15

#define NO_PARAM 0
#define NO_FUNC 0



void parse(uint8_t *data_in, uint16_t size);
bool set_action_param(uint16_t param_num, void *linked_data, void(*data_update_func)(void), void(*resp_func)(void));
bool bind_ranked_param(uint16_t param_num, uint8_t *linked_data, uint8_t *valid_values, uint16_t list_len);


int get_inc_val(void);
int get_min_num_param_val(void);
int get_max_num_param_val(void);
void set_err(int err);
void * get_linked_data(void);
bool copy_input_data(void *data_buf);
void write_response(void * data, uint8_t size);
uint16_t get_param_num(void);
uint8_t get_input_data_size(void);
uint32_t get_param_update_time(uint16_t param_num);
uint16_t get_valid_val_list_len(void);
uint8_t * get_valid_val_list(void);




#ifndef ARR_LEN
	#define ARR_LEN(_arr) (sizeof(_arr) / sizeof(_arr[0]))
#endif



#endif /* BGCP_BGCP_H_ */
