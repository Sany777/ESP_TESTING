#include "unity.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "string.h"
#include "stdio.h"

#include "device.h"
#include "esp_mac.h"
#include <arpa/inet.h>
#include <string.h>

static const char *TAG = "device";

static char Str_Dev_Serial_Number[20];


static 
void IntToUnicode (uint32_t value , char *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
      pbuf[idx] = (value >> 28) + '0';
    else
      pbuf[idx] = (value >> 28) + 'A' - 10;
    value = value << 4;
  }
}

static
void Get_SerialNum()
{
  uint8_t MAC[6];
  uint32_t Dev_Serial0, Dev_Serial1;
  
  esp_efuse_mac_get_default(MAC);
  Dev_Serial1 = (MAC[2]<<24) + (MAC[3]<<16) + (MAC[4]<<8) + MAC[5];
  Dev_Serial0 = (0x01<<24) + (0x00<<16) + (MAC[0]<<8) + (MAC[1]); 
  IntToUnicode (Dev_Serial0, Str_Dev_Serial_Number, 8);
  IntToUnicode (Dev_Serial1, &Str_Dev_Serial_Number[8], 8);
  Str_Dev_Serial_Number[16] = 0;

  ESP_LOGI(TAG, "Str_Dev_Serial_Number: %s.", (char*)Str_Dev_Serial_Number);
  ESP_LOGI(TAG, "MAC: %x:%x:%x:%x:%x:%x.", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
}

TEST_CASE("Set IP address from ipv4_t and string", "[net][device]")
{
  ipv4_t test = { .bytes = {192,168,0,1}};
  const char *ip_str = "192.168.0.1";
  struct in_addr exp;
  inet_aton(ip_str, &exp);
  uint8_t *exp_bytes = (uint8_t*) &exp.s_addr;
  uint32_t exp_u32 = *(unsigned*) &exp.s_addr;

  ESP_LOGI("TEST", "IP exp_bytes: %u.%u.%u.%u",
            (unsigned)exp_bytes[0], (unsigned)exp_bytes[1], (unsigned)exp_bytes[2], (unsigned)exp_bytes[3]);

  ESP_LOGI("TEST", "IP test.bytes: %u.%u.%u.%u",
            (unsigned)test.bytes[0], (unsigned)test.bytes[1], (unsigned)test.bytes[2], (unsigned)test.bytes[3]);
  ESP_LOGI("TEST", "0x%08X == 0x%08X", (unsigned)test.u32, (unsigned)exp_u32); 
  TEST_ASSERT_EQUAL_UINT32(test.u32, exp_u32);          
}


TEST_CASE("Compare functions for generating serial", "[device]") 
{
  char serial_str_simple_ver[20] = {0};
  Get_SerialNum();
  get_serial_number(serial_str_simple_ver);
  ESP_LOGI(TAG, "serial_str_simple_ver: %s", serial_str_simple_ver);
  TEST_ASSERT_EQUAL_STRING(Str_Dev_Serial_Number, serial_str_simple_ver);
}