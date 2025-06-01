#pragma once

#define SERIAL_STR_LEN 17



#define BIT_BGCP_SERVER_RUN BIT0
#define BIT_BGCP_PARSE BIT1
#define BIT_HTTP_SERVER_RUN BIT2
#define BIT_CLOUD_CLIENT_RUN BIT3
#define BIT_I2C_OPER_CPLT BIT4
#define BIT_I2C_OPER_ERR BIT5
#define BIT_WIFI_STA_CONNECTED BIT6
#define BIT_WIFI_STA_CONN_FAIL BIT7
#define BIT_WIFI_AP_START BIT8
#define BIT_WIFI_CONNECTED BIT9
#define BIT_WIFI_AUTO_CONN BIT10
#define BIT_DNS_SERVER_RUN BIT11
#define BIT_ECHO_SERVER_RUN BIT12



#define I2C_MASTER_SCL_PIN          17
#define I2C_MASTER_SDA_PIN          18
#define I2C_MASTER_NUM              0
#define CONFIG_I2C_CLK_SPEED_HZ     100000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0

#ifndef OFF		
    #define OFF 0
#endif

#ifndef ON		
    #define ON 0
#endif

// -------------- SUPPORT EVENT-DRIVEN SYSTEM ------------------


#define IP_DEFAULT_ADDR     {192, 168, 4, 1}

#define HOST_REMOTE_VENTS   "blauberg-group-clod.com"
#define PORT_REMOTE_VENTS   1326

#define DEF_LOCAL_HOSTNAME  "device.local"
#define PORT_LOCAL_SERVICE  4000