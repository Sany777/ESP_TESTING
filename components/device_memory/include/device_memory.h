#pragma once


#include "stdbool.h"




bool init_nvs(void);
bool read_flash(const char* data_name, void *data_buf, unsigned data_size);
bool write_flash(const char* data_name, const void *data, unsigned data_size);