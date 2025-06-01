#include "unity.h"

#include "device.h"
#include "wifi_service.h"
#include "comm_types.h"

#include "esp_log.h"
#include "string.h"
#include "stdio.h"
 #include "esp_mac.h"
#include "device_http_server.h"
#include "http_json_helper.h"
#include "udp_server.h"

static device_data_t device;
static const char *TAG = "test server";


dev_ip_info_t ap_ip_info = {
   .netmask = {.bytes = {255,255,255,0}},
   .ip = {.bytes = {192, 168, 10, 1}},
   .gateway = {.bytes = {192, 168, 10, 1}},
   .enable = false,
};





TEST_CASE("start server", "[server]")
{
    esp_log_level_set("wifi", ESP_LOG_DEBUG);
    esp_log_level_set("httpd", ESP_LOG_DEBUG);
    esp_log_level_set("httpd_txrx", ESP_LOG_DEBUG);
    device.status.title = "DEBUG";
    device.status.info = "debug mode";
    dev_wifi_config_t *conf = &device.settings.wifi_conf;
    memset(conf, 0, sizeof(dev_wifi_config_t));
    device_init();
    strcat(conf->ap.ssid, "DEBUG");
    strcat(conf->ap.password, "atlant123");
    conf->ap.ip_info = ap_ip_info; 
    TEST_ASSERT_TRUE(wifi_preinit(conf));
    TEST_ASSERT_TRUE(wifi_set_mode(conf, DEV_WIFI_MODE_AP));
    device_wait_bits(BIT_WIFI_AP_START|BIT_WIFI_CONNECTED, portMAX_DELAY);
    TEST_ASSERT_TRUE(init_http_server(&device));
    TEST_ASSERT_TRUE(start_dns_server(ap_ip_info.ip));
   ESP_LOGI(TAG, "HTTP server started");
   vTaskDelay(0XFFFFFFF / portTICK_PERIOD_MS);
}



#define FIELDS_NUM 10
#define HANDLERS_NUM 6

static json_field_t fields[FIELDS_NUM];
static json_http_handler_t handlers[HANDLERS_NUM];

TEST_CASE("add_json_field: INT, BOOL, STR, FLOAT, STRING", "[json_static_config]") 
{
    json_field_cfg_t cfg = {
        .data = fields,
        .capacity = FIELDS_NUM,
        .count = 0,
        .start_data_indx = 0,
        .update_last = false
    };

    int i = 123;
    bool b = true;
    char str[8] = "abc";
    float f = 3.14f;
    char string[16] = "hello";

    TEST_ASSERT_TRUE(add_json_field(&cfg, "i", JSON_FIELD_INT, &i));
    TEST_ASSERT_TRUE(add_json_field(&cfg, "b", JSON_FIELD_BOOL, &b));
    TEST_ASSERT_TRUE(add_json_field(&cfg, "str", JSON_FIELD_STR, str));
    TEST_ASSERT_TRUE(add_json_field(&cfg, "string", JSON_FIELD_STR, string));
    TEST_ASSERT_EQUAL(5, cfg.count);

    TEST_ASSERT_EQUAL(JSON_FIELD_INT, cfg.data[0].type);
    TEST_ASSERT_EQUAL(JSON_FIELD_BOOL, cfg.data[1].type);
    TEST_ASSERT_EQUAL(JSON_FIELD_STR, cfg.data[2].type);
    TEST_ASSERT_EQUAL(JSON_FIELD_STR, cfg.data[4].type);
}

TEST_CASE("add_json_field: overflow", "[json_static_config]") 
{
    json_field_cfg_t cfg = {
        .data = fields,
        .capacity = 2,
        .count = 0,
        .start_data_indx = 0,
        .update_last = false
    };
    int v1 = 1, v2 = 2, v3 = 3;
    TEST_ASSERT_TRUE(add_json_field(&cfg, "a", JSON_FIELD_INT, &v1));
    TEST_ASSERT_TRUE(add_json_field(&cfg, "b", JSON_FIELD_INT, &v2));
    TEST_ASSERT_FALSE(add_json_field(&cfg, "c", JSON_FIELD_INT, &v3));
}

