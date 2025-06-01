#ifndef BGCP_BGCP_DATA_DESCRIPTOR_H_
#define BGCP_BGCP_DATA_DESCRIPTOR_H_

#include "stdint-gcc.h"
#include "stdbool.h"




/* оголошення використовуваних стандартних числових типів даних
 * параметр 1 - назва типу даних, довільна назва
 * параметр 2 - тип числових даних - uin8_t, uint16_t,float,...
 * example:
	DEF_NUM_DATA(PARAM_TYPE_FLOAT, float)
	DEF_NUM_DATA(PARAM_TYPE_UINT32, uint32_t) ...
 */
#define PARAM_NUM_TYPE_LIST	 \
	DEF_NUM_DATA(PARAM_TYPE_8B, uint8_t) \
	DEF_NUM_DATA(PARAM_TYPE_16B, uint16_t)	


/* звязування типів даних і обробників
 * параметр 1 - назва типу даних, довільна назва;
 * параметр 2 - функція для зміни зв'язаних даних,
 *  	викликається при обробці вхідних даних (BGCP_FUNC_W, BGCP_FUNC_RW,BGCP_FUNC_INC_RW, BGCP_FUNC_DEC_RW)
 * параметр 3 - функція викликається при формуванні відповіді
*  		передбачає поміщення даних у буфер через виклик write_response()
*  		викликається при BGCP_FUNC_R, BGCP_FUNC_RW, BGCP_FUNC_INC_RW, BGCP_FUNC_DEC_RW
 */
#define MY_PARAM_TYPE_LIST \
	DEF_TYPE(PARAM_TYPE_1B, update_bool_data_type, send_bool_data_type)	\
	DEF_TYPE(PARAM_TYPE_BIN, update_bin_data_type, send_bin_data_type)	\
	DEF_TYPE(PARAM_TYPE_CONST_BIN, NULL, send_bin_data_type)			\
	DEF_TYPE(PARAM_TYPE_STR, update_str_data_type, send_str_data_type)	\
	DEF_TYPE(PARAM_TYPE_8B_RANGE, update_range_data, send_uint8_t)



#define DEF_NUM_DATA(code, type) code,
#define DEF_TYPE(code, seter, geter) code,
enum {
	PARAM_NUM_TYPE_LIST
	MY_PARAM_TYPE_LIST
	PARAM_TYPE_ACTION,
	PARAM_TYPE_UNKNOWN
};
#undef DEF_NUM_DATA
#undef DEF_TYPE


// оголошення згенерованих функцій встановлення
// даних для параметрів тих типів даних що визначені в PARAM_NUM_TYPE_LIST
// set_##type##_param -> set_uint16_t_param
#define DEF_NUM_DATA(code, type)	\
	extern  bool set_##type##_param(uint16_t param_num, type *data, type min, type max);

PARAM_NUM_TYPE_LIST
#undef DEF_NUM_DATA

bool bind_const_param(uint16_t param_num, const void *linked_data_buf, uint16_t data_size);
bool bind_binary_param(uint16_t param_num, void *linked_data_buf, uint16_t data_size);
bool bind_str_param(uint16_t param_num, char *string_buf, uint16_t min_data_len, uint16_t buf_size);
bool bind_bool_param(uint16_t param_num, bool *bool_data);
bool bind_cmd_param(uint16_t param_num, void(*executor)(void));
bool bind_resp_param(uint16_t param_num, void(*responder)(void));

// GET_FUNC_NAME(uint8_t) -> send_uint8_t_func
// SET_FUNC_NAME(uint16_t) -> update_uint16_t_func
#define GET_FUNC_NAME(type) send_##type
#define SET_FUNC_NAME(type) update_##type

#define DEF_NUM_DATA(code, type)	   \
void GET_FUNC_NAME(type)(void);	\
void SET_FUNC_NAME(type)(void);\

PARAM_NUM_TYPE_LIST
#undef DEF_NUM_DATA


// usefull tools
const uint8_t * seek_val_in_range_list(uint8_t val, const uint8_t * valid_values_list, uint16_t list_len);
uint8_t get_8b_val(uint8_t *data);
uint16_t get_16b_val(uint8_t *data);







#endif /* BGCP_BGCP_DATA_DESCRIPTOR_H_ */
