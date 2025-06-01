#pragma once

#include "esp_log.h"
#include "esp_err.h"
#include "stddef.h"
#include "esp_check.h"
#include "string.h"
#include "stdint.h"



#define IS_EQUAL(buf1, buf2) (memcmp((buf1), (buf2), sizeof(*(buf1))) == 0)


#define IPV4_ADDR(a, b, c, d) ((uint32_t)(((a) & 0xff) << 24 | ((b) & 0xff) << 16 | ((c) & 0xff) << 8 | ((d) & 0xff)))


#ifndef MIN
	#define MIN(_a, _b) ((_a)>(_b) ? (_b) : (_a))
#endif

#ifndef MAX
	#define MAX(_a, _b) ((_a)>(_b) ? (_a) : (_b))
#endif

#ifndef ARR_LEN
	#define ARR_LEN(_arr) (sizeof(_arr)/sizeof(_arr[0]))
#endif

#define CHECK_RET_ERR(result_) \
    do{ \
        const esp_err_t e = result_; \
        if(e != ESP_OK){ ESP_LOGE(__func__, "%s", esp_err_to_name(e)); return e; } \
    }while(0)

#define CHECK_AND_GOTO(result_, label_) \
    do{ \
        const esp_err_t e = result_; \
        if(e != ESP_OK){ ESP_LOGE(__func__, "%s", esp_err_to_name(e)); goto label_; } \
    }while(0)

#define CHECK_RET(err_) \
    do{ \
        const esp_err_t e = err_; \
        if(e != ESP_OK){ ESP_LOGE(__func__, "%s", esp_err_to_name(e)); return; } \
    }while(0)

#define CHECK_RET_FALSE(err_) \
do{ \
    const esp_err_t e = err_; \
    if(e != ESP_OK){ ESP_LOGE(__func__, "%s", esp_err_to_name(e)); return false; } \
}while(0)


uint32_t compute_broadcast(uint32_t ip, uint32_t netmask);

int cpy_str(void *dst, const void *src, int dst_buf_size);

void get_serial_number(char serial_str[17]);








