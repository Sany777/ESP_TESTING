#include "unity.h"

#include "wifi_service.h"
#include "device.h"
#include "device_memory.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "string.h"
#include "stdio.h"
#include "esp_wifi.h"
#include "event_manager.h"

static const char *TAG = "test wifi";


static dev_wifi_config_t conf;

#define MAX_WIFI_CH 13


void signal_connection(unsigned int now_ms, void *ctx)
{
   dev_ip_info_t *sta_ip_info = (dev_ip_info_t *)ctx;
   ESP_LOGD(TAG, "GET IP:"IPSTR, sta_ip_info->ip.bytes[0],sta_ip_info->ip.bytes[1],sta_ip_info->ip.bytes[2],sta_ip_info->ip.bytes[3]);
}

void signal_fail_connection(unsigned int now_ms, void *ctx)
{
   ESP_LOGD(TAG, "Connection timeout err");
}

static 
void test_set_mode(dev_wifi_mode_t initial_mode)
{
    wifi_set_mode(&conf, initial_mode);
    vTaskDelay(pdMS_TO_TICKS(1000));
    wifi_mode_t wifi_mode;
    esp_wifi_get_mode(&wifi_mode);
    TEST_ASSERT_EQUAL(get_wifi_mode(initial_mode), wifi_mode);
}

TEST_CASE("init wifi", "[wifi]")
{
   esp_log_level_set("wifi", ESP_LOG_DEBUG);
   device_init();
   strcat(conf.ap.ssid, "DEBUG");
   strcat(conf.ap.password, "atlant123");
   strcat(conf.sta.ssid, "edge184");
   strcat(conf.sta.password, "NASa0923");
   conf.ap.power_dbm = 10.5;
   TEST_ASSERT_TRUE(wifi_preinit(&conf));
   register_event_handler(EVN_STA_CONNECTED, signal_connection);
   register_event_handler(EVN_STA_CONN_FAIL, signal_fail_connection);
}

static float get_current_wifi_dbm(void)
{
    int8_t qdbm = 0;
    if (esp_wifi_get_max_tx_power(&qdbm) != ESP_OK) return -1;
    return qdbm / 4.0f;
}

#define FLOAT_EQ(x, y) (fabs((x) - (y)) < 0.26f) // 0.25 precision

TEST_CASE("Valid power values", "[wifi][tx_power]")
{
    wifi_set_mode(&conf, DEV_WIFI_MODE_AP);

    float dbm = 2.0f;
    TEST_ASSERT_TRUE(set_wifi_tx_power_exact_dbm(dbm));
    TEST_ASSERT_TRUE(FLOAT_EQ(get_current_wifi_dbm(), dbm));

    dbm = 20.0f;
    TEST_ASSERT_TRUE(set_wifi_tx_power_exact_dbm(dbm));
    TEST_ASSERT_TRUE(FLOAT_EQ(get_current_wifi_dbm(), dbm));

    dbm = 10.0f;
    TEST_ASSERT_TRUE(set_wifi_tx_power_exact_dbm(dbm));
    TEST_ASSERT_TRUE(FLOAT_EQ(get_current_wifi_dbm(), dbm));

    dbm = 17.75f;
    TEST_ASSERT_TRUE(set_wifi_tx_power_exact_dbm(dbm));
    TEST_ASSERT_TRUE(FLOAT_EQ(get_current_wifi_dbm(), 18.0f));
}

TEST_CASE("Not valid values", "[wifi][tx_power]")
{
    TEST_ASSERT_FALSE(set_wifi_tx_power_exact_dbm(1.9f));
    TEST_ASSERT_FALSE(set_wifi_tx_power_exact_dbm(21.1f));
    TEST_ASSERT_FALSE(set_wifi_tx_power_exact_dbm(-5.0f));
    TEST_ASSERT_FALSE(set_wifi_tx_power_exact_dbm(100.0f));
}


TEST_CASE("wifi mode: NULL -> APSTA", "[wifi]") 
{
   get_best_wifi_ch();
   vTaskDelay(pdMS_TO_TICKS(1000));
   test_set_mode(DEV_WIFI_MODE_APSTA);
}

