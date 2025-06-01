#include "bgcp_debug.h"

#include "bgcp.h"
#include <stddef.h>


typedef struct {
	unsigned code;
	const char* desc;
} CodeDescription;


#define DEF_CODE(code)	 { code, #code },
static CodeDescription param_list[] = {
		PARAM_LOG_LIST
};
static CodeDescription func_list[] = {
		FUNC_LOG_LIST
};
static CodeDescription cmd_list[] = {
		CMD_LOG_LIST
};

static
const char *get_desc(unsigned code, CodeDescription *data, unsigned len)
{
	for(int i=0; i<len; i++){
		if(data[i].code == code) return data[i].desc;
	}
	return NULL;
}


const char *get_param_desc(unsigned code)
{
	return get_desc(code, param_list, ARR_LEN(param_list));
}

const char *get_func_desc(unsigned code)
{
	return get_desc(code, func_list, ARR_LEN(func_list));
}

const char *get_cmd_desc(unsigned code)
{
	return get_desc(code, cmd_list, ARR_LEN(cmd_list));
}

// generate  err message list
#define DEF_ERR(code)	 #code,
static const char* err_message[] = {
		ERR_CODE_LIST
};

const char *get_err_desc(unsigned err_status)
{
	if(err_status < BGCP_ERR_MAX){
		return err_message[err_status];
	}
	return "";
}


