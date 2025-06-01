#ifndef EMB_FILE_H
#define EMB_FILE_H

#include "stdbool.h"
#include <stddef.h>
#include "stdint.h"



// ------------- SUPPORT EMBEDDED FILES -----------------
typedef struct {
    const char *start;
    size_t size;
} emb_file_t;

#define EMBEDDED_FILE(name) \
{ (const char *) _binary_##name##_start, \
    (size_t)(_binary_##name##_end - _binary_##name##_start) }

#define DEF_EMB_FILE(name) \
    extern const uint8_t  _binary_##name##_start[];  \
    extern const uint8_t  _binary_##name##_end[];


#define IPV4_SIZE  4   
#define MAX_SSID_LEN 32
#define MAX_PWD_LEN 64
#define MAX_HOST_NAME_LEN 32

// wraper, little endian format 
typedef union {
    uint8_t bytes[IPV4_SIZE];
    uint32_t u32;
} ipv4_t;



typedef struct {
    const char *hostname;
    ipv4_t ip;
    uint16_t port;
} serv_conf_t;

typedef struct {
    bool enable;
    ipv4_t netmask;
    ipv4_t gateway;
    ipv4_t ip;
    ipv4_t dns;
} dev_ip_info_t;

typedef struct {
    char ssid[MAX_SSID_LEN+1];
    char password[MAX_PWD_LEN+1];  
    dev_ip_info_t ip_info;
    uint8_t ch;
    float power_dbm;
} dev_ap_conf_t;

typedef struct {
    char ssid[MAX_SSID_LEN+1];
    char password[MAX_PWD_LEN+1]; 
    dev_ip_info_t ip_info;
    ipv4_t broadcast;
} dev_sta_conf_t;

typedef struct {
    dev_ap_conf_t ap;
    dev_sta_conf_t sta;
    int cur_mode;
} dev_wifi_config_t;

typedef struct {
    uint16_t day;  
    uint8_t  hour;
    uint8_t  min; 
} __attribute__((packed)) dev_time_info_t;

typedef struct {
    uint8_t speed1;
    uint8_t speed2;
    uint8_t off_timer;
    uint8_t on_timer;
    uint8_t inter_timer;
    bool motion_en;
    uint8_t humidity;
    bool wifi_en;
} device_conf_t;

typedef struct {
    uint64_t key1;
    device_conf_t main_settings;
} nfc_setings_data_t;

typedef struct {
    uint64_t key2;
    dev_sta_conf_t sta;
} nfc_wifi_data_t;

typedef struct {
    char pwd[MAX_PWD_LEN+1];
    uint64_t id;
    uint8_t type;
} dev_info_t;

typedef struct {
    serv_conf_t vents_client_conf;
    serv_conf_t dns_serv_conf;
    serv_conf_t bgcp_serv_conf;
    dev_wifi_config_t wifi_conf;
    dev_info_t dev_info;
    dev_time_info_t filter_time;
    dev_time_info_t filt_time_set;
} settings_t;


typedef struct {
    int humidity;
    bool motion;
    bool lt1;
    bool lt2;
    int fan_speed;
    const char *info;
    const char *title;
    bool admin_en;
} device_status_t;


typedef struct {
    device_status_t status;
    device_conf_t config;
    settings_t settings;
    uint8_t filter_status;
    uint8_t speed;
    uint16_t rpm;
    bool boost_switch;
    volatile bool emergency;
    uint16_t rh_val;
    uint8_t rh_perc;
    uint8_t voc_val;
    uint8_t voc_perc;
} device_data_t;







#endif // EMB_FILE_H