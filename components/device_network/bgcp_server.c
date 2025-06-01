#include "udp_server.h"
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



static int bgcp_response_handler(const uint8_t *query, int query_len, uint8_t *response, void *ctx);
static bool check_bgcp_header(const uint8_t *query, int query_len);



bool start_bgcp_server(serv_conf_t *serv_conf, device_data_t *device) 
{
    ipv4_t client_ip = {.bytes = {0,0,0,0} };
    serv_task_conf_t *bgcp_task_conf = (serv_task_conf_t *) malloc(sizeof(serv_task_conf_t));
    if(bgcp_task_conf){

        bgcp_task_conf->bit_ctrl = BIT_BGCP_SERVER_RUN;
        bgcp_task_conf->serv_conf = *serv_conf;
        bgcp_task_conf->user_ctx = device;

        device_set_bits(BIT_BGCP_SERVER_RUN);

        if(start_udp_server(bgcp_task_conf)){
            return true;
        }

        free(bgcp_task_conf);
    }
     device_clear_bits(BIT_BGCP_SERVER_RUN);
    return false;
}

static
bool check_bgcp_header(const uint8_t *query, int query_len)
{

    return false;
}

static 
int bgcp_response_handler(const uint8_t *query, int query_len, uint8_t *response, void *ctx)
{

    if(check_bgcp_header(query, query_len)){

    }
    return query_len;
}