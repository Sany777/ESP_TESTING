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

#define DNS_SERVER_PORT 53

static bool dns_hostname_match(const uint8_t *packet, size_t len, const char *hostname);
static int dns_response_handler(const uint8_t *query, int query_len, uint8_t *response, void *user_ctx);


bool start_dns_server(ipv4_t host_ip) 
{
    ipv4_t client_ip = { .bytes = {0,0,0,0} };
    serv_task_conf_t *dns_task_conf = (serv_task_conf_t *)malloc(sizeof(serv_task_conf_t));
    if(dns_task_conf){
        dns_task_conf->bit_ctrl = BIT_DNS_SERVER_RUN;
        dns_task_conf->serv_conf.port = DNS_SERVER_PORT;
        dns_task_conf->serv_conf.ip = client_ip;
        dns_task_conf->create_response_handler = dns_response_handler;
        dns_task_conf->user_ctx = CTX_U32(host_ip.u32);
        device_set_bits(BIT_DNS_SERVER_RUN);
        if(start_udp_server(dns_task_conf)){
            return true;
        }
        free(dns_task_conf);
    }
     device_clear_bits(BIT_DNS_SERVER_RUN);
    return false;
}

#define DNS_PTR_OFFSET   0xC00C
#define DNS_TYPE_A       1
#define DNS_CLASS_IN     1
#define DNS_TTL_SEC      120 // lifetime


typedef struct {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} __attribute__((packed)) dns_header_t;

typedef struct {
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t data_len;
    uint32_t addr;
} __attribute__((packed)) dns_answer_t;


static
int dns_response_handler(const uint8_t *query, int query_len, uint8_t *response, void *ctx)
{
    ipv4_t host_ip;
    host_ip.u32 = CTX_TO_U32(ctx);
    const char *hostname = DEF_LOCAL_HOSTNAME;

    if (!query || !response || query_len == 0 || host_ip.u32 == 0) return -1;
    if (!dns_hostname_match(query, query_len, hostname)) return -1;

    memset(response, 0, UDP_SEND_BUF_SIZE);
    dns_header_t *hdr = (dns_header_t*) response;
    memcpy(hdr, query, sizeof(dns_header_t));
    hdr->ancount = htons(1);
    int offset = sizeof(dns_header_t);

    while (query[offset] && offset < UDP_SEND_BUF_SIZE) {
        if (offset + query[offset] >= UDP_SEND_BUF_SIZE) return -1;
        offset += query[offset] + 1;
    }
    if ((offset += 5) >= UDP_SEND_BUF_SIZE) return -1;

    memcpy(response + sizeof(dns_header_t), query + sizeof(dns_header_t), offset - sizeof(dns_header_t));

    if (offset + sizeof(uint16_t) + sizeof(dns_answer_t) > UDP_SEND_BUF_SIZE) return -1;

    *(uint16_t *)(response + offset) = htons(DNS_PTR_OFFSET);
    offset += sizeof(uint16_t);
    dns_answer_t *ans = (dns_answer_t *)(response + offset);
    ans->type = htons(DNS_TYPE_A);
    ans->class = htons(DNS_CLASS_IN);
    ans->ttl = htonl(DNS_TTL_SEC);
    ans->data_len = htons(IPV4_SIZE);
    ans->addr = host_ip.u32;
    return offset + sizeof(dns_answer_t);
}


static
bool dns_hostname_match(const uint8_t *packet, size_t len, const char *hostname) 
{
    if (!packet || !hostname || len < sizeof(dns_header_t)) return false;
    size_t offset = sizeof(dns_header_t);
    const char *h = hostname;

    while (offset < len && packet[offset] != 0 && *h) {
        uint8_t label_len = packet[offset++];
        if (label_len == 0 || offset + label_len > len) return false;
        for (uint8_t i = 0; i < label_len; ++i) {
            if (*h == '\0' || tolower((unsigned char)packet[offset + i]) != tolower((unsigned char)*h)) {
                return false;
            }
            h++;
        }
        offset += label_len;
        if (*h == '.') h++;
    }
    return (packet[offset] == 0 && *h == '\0');
}