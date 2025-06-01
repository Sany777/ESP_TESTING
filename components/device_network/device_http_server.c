#include "device_http_server.h"
#include "http_json_helper.h"

#include "esp_http_server.h"
#include "device.h"

#include <sys/stat.h>
#include "string.h"
#include "portmacro.h"

#include "ota_update_stream.h"
#include "ota_adapters.h"

static const char *TAG = "HTTP_SERVER";


DEF_EMB_FILE(favicon_ico);
DEF_EMB_FILE(index_html);
DEF_EMB_FILE(style_min_css);
DEF_EMB_FILE(script_min_js);



static httpd_handle_t http_server;


#define JSON_HANDLERS_NUM 15
#define JSON_FIELD_NUM 25

static json_http_handler_t json_handlers[JSON_HANDLERS_NUM];
static json_field_t json_fields[JSON_FIELD_NUM];

typedef struct{
    const char *uri;
    httpd_method_t method;
    esp_err_t (*handler)(httpd_req_t *req);
}uri_data_t;

static const uri_data_t uri_list[] = {
    {
        .uri     = "/exit",
        .method  = HTTP_GET,
        .handler = exit_handler
    },
    {
        .uri     = "/admin",
        .method  = HTTP_GET,
        .handler = admin_enable_handler
    },
    {
        .uri     = "/filter_reset",
        .method  = HTTP_GET,
        .handler = filter_reset_handler
    },
    {
        .uri     = "/update",
        .method  = HTTP_POST,
        .handler = ota_handler
    },
    {
        .uri     = "/favicon.ico",
        .method  = HTTP_GET,
        .handler = get_favicon_handler
    },
    {
        .uri     = "/style.css",
        .method  = HTTP_GET,
        .handler = get_css_handler
    },
    {
        .uri     = "/script.js",
        .method  = HTTP_GET,
        .handler = get_script_handler
    },
    {
        .uri     = "/",
        .method  = HTTP_GET,
        .handler = get_index_handler
    },
};

#define STATIC_URI_NUM ARR_LEN(uri_list)
#define HANDLERS_NUM (JSON_HANDLERS_NUM + STATIC_URI_NUM)


void register_static_uri_handlers(device_data_t *device)
{
    httpd_uri_t uri_handler;
    for (int i = 0; i < STATIC_URI_NUM; i++) {
        uri_handler.uri = uri_list[i].uri;
        uri_handler.method = uri_list[i].method;
        uri_handler.handler = uri_list[i].handler;
        uri_handler.user_ctx = device;
        esp_err_t err = httpd_register_uri_handler(http_server, &uri_handler);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to register URI for %s", uri_list[i].uri);
            break;
        }
    }
}




void register_json_api_handlers(device_data_t * device) 
{
    json_field_cfg_t json_fields_cfg;
    json_config_t json_cfg;

    INIT_JSON_CFG(json_fields_cfg, json_fields, JSON_FIELD_NUM);
    INIT_JSON_CFG(json_cfg, json_handlers, JSON_HANDLERS_NUM);

    add_json_field(&json_fields_cfg, "humidity_cur", JSON_FIELD_INT, &device->status.humidity);
    add_json_field(&json_fields_cfg, "motion_cur", JSON_FIELD_BOOL, &device->status.motion);
    add_json_field(&json_fields_cfg, "lt1", JSON_FIELD_BOOL, &device->status.lt1);
    add_json_field(&json_fields_cfg, "lt2", JSON_FIELD_BOOL, &device->status.lt2);
    add_json_field(&json_fields_cfg, "fan_speed", JSON_FIELD_INT, &device->status.fan_speed);
    add_json_field(&json_fields_cfg, "admin", JSON_FIELD_BOOL, &device->status.admin_en);
    add_json_field(&json_fields_cfg, "info", JSON_FIELD_STR, device->status.info);
    add_json_field(&json_fields_cfg, "title", JSON_FIELD_STR, device->status.title);
    add_json_endpoint(&json_cfg,  "/status", HTTP_GET, &json_fields_cfg); 

    add_json_field(&json_fields_cfg, "wifi_en", JSON_FIELD_BOOL, &device->config.wifi_en);
    add_json_endpoint(&json_cfg,  "/wifi", HTTP_POST, &json_fields_cfg); 
    add_json_endpoint(&json_cfg,  "/wifi", HTTP_GET, &json_fields_cfg); 

    add_json_field(&json_fields_cfg, "off_timer", JSON_FIELD_INT, &device->config.off_timer);
    add_json_field(&json_fields_cfg, "on_timer", JSON_FIELD_INT, &device->config.on_timer);
    add_json_endpoint(&json_cfg, "/timers", HTTP_GET, &json_fields_cfg); 
    add_json_endpoint(&json_cfg, "/timers", HTTP_POST, &json_fields_cfg); 

    add_json_field(&json_fields_cfg, "speed1", JSON_FIELD_INT, &device->config.speed1);
    add_json_field(&json_fields_cfg, "speed2", JSON_FIELD_INT, &device->config.speed2);
    add_json_endpoint(&json_cfg, "/speed", HTTP_GET, &json_fields_cfg); 
    add_json_endpoint(&json_cfg, "/speed", HTTP_POST, &json_fields_cfg); 

    add_json_field(&json_fields_cfg, "inter_timer", JSON_FIELD_INT, &device->config.inter_timer);
    add_json_endpoint(&json_cfg, "/interval", HTTP_GET, &json_fields_cfg); 
    add_json_endpoint(&json_cfg, "/interval", HTTP_POST, &json_fields_cfg);

    add_json_field(&json_fields_cfg, "motion_en", JSON_FIELD_BOOL, &device->config.motion_en);
    add_json_endpoint(&json_cfg, "/motion", HTTP_GET, &json_fields_cfg); 
    add_json_endpoint(&json_cfg, "/motion", HTTP_POST, &json_fields_cfg); 

    add_json_field(&json_fields_cfg, "humidity", JSON_FIELD_INT, &device->config.humidity);
    add_json_endpoint(&json_cfg, "/humidity", HTTP_GET, &json_fields_cfg);
    add_json_endpoint(&json_cfg, "/humidity", HTTP_POST, &json_fields_cfg); 
    
    for(int i=0; i<json_cfg.count; i++){
        register_json_handler(http_server, &json_cfg.data[i]);
    }
}

