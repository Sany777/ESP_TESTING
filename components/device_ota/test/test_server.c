#include "unity.h"

#include "device.h"
#include "device_memory.h"
#include "event_manager.h"
#include "wifi_service.h"
#include "comm_types.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "string.h"
#include "stdio.h"

#include "device_http_server.h"
#include "udp_server.h"

extern device_data_t device;

static const char *TAG = "TEST OTA";



   
dev_ip_info_t ap_ip_info = {
   .netmask = {.bytes = {255,255,255,0}},
   .ip = {.bytes = {192, 168, 10, 1}},
   .gateway = {.bytes = {192, 168, 10, 1}},
   .enable = true,
};

static const uri_data_t uri_list[] = {
   {"/Status", HTTP_POST, handler_set_flag},
   {"/Area", HTTP_POST, handler_set_area},
   {"/Network", HTTP_POST, handler_set_net_data},
   {"/data?", HTTP_POST, handler_give_data},
   {"/close", HTTP_POST, handler_close},
   {"/Time", HTTP_POST, handler_set_time},
   {"/style.css", HTTP_GET, get_css_handler},
   {"/script.js", HTTP_GET, get_script_handler},
   {"/OTA", HTTP_POST, ota_handler},
   {"/*", HTTP_GET, get_index_handler}
};
   
   
   
TEST_CASE("Start server", "[ota]")
{
   dev_wifi_config_t *conf = &device.settings.wifi_conf;
   device_init();
   memset(&device, 0, sizeof(device));
   strcat(conf->ap.ssid, "DEBUG");
   strcat(conf->ap.password, "atlant123");
   conf->ap.ip_info = ap_ip_info; 
   TEST_ASSERT_TRUE(wifi_preinit(conf));
   TEST_ASSERT_TRUE(wifi_set_mode(conf, DEV_WIFI_MODE_AP));
   TEST_ASSERT_TRUE(start_http_server(uri_list, ARR_LEN(uri_list), &device));
   TEST_ASSERT_TRUE(start_dns_server(ap_ip_info.ip));
}


