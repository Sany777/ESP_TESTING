#ifndef HTTP_JSON_HELPER_H
#define HTTP_JSON_HELPER_H


#include <esp_err.h>
#include <stdbool.h>
#include "stddef.h"
#include "esp_http_server.h"

#define NO_CPLT_CALLBACK NULL
#define NO_CTX NULL
#define NO_FIELDS NULL

typedef enum {
    JSON_FIELD_INT,
    JSON_FIELD_BOOL,
    JSON_FIELD_STR,
} json_field_type_t;


typedef struct {
    const char *name;
    void *ptr;
    size_t max_len; 
    json_field_type_t type;
} json_field_t;



typedef struct {
    const char *uri;
    int method;
    json_field_t *fields;  
    int field_count;    
} json_http_handler_t;




/**
 * Provide utility structures and functions to configure
 * JSON data and their fields without using
 * dynamic memory allocation.
 */

 typedef struct {
    json_field_t *data;
    int capacity;
    int count;
    int start_data_indx; 
    bool update_last;
} json_field_cfg_t;


typedef struct {
    json_http_handler_t *data; 
    int capacity;
    int count;
} json_config_t;


bool add_json_field(json_field_cfg_t *cfg, const char *field_name, json_field_type_t type, void *ptr);
bool add_json_endpoint(json_config_t *cnf, const char *uri, int method, json_field_cfg_t *cfg);

                            
#define INIT_JSON_CFG(cfg, buf, members) \
do{ \
    memset(&cfg, 0, sizeof(cfg)); \
    cfg.data = buf;  \
    cfg.capacity = members;   \
}while(0)



esp_err_t register_json_handler(httpd_handle_t server, json_http_handler_t*handler);
bool add_json_endpoint(json_config_t *cnf, const char *uri, int method, json_field_cfg_t *field_cfg);






#endif // HTTP_JSON_HELPER_H