bool init_http_server(device_data_t *device)
{
    if(http_server != NULL) {
        httpd_stop(http_server);
        http_server = NULL;
    };
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = HANDLERS_NUM;
    config.uri_match_fn = httpd_uri_match_wildcard;
    if (httpd_start(&http_server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start server");
        return false;
    }
    
    if(http_server == NULL) {
        ESP_LOGE(TAG, "Failed to start server");
        return false;
    }
    
    device->status.admin_en = false;
    register_json_api_handlers(device);
    register_static_uri_handlers(device);

    // if(server != NULL) {
    //     httpd_stop(server);
    //     server = NULL;
    // }
    return true;
}


void stop_http_server()
{
    if(http_server != NULL){
        httpd_stop(http_server);
        http_server = NULL;
    }
}

esp_err_t ota_handler(httpd_req_t *req)
{
    // ota_update_stream_t *ota_stream = ota_update_stream_create(req);
    // if (ota_stream == NULL) {
    //     ESP_LOGE("OTA", "Failed to create OTA stream");
    //     httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to create OTA stream");
    //     return ESP_FAIL;
    // }
    // esp_err_t ret = ota_update_stream_process(ota_stream);
    httpd_resp_send_err(req, HTTPD_501_METHOD_NOT_IMPLEMENTED, "Unsupported method");
    return ESP_OK; 
}

esp_err_t get_favicon_handler(httpd_req_t *req)
{
    const emb_file_t file = EMBEDDED_FILE(favicon_ico);
    httpd_resp_set_type(req, "image/x-icon");
    return httpd_resp_send(req, file.start, file.size); 
}

esp_err_t get_index_handler(httpd_req_t *req)
{
    const emb_file_t file = EMBEDDED_FILE(index_html);
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, file.start, file.size); 
}

esp_err_t get_css_handler(httpd_req_t *req)
{
    const emb_file_t file = EMBEDDED_FILE(style_min_css);
    httpd_resp_set_type(req, "text/css");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    return httpd_resp_send(req, (const char *)file.start, file.size); 
}

esp_err_t get_script_handler(httpd_req_t *req)
{
    const emb_file_t file = EMBEDDED_FILE(script_min_js);
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    return httpd_resp_send(req, (const char *)file.start, file.size); 
}


esp_err_t filter_reset_handler(httpd_req_t *req)
{
    return httpd_resp_send(req, "Filter reset command received", HTTPD_RESP_USE_STRLEN);
}

esp_err_t admin_enable_handler(httpd_req_t *req)
{   
    device_data_t *device = (device_data_t *)req->user_ctx;
    device->status.admin_en = true;
    return get_index_handler(req);
}

esp_err_t exit_handler(httpd_req_t *req)
{
    httpd_resp_send(req, NULL, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    stop_http_server();
    return ESP_OK;
}