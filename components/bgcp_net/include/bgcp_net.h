#pragma once


#include "stdbool.h"
#include "stdint.h"


#define MIN_BGCP_MES_SIZE 10

bool check_bgcp_protocol(uint8_t *data, uint16_t size);