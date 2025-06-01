#include <bgcp_data_descriptor.h>

#include "bgcp_internal.h"
#include "bgcp.h"
#include <bgcp_queue.h>
#include "stdlib.h"
#include "string.h"



static void data_processing(bool need_update_local_data, bool need_response);
static void set_default_block_size(void);
static uint8_t read_byte(void);
static void link_data(void);
static void set_new_page(void);
static bool set_new_func(void);
static void set_data_size(void);
static bool check_data_in(void);
static void perform(uint8_t byte);
static void update_param_data_lifetime(uint32_t cur_time);
static void reset_param_data();

static ParserData parser;


/// @brief Parses the input data and processes BGCP commands.
void parse(uint8_t *data_in, uint16_t size)
{
	
#ifdef BGCP_DEBUG
	log_printf("\nparse %ub", size);
	parser.err_status = BGCP_OK;
#endif

	uint8_t byte;
	parser.data_in = data_in;
	parser.data_in_end = data_in + size;
	parser.page = 0;

	if( ! set_new_func()){
		return;
	}

	const uint32_t cur_time = GET_TIME();

	reset_req_queue();
	reset_param_data();

	while(parser.data_in < parser.data_in_end){
		byte = read_byte();
		if(byte == BGCP_CMD_PAGE){
			set_new_page();
		} else if(byte == BGCP_CMD_FUNC){
			set_new_func();
		} else if(byte == BGCP_CMD_SIZE){
			set_data_size();
		} else {
			perform(byte);
			update_param_data_lifetime(cur_time);
			reset_param_data();
		}
#ifdef BGCP_DEBUG
		if(parser.err_status != BGCP_OK){
			log_printf("<err: %s>", get_err_desc(parser.err_status));
			parser.err_status = BGCP_OK;
		}
#endif
	}

}


/// @brief Calls data processors for the current parameter.
static
void data_processing(bool need_update_local_data, bool need_response)
{
	const Descriptors *funcs = get_param_data_processor(parser.param);
	if(funcs){
		if(need_update_local_data && funcs->data_update_func){
			funcs->data_update_func();
		}
		if(need_response && funcs->resp_func){
			funcs->resp_func();
		}
	}
}

/// @brief sets data size after receiving BGCP_CMD_SIZE command
static
void set_data_size()
{
	const uint8_t  block_size = read_byte();
	if(parser.func_num == BGCP_FUNC_RW
			|| parser.func_num == BGCP_FUNC_W
			|| parser.func_num == BGCP_FUNC_RESP){
		parser.seted_data_size = block_size;
	}
}

/// @brief sets the default block size based on the function type
static
void set_default_block_size()
{
	if(parser.func_num == BGCP_FUNC_W
			|| parser.func_num == BGCP_FUNC_RW
			|| parser.func_num == BGCP_FUNC_RESP){
		parser.seted_data_size = DEFAULT_DATA_BLOCK_SIZE;
	} else {
		parser.seted_data_size = 0;
	}
}

/// @brief Links input data based on the function type
static
void link_data()
{
	if(parser.func_num == BGCP_FUNC_RW || parser.func_num == BGCP_FUNC_W || parser.func_num == BGCP_FUNC_RESP){
		parser.in_val = parser.data_in;
	}
	parser.in_data_size = parser.seted_data_size;
}

/// @brief sets a new page after receiving BGCP_CMD_PAGE command
void set_new_page()
{
	parser.page =  read_byte();
}

/// @brief Sets a new function based on the first byte or after receiving BGCP_CMD_FUNC command.
static
bool set_new_func()
{
	const uint8_t func = read_byte();

	bool is_valid_func = func == BGCP_FUNC_R
			|| func == BGCP_FUNC_RW
			|| func == BGCP_FUNC_DEC_RW
			|| func == BGCP_FUNC_INC_RW
			|| func == BGCP_FUNC_RESP
			|| func == BGCP_FUNC_W;


	if(func == BGCP_FUNC_DEC_RW){
		parser.inc_val = -1;
	} else	if(func == BGCP_FUNC_INC_RW){
		parser.inc_val = 1;
	} else {
		parser.inc_val = 0;
	}

	if(is_valid_func){
		parser.func_num = func;
	} else {
		parser.func_num = NO_FUNC;
	}

	set_default_block_size();

#ifdef BGCP_DEBUG

	const char *desc = get_func_desc(func);
	if(desc) log_printf("[F:%s]", desc);
	else log_printf("[?F:%u]", func);

#endif

	return is_valid_func;
}

/// @brief reads a byte from the input data buffer.
static
uint8_t read_byte()
{
	if(parser.data_in >= parser.data_in_end){
#ifdef BGCP_DEBUG
		set_err(BGCP_ERR_UNEXPECTED_DATA_END);
#endif
		return 0;
	}
	return  *(parser.data_in++);
}


