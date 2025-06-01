#include "bgcp.h"

#include "bgcp_internal.h"
#include "stdlib.h"



static Param param_list[PARAM_NUM];

extern const Descriptors *get_descriptors_of_data_type(uint8_t data_type);

const Descriptors *get_param_data_processor(Param *param)
{
	if(param->data_type == PARAM_TYPE_ACTION){
		return &param->dascriptors;
	}
	return get_descriptors_of_data_type(param->data_type);
}

Param *get_param(uint16_t param_num)
{
	for(int i=0; i<PARAM_NUM; i++){
		if(param_list[i].param_num == param_num){
			return &param_list[i];
		}
	}
	return NULL;
}

bool set_num_param(uint16_t param_num, uint8_t data_type, void *linked_data,  int min, int max)
{
	if(data_type == PARAM_TYPE_ACTION) return false;
	Param *param = get_param(param_num);
	if(param == NULL){
		param = get_param(NO_PARAM);
		if(param == NULL) return false;
		param->param_num = param_num;
	}
	param->linked_data = linked_data;
	param->data_type = data_type;
	param->description.min = min;
	param->description.max = max;
	param->time_update = 0;
	return true;
}

bool set_action_param(uint16_t param_num, void *linked_data, void(*data_update_func)(void), void(*resp_func)(void))
{
	Param *param = get_param(param_num);
	if(param == NULL){
		param = get_param(NO_PARAM);
		if(param == NULL) return false;
		param->param_num = param_num;
	}
	param->linked_data = linked_data;
	param->data_type = PARAM_TYPE_ACTION;
	param->dascriptors.resp_func = resp_func;
	param->dascriptors.data_update_func = data_update_func;
	param->time_update = 0;
	return true;
}


bool bind_ranked_param(uint16_t param_num, uint8_t *linked_data, uint8_t *valid_values, uint16_t list_len)
{
	Param *param = get_param(param_num);
	if(param == NULL){
		param = get_param(NO_PARAM);
		if(param == NULL) return false;
		param->param_num = param_num;
	}
	param->linked_data = linked_data;
	param->data_type = PARAM_TYPE_8B_RANGE;
	param->validator.valid_values_list = valid_values;
	param->validator.list_len = list_len;
	param->time_update = 0;
	return true;
}

uint32_t get_param_update_time(uint16_t param_num)
{
	Param * param = get_param(param_num);
	if(param){
		return param->time_update;
	}
	return 0;
}








