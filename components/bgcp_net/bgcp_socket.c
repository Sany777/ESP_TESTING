// #include "wifi_service.h"

// #include "device.h"

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"
// #include "esp_log.h"
// // #include "esp_wifi.h"

// #include "lwip/sockets.h"
// #include "lwip/netdb.h"
// #include "lwip/sys.h"



// #define WIFI_CONNECTED_BIT BIT0
// #define WIFI_DISCONNECTED_BIT BIT1
// #define UDP_RECV_BUFFER_SIZE 255
// #define UDP_SEND_BUFFER_SIZE 255
// #define ADDR_STR_LEN 128

// #include <string.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include "esp_log.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/event_groups.h"
// #include "freertos/task.h"
// #include "lwip/sockets.h"
// #include "lwip/inet.h"
// #include <errno.h>


// #define HOST_IP_ADDR    "192.168.4.1"
// #define BLAUBERG_SERVER "blauberg-group-cloud.com"
// #define BLAUBERG_PORT    1336
// #define LOCAL_PORT       4000


// #include "bgcp.h"


// #define WIFI_CONNECTED_BIT BIT0

// // Налаштування буферів і порту
// #define UDP_RECV_BUFFER_SIZE 512
// #define ADDR_STR_LEN 128
// #define CONFIG_UDP_LOCAL_PORT 1234

// static const char *TAG = "BGCP CONNECTION";


// int create_UDP_client_ipv4_socket(int addr_family, int ip_protocol)
// {
//     int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
//     if (sock < 0) {
//         ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
//         return -1;
//     }

//     // Set receive timeout of 2 seconds
//     struct timeval timeout = { .tv_sec = 2, .tv_usec = 0 };
//     setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
//     return sock;
// }

// int create_UDP_server_ipv4_socket(const char *address, int port)
// {
//     struct sockaddr_in dest_addr = { .sin_family = AF_INET, .sin_port = htons(port), };

//     // Determine address based on input
//     if (strcmp(address, "0.0.0.0") == 0) {
//         dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//     } else if (inet_pton(AF_INET, address, &dest_addr.sin_addr) <= 0) {
//         ESP_LOGE(TAG, "Invalid IP address: %s", address);
//         return -1;
//     }

//     // Create UDP socket
//     int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//     if (sock < 0) {
//         ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
//         return -1;
//     }

//     // Enable address reuse and set timeouts
//     int opt = 1;
//     setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//     struct timeval timeout = { .tv_sec = 2, .tv_usec = 0 };
//     setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
//     setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

//     // Bind socket to address and port
//     if (bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
//         ESP_LOGE(TAG, "Socket bind failed: errno %d", errno);
//         close(sock);
//         return -1;
//     }

//     ESP_LOGI(TAG, "Socket successfully bound to port %d", port);
//     return sock;
// }

// void close_udp_socket(int *udp_socket)
// {
//     if (*udp_socket >= 0) {
//         shutdown(*udp_socket, 0);
//         close(*udp_socket);
//         *udp_socket = -1;
//     }
// }

// void udp_server_task(void *pvParameters)
// {
//     uint8_t rx_buffer[UDP_RECV_BUFFER_SIZE];
//     char addr_str[ADDR_STR_LEN];
//     struct sockaddr_storage source_addr;
//     socklen_t socklen = sizeof(source_addr);
//     int udp_socket = -1;
//     while (1) {
//         vTaskDelay(pdMS_TO_TICKS(100));
//         device_wait_bits(BIT_AP_CLIENT, portMAX_DELAY);
//         if (udp_socket == -1) {
//             udp_socket = create_UDP_server_ipv4_socket("0.0.0.0", CONFIG_UDP_LOCAL_PORT);
//             if (udp_socket < 0) {
//                 vTaskDelay(pdMS_TO_TICKS(2000));
//                 continue;
//             }
//         }

//         while (1){
//             int len = recvfrom(udp_socket, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
            
//             if (len < 0) {
//                 if (errno == EAGAIN || errno == EWOULDBLOCK){
//                     ESP_LOGI(TAG, "recvfrom timed out, retrying...");
//                 } else {
//                     ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
//                     break;
//                 }
//             } else {
//                 rx_buffer[len] = 0;
//                 // Get sender IP address
//                 if (source_addr.ss_family == AF_INET){
//                     inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
//                 }
//                 if (check_bgcp_protocol(rx_buffer, len)){
//                     device_wait_bits(BIT_PARSE, portMAX_DELAY);
//                     device_set_bits(BIT_PARSE);
//                     parse(rx_buffer, len);
//                     // Send response if needed
//                     if (get_req_queue_size()){
//                         int send_err = sendto(udp_socket, get_req_queue_data(), get_req_queue_size(), 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
//                         if (send_err < 0) {
//                             ESP_LOGE(TAG, "Failed to send response: errno %d", errno);
//                         }
//                     }
//                     device_clear_bits(BIT_PARSE);
//                 }
//             }
//         }
//         if (!(device_get_bits()&BIT_AP_CLIENT)){
//             close_udp_socket(&udp_socket);
//             vTaskDelay(pdMS_TO_TICKS(2000));
//         }
//     }

//     vTaskDelete(NULL);
// }

