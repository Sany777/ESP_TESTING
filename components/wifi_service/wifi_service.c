#include "wifi_service.h"

#include "device.h"
#include "event_manager.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include <string.h>
#include "esp_check.h"
#include "lwip/inet.h"


#define WIFI_EVN_STOP_VAL 0xffff

static const char *TAG = "wifi";
static esp_netif_t *sta_netif;
static esp_netif_t *ap_netif;

static bool set_ap_ip_info(esp_netif_t *netif, const dev_ip_info_t *info);
static bool set_sta_ip_info(esp_netif_t *netif, const dev_ip_info_t *info);


static bool _wifi_init;

static 
void wifi_event_handler(void* user_ctx, 
    esp_event_base_t event_base,
    int32_t event_id, 
    void* event_data)
{
    static int reconnection_ctrl;
    
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
            if(device_get_bits() & BIT_WIFI_AUTO_CONN){
                reconnection_ctrl = 0;
                esp_wifi_connect();
            }
        break;

        case WIFI_EVENT_STA_DISCONNECTED:
        {
            if(device_get_bits() & BIT_WIFI_AUTO_CONN) {
                if(reconnection_ctrl < WIFI_MAX_RETRY) {
                    esp_wifi_connect();
                    reconnection_ctrl++;
                } else {
                    device_clear_bits(BIT_WIFI_STA_CONNECTED);
                    device_set_bits(BIT_WIFI_STA_CONN_FAIL);
                    // set_event(EVN_STA_CONN_FAIL, user_ctx);
                }
            }
        }
        break;

        case WIFI_EVENT_AP_START:
            device_set_bits(BIT_WIFI_AP_START);
            // set_event(EVN_AP_START, user_ctx);
        break;

        case WIFI_EVENT_AP_STACONNECTED: 
        {
            wifi_event_ap_staconnected_t* conn = (wifi_event_ap_staconnected_t*) event_data;
            device_set_bits(BIT_WIFI_CONNECTED);
            // set_event(EVN_AP_CLIENT, user_ctx);
            break;
        }

        case WIFI_EVENT_STA_STOP:
            device_clear_bits(BIT_WIFI_STA_CONNECTED|BIT_WIFI_AUTO_CONN);
            // set_event(EVN_STA_STOP, user_ctx);
        break;

        case WIFI_EVENT_AP_STADISCONNECTED: 
        {
            wifi_event_ap_stadisconnected_t* disc = (wifi_event_ap_stadisconnected_t*) event_data;
            device_clear_bits(BIT_WIFI_CONNECTED);
        }
        break;

        default: break;
        }
    } else if (event_base == IP_EVENT) {
        if (event_id == IP_EVENT_STA_GOT_IP) {
            device_set_bits(BIT_WIFI_STA_CONNECTED);
            reconnection_ctrl = 0;
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            void *ctx = NULL;
            if(user_ctx != NULL){
                dev_ip_info_t *sta_ip_info = (dev_ip_info_t *)user_ctx;
                sta_ip_info->ip.u32 = event->ip_info.ip.addr;
                sta_ip_info->gateway.u32 = event->ip_info.gw.addr;
                sta_ip_info->netmask.u32 = event->ip_info.netmask.addr;
                ctx = sta_ip_info;
            }
            set_event(EVN_STA_CONNECTED, ctx);
        }
    }
}
         


bool wifi_preinit(dev_wifi_config_t *conf)
{
    if(_wifi_init) {
        return true;
    }
    CHECK_RET_FALSE(esp_netif_init());
    CHECK_RET_FALSE(esp_event_loop_create_default());
    sta_netif = esp_netif_create_default_wifi_sta();
    ap_netif = esp_netif_create_default_wifi_ap();
    assert(ap_netif != NULL);
    assert(sta_netif != NULL);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    CHECK_RET_FALSE(esp_wifi_init(&cfg));
    esp_wifi_set_ps(WIFI_PS_NONE);
    CHECK_RET_FALSE(esp_wifi_set_mode(WIFI_MODE_NULL));
    CHECK_RET_FALSE(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, &conf->sta.ip_info));
    CHECK_RET_FALSE(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, &conf->sta.ip_info));
    CHECK_RET_FALSE(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    conf->cur_mode = DEV_WIFI_MODE_NULL;
    _wifi_init = true;
    return true;
}

