#include "udp_comm.h"

#include "event_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sockets.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_event.h"
#include <string.h>
#include <stdint.h>  
#include <arpa/inet.h>


// UDP echo (RFC 862) port
#define UDP_ECHO_PORT 7

static int echo_response_handler(const uint8_t *query, int query_len, uint8_t *response, void *ctx);


bool start_ping_server() 
{
    ipv4_t client_ip = {.bytes = {0,0,0,0} };
    serv_task_conf_t *ping_task_conf = (serv_task_conf_t *)malloc(sizeof(serv_task_conf_t));
    if(ping_task_conf){
        ping_task_conf->bit_ctrl = BIT_ECHO_SERVER_RUN;
        ping_task_conf->serv_conf.port = UDP_ECHO_PORT;
        ping_task_conf->serv_conf.ip = client_ip;
        ping_task_conf->create_response_handler = echo_response_handler;
        ping_task_conf->user_ctx = NULL;
        
        device_set_bits(BIT_ECHO_SERVER_RUN);

        if(start_udp_server(ping_task_conf)){
            return true;
        }

        free(ping_task_conf);
    }
     device_clear_bits(BIT_ECHO_SERVER_RUN);
    return false;
}


static 
int echo_response_handler(const uint8_t *query, int query_len, uint8_t *response, void *ctx)
{
    memcpy(response, query, query_len);
    return query_len;
}