#include <bgcp_data_descriptor.h>

#include <bgcp_queue.h>
#include "bgcp_protocol_const.h"
#include "bgcp_internal.h"
#include "string.h"
#include "bgcp.h"


// declaration of handlers
static void update_bool_data_type(void);
static void send_bool_data_type(void);
static void update_bin_data_type(void);
static void send_bin_data_type(void);
static void update_str_data_type(void);
static void send_str_data_type(void);
static void update_range_data(void);


bool bind_str_param(uint16_t param_num, char *string_buf, uint16_t min_data_len, uint16_t buf_size)
{
	return set_num_param(param_num, PARAM_TYPE_BIN, string_buf, min_data_len, buf_size);
}

bool bind_const_param(uint16_t param_num, const void *linked_data_buf, uint16_t data_size)
{
	return set_num_param(param_num, PARAM_TYPE_CONST_BIN, linked_data_buf,  0, data_size);
}

bool bind_binary_param(uint16_t param_num, void *linked_data, uint16_t data_size)
{
	return set_num_param(param_num, PARAM_TYPE_BIN, linked_data, 0, data_size);
}

bool bind_bool_param(uint16_t param_num, bool *bool_data)
{
	return set_num_param(param_num, PARAM_TYPE_1B, bool_data,  2, 0);
}

bool bind_cmd_param(uint16_t param_num, void(*executor)(void))
{	
	return set_action_param(param_num, NULL, executor, NULL);
}

bool bind_resp_param(uint16_t param_num, void(*responder)(void))
{
	return set_action_param(param_num, NULL, NULL, responder);
}
// example func prototype for uint8_t:
// bind_uint8_t_param(uint8_t *linked_data, int min, int max)
#define DEF_NUM_DATA(type_id, type) bool bind_##type##_param(uint16_t param_num, type * linked_data, type min, type max)	\
{																						\
	return set_num_param(param_num, type_id, linked_data,  min, max);	\
}

PARAM_NUM_TYPE_LIST
#undef DEF_NUM_DATA


const uint8_t * seek_val_in_range_list(uint8_t val, const uint8_t * valid_values_list, uint16_t list_len)
{
	for(int i=0; i<list_len; i++){
		if(valid_values_list[i] == val){
			return &valid_values_list[i];
		}
	}
	return NULL;
}

static
void update_range_data()
{
	const uint16_t list_len = get_valid_val_list_len();
	const uint8_t *valid_values_list = get_valid_val_list();
	uint8_t *linked_val = get_linked_data();
	uint8_t new_val;
	if(linked_val == NULL) return;
	if(get_input_data_size() == sizeof(uint8_t)){
		copy_input_data(&new_val);
		if(seek_val_in_range_list(new_val, valid_values_list, list_len)){
			*linked_val = new_val;
		}
	} else {
		int inc = get_inc_val();
		if(inc == 1 || inc == -1){
			const uint8_t *ptr = seek_val_in_range_list(*linked_val, valid_values_list, list_len);
			if(ptr == NULL) return;
			const uint8_t *start = &valid_values_list[0];
			const uint8_t *end = &valid_values_list[list_len-1];
			const uint8_t * next_val_ptr = ptr + inc;
			if(next_val_ptr >= start && next_val_ptr <= end){
				*linked_val = *next_val_ptr;
			}
		}
	}
}

static
void update_str_data_type(void)
{
	size_t len = get_input_data_size();
	char * data_buf = get_linked_data();
	if(len < get_max_num_param_val() && len >= get_min_num_param_val()){
		copy_input_data(data_buf);
		data_buf[len] = '\0';
	}
}

static
void send_str_data_type(void)
{
	write_response(get_linked_data(), strlen(get_linked_data()));
}

static
void update_bin_data_type(void)
{
	if(get_input_data_size() == get_max_num_param_val()){
		copy_input_data(get_linked_data());
	}
}

static
void send_bin_data_type(void)
{
	write_response(get_linked_data(), get_max_num_param_val());
}

static
void update_bool_data_type()
{
	uint8_t in_data;
	bool * val = (bool *)get_linked_data();
	if(val && get_input_data_size() == sizeof(uint8_t) && copy_input_data(&in_data)){
		if(in_data == SWITCH){
			* val = ! *val;
		} else if(in_data == OFF){
			* val = false;
		} else if(in_data == ON){
			* val = true;
		}
	}
}

static
void send_bool_data_type()
{
	write_response(get_linked_data(), sizeof(bool));
}

// generated set functions
#define DEF_NUM_DATA(code, type) void SET_FUNC_NAME(type)(void)	\
{ \
	type new_val; \
	type * val = (type *) get_linked_data(); \
	type max_val= get_max_num_param_val(); \
	type min_val = get_min_num_param_val(); \
	type inc_val = get_inc_val(); \
	if(val != NULL){ \
		if(inc_val != 0){ \
			new_val = *val + inc_val; \
			* val = new_val > max_val ? min_val  : new_val < min_val ? max_val : new_val; \
		} else if(get_input_data_size() == sizeof(type) && copy_input_data(&new_val)){ \
			* val = new_val > max_val ? max_val  : new_val < min_val ? min_val : new_val; \
		} \
	} \
}

PARAM_NUM_TYPE_LIST
#undef DEF_NUM_DATA

// example func prototype for uint8_t:
// send_uint8_t(uint8_t *linked_data, int min, int max)
#define DEF_NUM_DATA(type_id, type) void GET_FUNC_NAME(type)(void)	\
{ \
	write_response(get_linked_data(), sizeof(type)); \
}

PARAM_NUM_TYPE_LIST
#undef DEF_NUM_DATA



#define DEF_NUM_DATA(type_id, type)  { type_id, { SET_FUNC_NAME(type), GET_FUNC_NAME(type) } },
#define DEF_TYPE(type_id, seter, geter) { type_id, { seter, geter} },

 const ParamDescriptor data_type_descriptor_list [] = {
		MY_PARAM_TYPE_LIST
		PARAM_NUM_TYPE_LIST
};
#undef DEF_NUM_DATA


const Descriptors *get_descriptors_of_data_type(uint8_t data_type)
{
	for(int i=0; i<ARR_LEN(data_type_descriptor_list); i++){
		if(data_type_descriptor_list[i].data_type == data_type){
			return &data_type_descriptor_list[i].descriptors;
		}
	}
	return NULL;
}

uint8_t get_8b_val(uint8_t *data)
{
	if(!data) return 0;
	return (*(uint8_t*)data);
}

uint16_t get_16b_val(uint8_t *data)
{
	if(!data) return 0;
	return (*(uint8_t*)data) | (*(uint8_t*) (data+1) << 8 );
}