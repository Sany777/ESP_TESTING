#include "udp_comm.h"

#include "device.h"
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


const char *TAG = "udp_sever";


static int udp_socket_create(ipv4_t ip, uint16_t port);


static
void udp_server_task(void *pv)
{
    serv_task_conf_t *conf = (serv_task_conf_t *) pv;
    if (conf == NULL || conf->create_response_handler == NULL) {
        ESP_LOGE(TAG, "Invalid server configuration");
        vTaskDelete(NULL);
        return;
    }
    
    int sock = -1;
    struct sockaddr_in client_addr = { 0 };
    socklen_t client_addr_len = sizeof(client_addr);
    uint8_t query[UDP_RECV_BUF_SIZE], response[UDP_SEND_BUF_SIZE];
    unsigned bits;
    while ((bits = device_get_bits()) & conf->bit_ctrl) {
        vTaskDelay(pdMS_TO_TICKS(10));
        if (sock < 0) {
            sock = udp_socket_create(conf->serv_conf.ip, conf->serv_conf.port);
            if (sock < 0) {
                ESP_LOGE(TAG, "Socket creation failed: errno %d", errno);
                vTaskDelay(pdMS_TO_TICKS(2000));
                continue;
            }
        }

        int query_len = recvfrom(sock, query, sizeof(query), 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (query_len < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                vTaskDelay(pdMS_TO_TICKS(200));
            } else {
                ESP_LOGE(TAG, "recvfrom failed: errno %d. Restarting socket.", errno);
                close(sock);
                sock = -1;
                vTaskDelay(pdMS_TO_TICKS(2000));
            }
            continue;
        }

        ESP_LOGD(TAG, "Received %d bytes from %s:%d", query_len,
                 inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        int resp_len = conf->create_response_handler(query, query_len, response, conf->user_ctx);
        if (resp_len > 0) {
            sendto(sock, response, resp_len, 0, (struct sockaddr *)&client_addr, client_addr_len);
        }
    }

    ESP_LOGI(TAG, "UDP server stopped on port %u", conf->serv_conf.port);
    if (sock >= 0) close(sock);
    free(pv);
    vTaskDelete(NULL);
}


bool start_udp_server(serv_task_conf_t *conf)
{
    return xTaskCreate(udp_server_task, "", UDP_SERV_STACK_SIZE, conf, UDP_SERV_TASK_PRIO, NULL) == pdTRUE;
}


static
int udp_socket_create(ipv4_t ip, uint16_t port)
{
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Socket creation failed: errno %d", errno);
        return -1;
    }

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = ip.u32;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ESP_LOGE(TAG, "Socket bind failed: errno %d", errno);
        close(sock);
        return -1;
    }

    struct timeval timeout = { .tv_sec = UDP_SERV_TIMEOUT_SEC, .tv_usec = 0 };
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        ESP_LOGW(TAG, "Failed to set recv timeout");
    }
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        ESP_LOGW(TAG, "Failed to set send timeout");
    }

    ESP_LOGI(TAG, "UDP socket created and bound to port %u", port);

    return sock;
}