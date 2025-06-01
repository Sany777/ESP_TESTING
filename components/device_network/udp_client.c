#include "udp_comm.h"
#include "comm_types.h"

#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/ip_addr.h"
#include "esp_log.h"


static const char *TAG = "udp_client";

bool resolve_hostname_to_ip4(const char *hostname, ip4_addr_t *out_ip) 
{
    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    int err = getaddrinfo(hostname, NULL, &hints, &res);
    if (err != 0 || res == NULL) return false;
    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    out_ip->addr = addr->sin_addr.s_addr;
    freeaddrinfo(res);
    return true;
}


static 
void udp_client_task(void *pv) 
{
    client_task_conf_t *task_conf = (client_task_conf_t *)pv;
    
    if(task_conf == NULL 
            || task_conf->send_inerval == 0 
            || task_conf->parse_response_handler == NULL 
            || task_conf->create_request_handler == NULL){
        ESP_LOGE(TAG, "arg err");
        vTaskDelete(NULL);
        return;
    }
    
    const serv_conf_t *conf = &task_conf->serv_conf;
    ip4_addr_t resolved_ip = { .addr = conf->ip.u32 };
    if (resolved_ip.addr == 0) {
        if (conf->hostname == NULL || !resolve_hostname_to_ip4(conf->hostname, &resolved_ip)) {
            ESP_LOGE(TAG, "DNS failed for host: %s", conf->hostname);
            vTaskDelete(NULL);
            return;
        }
    }

    struct sockaddr_in dest_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(conf->port),
        .sin_addr.s_addr = resolved_ip.addr,
    };

    int sock = -1;

    uint8_t rx_buffer[UDP_RECV_BUF_SIZE];
    uint8_t tx_buffer[UDP_RECV_BUF_SIZE];
    unsigned bits;
    while ((bits = device_get_bits()) & task_conf->bit_ctrl) {
        vTaskDelay(pdMS_TO_TICKS(task_conf->send_inerval)); 
        if (sock < 0) {
            sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
            if (sock < 0) {
                ESP_LOGE(TAG, "Client socket creation failed: errno %d", errno);
                vTaskDelay(pdMS_TO_TICKS(2000));
                continue;
            }
        }
        int req_len = task_conf->create_request_handler(rx_buffer, task_conf->user_ctx);
        if(req_len <= 0) {
            continue;
        }
        int err = sendto(sock, rx_buffer, req_len, 0,
                         (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(TAG, "UDP send failed: errno %d", errno);
            continue;
        } 
        struct sockaddr_in source_addr;
        socklen_t socklen = sizeof(source_addr);
        int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, MSG_DONTWAIT,
                            (struct sockaddr *)&source_addr, &socklen);
        if (len <= 0) {
            continue;
        }

        len = task_conf->parse_response_handler(rx_buffer, len, tx_buffer, task_conf->user_ctx);
        if (len > 0) {
            sendto(sock, tx_buffer, len, 0, (struct sockaddr *)&source_addr, socklen);
        }
    }

    ESP_LOGI(TAG, "UDP client stopping...");
    if(sock >= 0)close(sock);
    free(task_conf);
    vTaskDelete(NULL);
}


bool start_udp_client(client_task_conf_t *conf) 
{
    if (!conf || !conf->serv_conf.hostname || conf->serv_conf.port == 0) {
        ESP_LOGE(TAG, "Invalid configuration");
        return false;
    }
    BaseType_t res = xTaskCreatePinnedToCore(
        udp_client_task,
        "udp_client_task",
        UDP_CLIENT_STACK_SIZE,
        conf,
        UDP_CLIENT_TASK_PRIO,
        NULL,
        tskNO_AFFINITY
    );

    if (res != pdPASS) {
        ESP_LOGE(TAG, "Failed to create UDP client task");
        return false;
    }

    return true;
}