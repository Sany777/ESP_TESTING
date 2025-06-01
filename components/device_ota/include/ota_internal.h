#ifndef OTA_INTERNAL_H
#define OTA_INTERNAL_H


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"


#define BIN_SIZE 0x120000 // 1.17 MB
#define ESP_ENC_HEADER_MAGIC "VEN\0"

#define CHECK_TRUE_AND_GOTO(_res, mes_loge, _label)   \
do{ \
    if(_res == false){  \
        ESP_LOGE(TAGE, "%s", mes_loge); \
        goto _label;    \
    }   \
}while(0)

typedef struct __attribute__((packed)) {
    uint8_t magic[4];
    uint8_t enc_gcm_key[384];
    uint8_t iv[16];
    uint8_t bin_size[4];
    uint8_t auth_tag[16];
    uint8_t reserved_rsa[88];
} esp_enc_img_rsa_header_t;

static 
inline uint32_t read_u32_le(const uint8_t b[4]) 
{
    return ((uint32_t)b[0]) | ((uint32_t)b[1] << 8) |
           ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}



#endif // OTA_INTERNAL_H