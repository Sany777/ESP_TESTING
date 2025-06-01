#include "wifi_service.h"

#include "esp_wifi.h"
#include "device.h"


#define MAX_SCAN_RESULTS 30
#define MAX_WIFI_CH 13


static const char *TAG = "wifi_scan";
static wifi_ap_record_t ap_info[MAX_SCAN_RESULTS];


int get_best_wifi_ch(void)
{
    wifi_mode_t prev_mode;

    if (esp_wifi_get_mode(&prev_mode) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get WiFi mode");
        return 1;
    }

    if (prev_mode != WIFI_MODE_NULL) {
        set_wifi_off();
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    vTaskDelay(pdMS_TO_TICKS(10)); 

    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time = { .active = {.min = 100, .max = 300}, },
    };
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

    uint16_t ap_count = MAX_SCAN_RESULTS;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_info));

    int channel_counts[14] = {0};
    for (int i = 0; i < ap_count; i++) {
        uint8_t ch = ap_info[i].primary;
        if (ch >= 1 && ch <= MAX_WIFI_CH) {
            channel_counts[ch]++;
        }
    }

    int best_channel = 1;
    for (int ch = 2; ch <= MAX_WIFI_CH; ch++) {
        if (channel_counts[ch] < channel_counts[best_channel]) {
            best_channel = ch;
        }
    }

#ifdef DEBUG
    for (int ch = 1; ch <= MAX_WIFI_CH; ch++) {
        ESP_LOGI(TAG, "Channel %2d: %d APs", ch, channel_counts[ch]);
    }
#endif

    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));

    if (prev_mode != WIFI_MODE_NULL) {
        vTaskDelay(pdMS_TO_TICKS(50));
        ESP_ERROR_CHECK(esp_wifi_set_mode(prev_mode));
        set_wifi_on();
    }

    return best_channel;
}
