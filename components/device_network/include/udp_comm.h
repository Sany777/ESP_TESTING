#ifndef UDP_COMM_H
#define UDP_COMM_H

#include "device.h"


#define UDP_SEND_BUF_SIZE 512
#define UDP_RECV_BUF_SIZE 512
#define UDP_SERV_TASK_PRIO 7
#define UDP_SERV_TIMEOUT_SEC 2
#define UDP_SERV_STACK_SIZE 4096
#define UDP_CLIENT_TASK_PRIO  5
#define UDP_CLIENT_STACK_SIZE 4096



typedef int (*create_response_handler_t)(const uint8_t *query, int query_len, uint8_t *response, void *user_ctx);
typedef int (*create_request_handler_t)(uint8_t *response_buf, void *user_ctx);

typedef struct {
    uint32_t bit_ctrl;
    serv_conf_t serv_conf;
    create_response_handler_t create_response_handler;
    uint32_t bit_en;
    void *user_ctx;
} serv_task_conf_t;


typedef struct {
    uint32_t bit_ctrl;
    serv_conf_t serv_conf;
    create_request_handler_t create_request_handler;
    create_response_handler_t parse_response_handler;
    void *user_ctx;
    int send_inerval;
} client_task_conf_t;



bool start_udp_server(serv_task_conf_t *conf);

bool start_udp_client(client_task_conf_t *conf);






#endif  // UDP_COMM_H