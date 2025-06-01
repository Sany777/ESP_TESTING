#include "global_helpers.h"

#include "device_def.h"
#include "stdio.h"

#include "esp_mac.h"

static const char * TAG = "device";

uint32_t compute_broadcast(uint32_t ip, uint32_t netmask) 
{
    return (ip & netmask) | (~netmask);
}

void get_serial_number(char serial_str[SERIAL_STR_LEN])
{
    uint8_t mac[6];
    esp_efuse_mac_get_default(mac);
    snprintf(serial_str, SERIAL_STR_LEN, "0100%02X%02X%02X%02X%02X%02X",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}


int cpy_str(void *dst, const void *src, int dst_buf_size)
{
    int len = strnlen((char*)src, dst_buf_size);
    if(len == dst_buf_size) return -1;
    memcpy(dst, src, len);
    return len;
}
