#pragma once


#include "device.h"
#include <stdint.h>
#include <stdbool.h>


#define AP_MAX_CONNECTION 	2
#define MIN_WIFI_PWD_LEN    8
#define MIN_WIFI_SSID_LEN   3
#define WIFI_MAX_RETRY      10

// #define DEBUG 


typedef enum {
    DEV_WIFI_MODE_NOT_INIT = 0,
    DEV_WIFI_MODE_NULL,  
    DEV_WIFI_MODE_STA,     
    DEV_WIFI_MODE_AP,      
    DEV_WIFI_MODE_APSTA,  
} dev_wifi_mode_t;


bool wifi_set_mode(dev_wifi_config_t *conf, const dev_wifi_mode_t mode);

bool wifi_preinit(dev_wifi_config_t *conf);
int get_best_wifi_ch(void);


int get_wifi_mode(const dev_wifi_mode_t mode);

bool set_wifi_on();
bool set_wifi_off();


bool set_wifi_tx_power_exact_dbm(float dbm);