uint16_t get_valid_val_list_len()
{
	if(parser.param && parser.param->data_type == PARAM_TYPE_8B_RANGE){
		return parser.param->validator.list_len;
	}
#ifdef BGCP_DEBUG
	set_err(BGCP_ERR_DATA_SIZE);
#endif
	return 0;
}

uint8_t * get_valid_val_list()
{
	if(parser.param && parser.param->data_type == PARAM_TYPE_8B_RANGE){
		return parser.param->validator.valid_values_list;
	}
#ifdef BGCP_DEBUG
	set_err(BGCP_ERR_DATA_SIZE);
#endif
	return NULL;
}


void * get_linked_data()
{
	if(parser.param){
		return parser.param->linked_data;
	}
#ifdef BGCP_DEBUG
	set_err(BGCP_ERR_DATA_SIZE);
#endif
	return NULL;
}

int get_inc_val()
{
	if(parser.param->data_type == PARAM_TYPE_ACTION){
#ifdef BGCP_DEBUG
		set_err(BGCP_ERR_DATA_FORMAT_ERR);
#endif
		return 0;
	}
	return parser.inc_val;
}

int get_min_num_param_val()
{
	if(parser.param->data_type == PARAM_TYPE_ACTION){
#ifdef BGCP_DEBUG
		set_err(BGCP_ERR_DATA_FORMAT_ERR);
#endif
		return 0;
	}
	return  parser.param->description.min;
}

int get_max_num_param_val()
{
	if(parser.param->data_type == PARAM_TYPE_ACTION){
#ifdef BGCP_DEBUG
		set_err(BGCP_ERR_DATA_FORMAT_ERR);
#endif
		return 0;
	}
	return  parser.param->description.max;
}

uint8_t get_input_data_size()
{
	if(parser.in_val == NULL) return 0;
	return parser.in_data_size;
}


bool copy_input_data(void *data_buf)
{
	if(parser.in_val && parser.in_data_size){
		memcpy(data_buf, parser.in_val, parser.in_data_size);
		return true;
	}
#ifdef BGCP_DEBUG
	set_err(BGCP_ERR_NO_DATA);
#endif
	return false;
}

void write_response(void * data, uint8_t size)
{
	queue_push_param_data(BGCP_FUNC_RESP, parser.param->param_num, data, size);
}

void set_err(int err)
{
	parser.err_status = err;
}

static
void update_param_data_lifetime(uint32_t cur_time)
{
	if(parser.param && is_write_func(parser.func_num)){
		parser.param->time_update = cur_time;
	}
}

static
bool check_data_in()
{
	if(parser.func_num == NO_FUNC){
		return false;
	}
	if(parser.data_in_end < (parser.data_in + parser.seted_data_size)){
#ifdef BGCP_DEBUG
		set_err(BGCP_ERR_UNEXPECTED_DATA_END);
#endif
		return false;
	}
	return true;
}

static
void reset_param_data()
{
	parser.param = NULL;
	parser.in_data_size = 0;
	parser.in_val = NULL;
}

uint16_t get_param_num()
{
	if(parser.param){
		return parser.param->param_num;
	}
	return NO_PARAM;
}

/// @brief Sets parameter data based on the given parameter number.
static
void perform(uint8_t byte)
{
	uint16_t param_num;
	uint8_t param_num_lbyte;
	bool perform_processing;
	
	const bool need_update_local_data = is_write_func(parser.func_num);
	const bool need_response =  is_read_func(parser.func_num);
	
	if(byte == BGCP_CMD_NOT_SUP){
		param_num_lbyte = read_byte();
		perform_processing  = false;
		parser.seted_data_size = 0;
#ifdef BGCP_DEBUG
		log_printf("[NOT_SUP][%u]", param_num_lbyte);
#endif
	} else {
		param_num_lbyte = byte;
		perform_processing = check_data_in();
	}

	param_num = (uint16_t) parser.page << 8 | param_num_lbyte;
	parser.param = get_param(param_num);
	if(parser.param == NULL){
		if(need_response){
			queue_push_no_sup_param(param_num);
		}
	} else if(perform_processing){
		link_data();
		data_processing(need_update_local_data, need_response);
	}

#ifdef BGCP_DEBUG

	const char *desc = get_param_desc(param_num);
	if(desc) log_printf("[P:%s]", desc);
	else log_printf("[?P%u]", param_num);

	if(parser.in_data_size == 1)	log_printf("[D:%u]", get_8b_val(parser.in_val));
	else if(parser.in_data_size == 2)	log_printf("[D:%u]", get_16b_val(parser.in_val));

#endif

	parser.data_in += parser.seted_data_size;
	set_default_block_size();
}
