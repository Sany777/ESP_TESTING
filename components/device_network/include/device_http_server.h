#ifndef SETTING_SERVER_H
#define SETTING_SERVER_H

#include "esp_http_server.h"
#include <stddef.h>
#include "stdbool.h"
#include "esp_err.h"
#include "device.h"


#define HTTP_SERV_BUF_SIZE 5096
#define MAX_OTA_SIZE 0x120000 

esp_err_t admin_enable_handler(httpd_req_t *req);
esp_err_t get_favicon_handler(httpd_req_t *req);
esp_err_t get_index_handler(httpd_req_t *req);
esp_err_t get_css_handler(httpd_req_t *req);
esp_err_t get_script_handler(httpd_req_t *req);
esp_err_t ota_handler(httpd_req_t *req);
esp_err_t filter_reset_handler(httpd_req_t *req);
esp_err_t exit_handler(httpd_req_t *req);


bool init_http_server(device_data_t *device);
void stop_http_server(void);







void stop_http_server();









#endif