TEST_CASE("wifi mode: AP -> AP", "[wifi]") 
{
   wifi_set_mode(&conf, DEV_WIFI_MODE_AP);
   vTaskDelay(pdMS_TO_TICKS(1000));
   wifi_set_mode(&conf, DEV_WIFI_MODE_AP); 
   vTaskDelay(pdMS_TO_TICKS(2000));
}

TEST_CASE("wifi mode: APSTA -> NULL -> NULL", "[wifi]") 
{
   test_set_mode(DEV_WIFI_MODE_APSTA);
   test_set_mode(DEV_WIFI_MODE_NULL);
   test_set_mode(DEV_WIFI_MODE_NULL); 
}

TEST_CASE("wifi mode: NULL -> AP", "[wifi]") 
{
   test_set_mode(DEV_WIFI_MODE_NULL);
   test_set_mode(DEV_WIFI_MODE_AP);
}

TEST_CASE("wifi mode: AP -> NULL -> APSTA", "[wifi]") 
{
   test_set_mode(DEV_WIFI_MODE_AP);
   test_set_mode(DEV_WIFI_MODE_NULL);
   test_set_mode(DEV_WIFI_MODE_APSTA);
}

TEST_CASE("wifi mode: APSTA -> STA", "[wifi]") 
{
   test_set_mode(DEV_WIFI_MODE_APSTA);
   test_set_mode(DEV_WIFI_MODE_STA);
}

TEST_CASE("wifi mode: STA -> AP", "[wifi]") 
{
   test_set_mode(DEV_WIFI_MODE_STA);
   test_set_mode(DEV_WIFI_MODE_AP);
}


TEST_CASE("wifi mode: AP -> APSTA -> NULL", "[wifi]") 
{
   test_set_mode(DEV_WIFI_MODE_AP);
   test_set_mode(DEV_WIFI_MODE_APSTA);
   test_set_mode(DEV_WIFI_MODE_NULL);
}

TEST_CASE("wifi mode: NULL -> STA", "[wifi]") 
{
   test_set_mode(DEV_WIFI_MODE_NULL);
   test_set_mode(DEV_WIFI_MODE_STA);
   test_set_mode(DEV_WIFI_MODE_NULL);
}

static 
void test_best_channel_and_mode(dev_wifi_mode_t initial_mode)
{
    wifi_set_mode(&conf, initial_mode);
    vTaskDelay(pdMS_TO_TICKS(1000));
    int best_ch = get_best_wifi_ch();
    TEST_ASSERT_TRUE(best_ch >= 1 && best_ch <= MAX_WIFI_CH);
    wifi_mode_t wifi_mode;
    esp_wifi_get_mode(&wifi_mode);
    ESP_LOGI(TAG, "Initial mode: %d, Best channel: %d", initial_mode, best_ch);
    TEST_ASSERT_EQUAL(get_wifi_mode(initial_mode), wifi_mode);
}

TEST_CASE("get_best_wifi_ch() in NULL mode", "[wifi][scan]")
{
   test_best_channel_and_mode(DEV_WIFI_MODE_NULL);
}

TEST_CASE("get_best_wifi_ch() in STA mode", "[wifi][scan]")
{
    test_best_channel_and_mode(DEV_WIFI_MODE_STA);
}

TEST_CASE("get_best_wifi_ch() in AP mode", "[wifi][scan]")
{
    test_best_channel_and_mode(DEV_WIFI_MODE_AP);
}

TEST_CASE("get_best_wifi_ch() in APSTA mode", "[wifi][scan]")
{
    test_best_channel_and_mode(DEV_WIFI_MODE_APSTA);
}



TEST_CASE("Connection to STA", "[wifi][scan]")
{
   ESP_LOGI(TAG, "Check connection to STA...");
   wifi_set_mode(&conf, DEV_WIFI_MODE_STA);
   vTaskDelay(pdMS_TO_TICKS(15000));
}