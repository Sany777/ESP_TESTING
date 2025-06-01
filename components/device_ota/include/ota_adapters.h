#pragma once

#include "ota_update_stream.h"
#include "esp_http_server.h"


void httpd_input_stream_init(ota_input_stream_t *stream, httpd_req_t *req);


void uart_input_stream_init(ota_input_stream_t *stream, int uart_num);
