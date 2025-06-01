#pragma once


#include "stdbool.h"
#include "stdint.h"


typedef struct {
	void(*data_update_func)(void);
	void(*resp_func)(void);
}Descriptors;

typedef struct {
	int min;
	int max;
}NumDescription;

typedef struct {
	uint16_t list_len;
	uint8_t *valid_values_list;
}ValidatorData;

typedef struct {
	uint16_t param_num;
    uint8_t data_type;
    void * linked_data;
    uint32_t time_update;
	union{
    	NumDescription description;
    	Descriptors dascriptors;
		ValidatorData validator;
	};
}Param;

typedef struct{
	uint8_t data_type;
	Descriptors descriptors;
}ParamDescriptor;

typedef struct {
	uint16_t param_num;
	unsigned err_status;
	unsigned data_out_indx;
	void * in_val;
	Param *param;
	unsigned func_num;
	uint8_t *data_in;
	uint8_t *data_in_end;
	uint8_t page;
	int inc_val;
	unsigned seted_data_size;
	unsigned in_data_size;
}ParserData;

Param *get_param(uint16_t param_num);
const Descriptors *get_param_data_processor(Param *param);
bool set_num_param(uint16_t param_num, uint8_t data_type, void *linked_data,  int min, int max);


#define DEFAULT_DATA_BLOCK_SIZE 1



#define is_write_func(func_num) \
	((func_num) != BGCP_FUNC_R && (func_num) != NO_FUNC)

#define is_read_func(func_num)\
		(func_num == BGCP_FUNC_R	\
				|| func_num == BGCP_FUNC_RW	\
				|| func_num == BGCP_FUNC_DEC_RW	\
				|| func_num == BGCP_FUNC_INC_RW)