bool wifi_set_mode(dev_wifi_config_t *conf, const dev_wifi_mode_t new_mode)
{
    wifi_mode_t cur_wifi_mode;

    wifi_config_t sta_cfg = {0};
    wifi_config_t ap_cfg = {0};

    if(!_wifi_init){
        ESP_LOGE(TAG, "Wifi not init");
        return  false;
    }

    if (new_mode == DEV_WIFI_MODE_STA || new_mode == DEV_WIFI_MODE_APSTA) {
        if(cpy_str(sta_cfg.sta.ssid, conf->sta.ssid, sizeof(sta_cfg.sta.ssid)) < 1) {
            return false;
        }
        int sta_password_len = cpy_str(sta_cfg.sta.password, conf->sta.password, sizeof(sta_cfg.sta.password));
        if(sta_password_len == -1 || (sta_password_len > 0 && sta_password_len < 8)) {
            ESP_LOGE(TAG, "Invalid sta password");
            return false;
        }
        sta_cfg.sta.threshold.authmode = sta_password_len != 0 ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN;
    }

    if (new_mode == DEV_WIFI_MODE_AP || new_mode == DEV_WIFI_MODE_APSTA) {
        if(cpy_str(ap_cfg.ap.ssid, conf->ap.ssid, sizeof(ap_cfg.ap.ssid)) < 1) {
            ESP_LOGE(TAG, "Invalid ap ssid");
            return false;
        }
        int ap_password_len = cpy_str(ap_cfg.ap.password, conf->ap.password, sizeof(ap_cfg.ap.password));
        if(ap_password_len == -1 || (ap_password_len > 0 && ap_password_len < 8)) {
            ESP_LOGE(TAG, "Invalid ap password");
            return false;
        }
        ap_cfg.ap.authmode = ap_password_len != 0 ? WIFI_AUTH_WPA_WPA2_PSK : WIFI_AUTH_OPEN;
        ap_cfg.ap.max_connection = AP_MAX_CONNECTION;
        if(conf->ap.ch == 0 || conf->ap.ch > 13) {
            ap_cfg.ap.channel = 1;
        } else {
            ap_cfg.ap.channel = conf->ap.ch;
        }
    }

    esp_wifi_get_mode(&cur_wifi_mode);

    if (cur_wifi_mode != WIFI_MODE_NULL) {
        set_wifi_off();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    
    wifi_mode_t wifi_mode = get_wifi_mode(new_mode);

    CHECK_RET_FALSE(esp_wifi_set_mode(wifi_mode));
    
    if (new_mode == DEV_WIFI_MODE_STA || new_mode == DEV_WIFI_MODE_APSTA) {        
        set_sta_ip_info(sta_netif, &conf->sta.ip_info);
        CHECK_RET_FALSE(esp_wifi_set_config(WIFI_IF_STA, &sta_cfg));
    }
    
    if (new_mode == DEV_WIFI_MODE_AP || new_mode == DEV_WIFI_MODE_APSTA) {
        set_ap_ip_info(ap_netif, &conf->ap.ip_info);
        CHECK_RET_FALSE(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));
    }
    
    if(new_mode != DEV_WIFI_MODE_NULL) {
        set_wifi_on();
        if (new_mode == DEV_WIFI_MODE_AP || new_mode == DEV_WIFI_MODE_APSTA) {
            set_wifi_tx_power_exact_dbm(conf->ap.power_dbm);
        }
    }
    conf->cur_mode = new_mode;
    return true;
}


static
bool set_ap_ip_info(esp_netif_t *netif, const dev_ip_info_t *info)
{
    if (!info->enable) {
        return true;
    }
    esp_netif_ip_info_t ip_info = {
        .ip.addr = info->ip.u32,
        .netmask.addr = info->netmask.u32,
        .gw.addr = info->gateway.u32
    };
    CHECK_RET_FALSE(esp_netif_dhcps_stop(netif));
    CHECK_RET_FALSE(esp_netif_set_ip_info(netif, &ip_info));
    CHECK_RET_FALSE(esp_netif_dhcps_start(netif));
    return true;
}


static
bool set_sta_ip_info(esp_netif_t *netif, const dev_ip_info_t *info)
{
    if (!info->enable) {
        return esp_netif_dhcpc_start(netif) == ESP_OK;
    }

    esp_netif_ip_info_t ip_info = {
        .ip.addr = info->ip.u32,
        .netmask.addr = info->netmask.u32,
        .gw.addr = info->gateway.u32
    };

    CHECK_RET_FALSE(esp_netif_dhcpc_stop(netif));
    CHECK_RET_FALSE(esp_netif_set_ip_info(netif, &ip_info));
    esp_netif_dns_info_t dns_info = {
        .ip.type = ESP_IPADDR_TYPE_V4,
        .ip.u_addr.ip4.addr = info->dns.u32
    };
    CHECK_RET_FALSE(esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info));
    return true;
}


int get_wifi_mode(const dev_wifi_mode_t mode)
{
    switch (mode) {
        case DEV_WIFI_MODE_STA: return WIFI_MODE_STA;
        case DEV_WIFI_MODE_AP: return WIFI_MODE_AP;
        case DEV_WIFI_MODE_APSTA: return WIFI_MODE_APSTA;
        default:break;
    }
    return WIFI_MODE_NULL;
}


bool set_wifi_on()
{
    device_set_bits(BIT_WIFI_AUTO_CONN);
    return esp_wifi_start() == ESP_OK;
}

bool set_wifi_off()
{
    device_clear_bits(BIT_WIFI_STA_CONNECTED|BIT_WIFI_AUTO_CONN);
    return esp_wifi_stop() == ESP_OK;
}


// Param power unit is 0.25dBm, range is [8, 84] corresponding to 2dBm - 20dBm.
static 
int8_t dbm_to_raw_tx_power(float dbm)
{
    int8_t raw = (int8_t)(dbm * 4.0f + 0.5f); 
    if (raw < 8)  raw = 8;
    if (raw > 84) raw = 84;
    return raw;
}


bool set_wifi_tx_power_exact_dbm(float dbm)
{
    if (dbm < 2.0f || dbm > 21.0f) {
        ESP_LOGE(TAG, "Invalid tx power value");
        return false;
    }
    int8_t raw = dbm_to_raw_tx_power(dbm);
    return esp_wifi_set_max_tx_power(raw) == ESP_OK;
}