TEST_CASE("add_json_endpoint: GET and POST", "[json_static_config]") 
{
    json_field_cfg_t field_cfg = {
        .data = fields,
        .capacity = FIELDS_NUM,
        .count = 0,
        .start_data_indx = 0,
        .update_last = false
    };
    json_config_t cnf = {
        .data = handlers,
        .capacity = HANDLERS_NUM,
        .count = 0
    };

    int v = 1;
    add_json_field(&field_cfg, "a", JSON_FIELD_INT, &v);

    TEST_ASSERT_TRUE(add_json_endpoint(&cnf, "/test", HTTP_GET, &field_cfg));
    TEST_ASSERT_EQUAL(1, cnf.count);
    TEST_ASSERT_EQUAL(HTTP_GET, cnf.data[0].method);

    TEST_ASSERT_TRUE(add_json_endpoint(&cnf, "/test", HTTP_POST, &field_cfg));
    TEST_ASSERT_EQUAL(2, cnf.count);
    TEST_ASSERT_EQUAL(HTTP_POST, cnf.data[1].method);

    TEST_ASSERT_EQUAL_PTR(cnf.data[0].fields, cnf.data[1].fields);
    TEST_ASSERT_EQUAL(cnf.data[0].field_count, cnf.data[1].field_count);
}

TEST_CASE("add_json_endpoint: overflow", "[json_static_config]") 
{
    json_field_cfg_t field_cfg = {
        .data = fields,
        .capacity = FIELDS_NUM,
        .count = 0,
        .start_data_indx = 0,
        .update_last = false
    };
    json_config_t cnf = {
        .data = handlers,
        .capacity = 1,
        .count = 0
    };

    int v = 1;
    add_json_field(&field_cfg, "a", JSON_FIELD_INT, &v);
    TEST_ASSERT_TRUE(add_json_endpoint(&cnf, "/a", HTTP_GET, &field_cfg));
    add_json_field(&field_cfg, "b", JSON_FIELD_INT, &v);
    TEST_ASSERT_FALSE(add_json_endpoint(&cnf, "/b", HTTP_GET, &field_cfg));
}

TEST_CASE("add_json_endpoint: no fields", "[json_static_config]") 
{
    json_field_cfg_t field_cfg = {
        .data = fields,
        .capacity = FIELDS_NUM,
        .count = 0,
        .start_data_indx = 0,
        .update_last = false
    };
    json_config_t cnf = {
        .data = handlers,
        .capacity = HANDLERS_NUM,
        .count = 0
    };

    TEST_ASSERT_FALSE(add_json_endpoint(&cnf, "/empty", HTTP_GET, &field_cfg));
}

TEST_CASE("add_json_field: update_last logic", "[json_static_config]") 
{
    json_field_cfg_t field_cfg = {
        .data = fields,
        .capacity = FIELDS_NUM,
        .count = 0,
        .start_data_indx = 0,
        .update_last = false
    };
    json_config_t cnf = {
        .data = handlers,
        .capacity = HANDLERS_NUM,
        .count = 0
    };

    int v1 = 1, v2 = 2;
    add_json_field(&field_cfg, "a", JSON_FIELD_INT, &v1);
    add_json_endpoint(&cnf, "/a", HTTP_GET, &field_cfg);
    TEST_ASSERT_TRUE(field_cfg.update_last);
    if(field_cfg.update_last){
        field_cfg.start_data_indx = field_cfg.count;
        field_cfg.update_last = false;
    }
    add_json_field(&field_cfg, "b", JSON_FIELD_INT, &v2);
    TEST_ASSERT_EQUAL_STRING("b", field_cfg.data[field_cfg.start_data_indx].name);
    TEST_ASSERT_EQUAL(1, field_cfg.count - field_cfg.start_data_indx);
}