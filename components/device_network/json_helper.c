#include "http_json_helper.h"
#include <cJSON.h>
#include "esp_log.h"
#include "esp_err.h"
#include <string.h>
#include <stdlib.h>
#include "esp_http_server.h"

#include <ctype.h>
#include "string.h"
#include "stdio.h"


static const char *TAG = "JSON_HELPER";

static esp_err_t json_post_handler(httpd_req_t *req);
static esp_err_t json_get_handler(httpd_req_t *req);


esp_err_t register_json_handler(httpd_handle_t server, json_http_handler_t *handler)
{
    if (!server || !handler) {
        ESP_LOGE(TAG, "Invalid parameters: server or handler is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    if(!handler->uri 
            || handler->field_count <= 0 
            || !handler->fields 
            || (handler->method != HTTP_POST && handler->method != HTTP_GET)) {
        ESP_LOGE(TAG, "Invalid handler configuration");
        return ESP_ERR_INVALID_ARG;
    }
    httpd_uri_t uri_handler = {
        .uri = handler->uri,
        .method = handler->method,
        .handler = handler->method == HTTP_POST 
                            ? json_post_handler 
                            : json_get_handler,
        .user_ctx = handler
    };
    return httpd_register_uri_handler(server, &uri_handler);
}

static 
esp_err_t add_field_to_json(cJSON *json_obj, const json_field_t *field) 
{
 
    switch (field->type) {
        case JSON_FIELD_INT:
            return cJSON_AddNumberToObject(json_obj, field->name, *(int *)field->ptr) ? ESP_OK : ESP_FAIL;
        case JSON_FIELD_BOOL:
            return cJSON_AddBoolToObject(json_obj, field->name, *(bool *)field->ptr) ? ESP_OK : ESP_FAIL;
        case JSON_FIELD_STR:
            return cJSON_AddStringToObject(json_obj, field->name, (const char *)field->ptr) ? ESP_OK : ESP_FAIL;
        default:
            return ESP_FAIL;
    }
}
#define JSON_MAX_SIZE 512


static 
esp_err_t json_post_handler(httpd_req_t *req)
{
    const json_http_handler_t *post_handler = (const json_http_handler_t *)req->user_ctx;
    if (!post_handler) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, NULL);
        return ESP_FAIL;
    }
    if (req->content_len == 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, NULL);
        return ESP_FAIL;
    }
    
    char net_buf[JSON_MAX_SIZE];
    if (req->content_len > sizeof(net_buf) - 1) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, NULL);
        return ESP_FAIL;
    }

    int len = httpd_req_recv(req, net_buf, req->content_len);
    if (len <= 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Receive failed");
        return ESP_FAIL;
    }
    net_buf[len] = '\0';
    cJSON *json = cJSON_Parse(net_buf);
    if (!json) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "JSON parse error");
        return ESP_FAIL;
    }

    for (size_t i = 0; i < post_handler->field_count; i++) {
        const json_field_t *field = &post_handler->fields[i];
        if(field->name == NULL) {
            ESP_LOGE(TAG, "Field name is NULL");
            continue;
        }
        if(field->ptr == NULL) {
            ESP_LOGE(TAG, "Field  %s value is NULL", field->name);
            continue;
        }
        cJSON *item = cJSON_GetObjectItemCaseSensitive(json, field->name);
        if (!item) {
            ESP_LOGW(TAG, "Missing field: %s", field->name);
            continue;
        }

        switch (field->type) {
            case JSON_FIELD_INT:
                if (cJSON_IsNumber(item)) {
                    *(int *)field->ptr = item->valueint;
                } else {
                    ESP_LOGW(TAG, "Field '%s' is not integer", field->name);
                }
                break;

            case JSON_FIELD_BOOL:
                if (cJSON_IsBool(item)) {
                    *(bool *)field->ptr = cJSON_IsTrue(item);
                } else {
                    ESP_LOGW(TAG, "Field '%s' is not boolean", field->name);
                }
                break;

            case JSON_FIELD_STR:
                if (cJSON_IsString(item) && item->valuestring) {
                    char *str_ptr = (char *)field->ptr;
                    strncpy(str_ptr, item->valuestring, field->max_len - 1);
                    str_ptr[field->max_len - 1] = '\0';
                } else {
                    ESP_LOGW(TAG, "Field '%s' is not string", field->name);
                }
                break;

            default:
                ESP_LOGW(TAG, "Unsupported field type: %d", field->type);
                break;
        }
    }

    cJSON_Delete(json);

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_status(req, "200 OK");
    return httpd_resp_sendstr(req, "OK");
}


static
esp_err_t json_get_handler(httpd_req_t *req)
{
    if (!req || !req->user_ctx) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No context");
        return ESP_FAIL;
    }

    const json_http_handler_t *get_handler = (const json_http_handler_t *)req->user_ctx;

    cJSON *json_obj = cJSON_CreateObject();
    if (!json_obj) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, NULL);
        return ESP_FAIL;
    }

    for (size_t i = 0; i < get_handler->field_count; i++) {
        const json_field_t *field = &get_handler->fields[i];
        if (!field->name || !field->ptr) {
            ESP_LOGE(TAG, "Invalid parameters %d: field name or pointer is NULL", i);
            continue;
        }
        if (add_field_to_json(json_obj, field) != ESP_OK) {
            cJSON_Delete(json_obj);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, NULL);
            return ESP_FAIL;
        }
    }

    char *json_str = cJSON_PrintUnformatted(json_obj);
    cJSON_Delete(json_obj);

    if (!json_str) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, NULL);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    esp_err_t err = httpd_resp_sendstr(req, json_str);
    free(json_str);
    return err;
}