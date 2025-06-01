#include "http_json_helper.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_http_server.h"

/**
 * This module provides utility structures and functions to configure
 * JSON data and their fields without using
 * dynamic memory allocation.
 */

 bool add_json_field(json_field_cfg_t *cfg, const char *field_name, json_field_type_t type, void *ptr) 
 {
     if (!cfg || !field_name || !ptr) return false;
     if (cfg->count >= cfg->capacity) {
        ESP_LOGE("add_json_field()",  "No field added for %s", field_name);
        return false;  
    }
    if(cfg->update_last){
        cfg->start_data_indx = cfg->count;
        cfg->update_last = false;
    }
    json_field_t *field = &cfg->data[cfg->count++];
    field->name = field_name;
    field->type = type;
    field->ptr = ptr;
     return true;
 }
 
 bool add_json_endpoint(json_config_t *cnf,  const char *uri, int method, json_field_cfg_t *field_cfg) 
 {
     if (!cnf || !uri || !field_cfg) return false;
     if (cnf->count >= cnf->capacity) return false;
     if (field_cfg->count == field_cfg->start_data_indx) {
         ESP_LOGE("add_json_endpoint()", "No fields added for endpoint %s", uri);
         return false;
     }
 
     json_http_handler_t *handler = &cnf->data[cnf->count++];
     handler->uri = uri;
     handler->fields = &field_cfg->data[field_cfg->start_data_indx];
     handler->field_count = field_cfg->count - field_cfg->start_data_indx;
     handler->method = method;
     field_cfg->update_last = true;
     return true;
 }