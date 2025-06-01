#ifndef UDP_SERVER_H
#define UDP_SERVER_H




#include "device.h"







bool start_dns_server(ipv4_t host_ip);

bool start_ping_server(void);

bool start_bgcp_server(serv_conf_t *serv_conf, device_data_t *device);







#endif