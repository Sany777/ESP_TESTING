// #include "wifi_service.h"


// #define HOST_IP_ADDR 			"192.168.4.1"
// #define WEB_SERVER 				"blauberg-group-cloud.com"
// #define blauberg_PORT 			"1336"


// void udp_client_recv_task(void *pvParameters)
// {
//     char rx_buffer[255];
//     int addr_family = 0;
//     int ip_protocol = 0;
//     uint16_t i = 0, chksum;
//     int err = 0;
//     struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
//     socklen_t socklen = sizeof(source_addr);

//     while (1) 
//     {
//         int len = 0;
//         if (device.sock > 0)// && device.wifi_conn_status  == IP_EVENT_STA_GOT_IP) 
//         {
//             len = recvfrom(device.sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
//         }
//         // Error occurred during receiving
//         if (len < 0) {
//             ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
//             //vTaskDelete(NULL);
//         }
//         // Data received
//         else 
//         {
            
//             rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
       
//             if(check_protocol(rx_buffer, len) == 1) // Если протокол BGCP и совпала контрольная сумма
//             {
//                 error_counter = 0;
//                 if(rx_buffer[2] == BGCP_ETH) // если тип протокола для Ethernet в локальной сети
//                 {
//                     if(((memcmp(&rx_buffer[4], device.current_id, rx_buffer[3]) == 0) && (Vent_param_wifi.setup_mode == 0)) || (Vent_param_wifi.setup_mode == 1)) // если совпал ID мастера, которым управляем
//                     {
//                         uint16_t jump_size = 0, page = 0, param, param_size, r_pos;
//                         uint8_t flag_check_func = 1, BGCP_func;
                            
//                         r_pos = 4 + rx_buffer[3]; // позиция в масиве, где начинается блок FUNC
//                         r_pos += 1 + rx_buffer[r_pos]; // пароль
//                         //********* цикл в котором смотрим блок FUNC и DATA *********//
//                         for(; r_pos < len - 2; r_pos++)
//                         {
//                             param_size = 1;
                            
//                             if((flag_check_func == 1) || (rx_buffer[r_pos] == (char)BGCP_CMD_FUNC)) // если номер функции
//                             {
//                                 device.disconnect_with_skynet = 0;
//                                 if(rx_buffer[r_pos] == (char)BGCP_CMD_FUNC)
//                                     r_pos++;
//                                 flag_check_func = 0;
//                                 //=== проверяем номер функции ===//
//                                 BGCP_func = rx_buffer[r_pos];
//                                 if(BGCP_func != BGCP_FUNC_RESP) // если функция не поддерживается
//                                     break;
//                                 //===========================================//
//                                 continue;
//                             }
//                             else if(rx_buffer[r_pos] == (char)BGCP_CMD_PAGE) // если номер страницы с параметрами
//                             {
//                                 page = rx_buffer[++r_pos];
//                                 continue;
//                             }
//                             else if(rx_buffer[r_pos] == (char)BGCP_CMD_SIZE) // если размер параметра
//                             {
//                                 param_size = rx_buffer[++r_pos];
//                                 r_pos++;
//                             }
//                             else if(rx_buffer[r_pos] == (char)BGCP_CMD_NOT_SUP) // если параметр не поддерживается
//                             {
//                                 r_pos++;
//                                 continue;
//                             }
//                             //=== определяем размер перехода ===//
//                             jump_size = param_size;
//                             //==================================//
                            
//                             //if(wifi_setup_mode == 0)
//                             //    ping_conn_master = 1;
                            
//                             //******* проверка параметров ******//
//                             param = (uint16_t)(page << 8) | (uint16_t)(rx_buffer[r_pos]);

//                             switch(param)
//                             {
//                                 case 0:
//                                     break;
//                                 case P_POWER:
//                                 {
//                                     //printf("P_POWER\n");
//                                     device.button_1_state = rx_buffer[r_pos+1];
//                                     if(device.buttons_update_flag == 1 || device.name_send_func != Send_Custom_data) device.button_1 = rx_buffer[r_pos+1];
//                                     //printf("device.button_1_state %d, device.button_1: %d\n", device.button_1_state, device.button_1);
//                                     break;
//                                 }
//                                 case P_SPEED_MODE:
//                                 {
//                                     //printf("P_SPEED_MODE\n");
//                                     device.button_2_state = rx_buffer[r_pos+1];
//                                     if(device.buttons_update_flag == 1 || device.name_send_func != Send_Custom_data)  device.button_2 = rx_buffer[r_pos+1];
//                                     //printf("device.button_2_state %d, device.button_2: %d\n", device.button_2_state, device.button_2);
//                                     break;
//                                 }
//                                 case P_BPS_MODE:
//                                 {
//                                     //printf("P_BPS_MODE\n");
//                                     device.button_4_state = rx_buffer[r_pos+1];
//                                     if(device.buttons_update_flag == 1 || device.name_send_func != Send_Custom_data) device.button_4 = rx_buffer[r_pos+1];
//                                     //printf("device.button_4_state %d, device.button_4: %d\n", device.button_4_state, device.button_4);
//                                     break;
//                                 }
//                                 case P_WIFI_CONN:
//                                 {
//                                     //printf("P_WIFI_CONN\n");
//                                     //device.button_5_state = rx_buffer[r_pos+1];
//                                     //printf("device.button_5_state %d\n", device.button_5_state);
//                                     break;
//                                 }
//                                 case P_StateFILTER:
//                                 {
//                                     //printf("P_StateFILTER\n");
//                                     if(rx_buffer[r_pos+1] == 0)
//                                         device.button_3_state = OFF;
//                                     else
//                                         device.button_3_state = ON;
//                                     if(device.buttons_update_flag == 1 || device.name_send_func != Send_Custom_data) device.button_3 = device.button_3_state;
//                                     //printf("device.button_3_state %d, device.button_3: %d\n", device.button_3_state, device.button_3);
//                                     break;
//                                 }
//                                 case P_DevicePWD:
//                                 {
//                                     //printf("P_DevicePWD\n");
//                                     uint8_t temp;
//                                     if(param_size <= 8)
//                                     {
//                                         memset(Vent_param_wifi.DevPWD, '\0', 9);
//                                         for(temp = 0; temp < param_size; temp++)
//                                             Vent_param_wifi.DevPWD[temp] = rx_buffer[r_pos+1+temp];
//                                         //printf("DevicePWD: %s\n", Vent_param_wifi.DevPWD);
//                                     }
//                                     break;
//                                 }
                                
//                                 case P_ALARM:
//                                 {
//                                     //printf("P_ALARM\n");
//                                     if(rx_buffer[r_pos+1] == 1)
//                                         device.button_6_state = ON;
//                                     else
//                                         device.button_6_state = OFF;
//                                     if(device.buttons_update_flag == 1 || device.name_send_func != Send_Custom_data)  device.button_6 = device.button_6_state;
//                                     //printf("device.button_6_state %d, device.button_6: %d\n", device.button_6_state, device.button_6);
//                                     break;
//                                 }
//                                 case P_CurETHR_IP:
//                                 {
//                                     //printf("P_CurETHR_IP\n");
//                                     if((rx_buffer[r_pos+1] != 0) || (rx_buffer[r_pos+2] != 0) || (rx_buffer[r_pos+3] != 0) || (rx_buffer[r_pos+4] != 0))
//                                     {
//                                         device.eth_IP[0] = rx_buffer[r_pos+1];
//                                         device.eth_IP[1] = rx_buffer[r_pos+2];
//                                         device.eth_IP[2] = rx_buffer[r_pos+3];
//                                         device.eth_IP[3] = rx_buffer[r_pos+4];
                                        
//                                         printf("device.eth_IP[0]: %d\n", device.eth_IP[0]);
//                                         printf("device.eth_IP[1]: %d\n", device.eth_IP[1]);
//                                         printf("device.eth_IP[2]: %d\n", device.eth_IP[2]);
//                                         printf("device.eth_IP[3]: %d\n\n", device.eth_IP[3]);
//                                         //device.name_send_func = Send_Sync_data_control;
//                                     }
//                                     else
//                                     {
//                                         device.eth_IP[0] = 0;
//                                         device.eth_IP[1] = 0;
//                                         device.eth_IP[2] = 0;
//                                         device.eth_IP[3] = 0;
//                                     }
//                                     break;
//                                 }
//                                 case P_CurWIFI_IP:
//                                 {
//                                     //printf("P_CurWIFI_IP\n");
//                                     if((rx_buffer[r_pos+1] != 0) || (rx_buffer[r_pos+2] != 0) || (rx_buffer[r_pos+3] != 0) || (rx_buffer[r_pos+4] != 0))
//                                     {
//                                         device.wifi_IP[0] = rx_buffer[r_pos+1];
//                                         device.wifi_IP[1] = rx_buffer[r_pos+2];
//                                         device.wifi_IP[2] = rx_buffer[r_pos+3];
//                                         device.wifi_IP[3] = rx_buffer[r_pos+4];
                                        
//                                         printf("device.wifi_IP[0]: %d\n", device.wifi_IP[0]);
//                                         printf("device.wifi_IP[1]: %d\n", device.wifi_IP[1]);
//                                         printf("device.wifi_IP[2]: %d\n", device.wifi_IP[2]);
//                                         printf("device.wifi_IP[3]: %d\n\n", device.wifi_IP[3]);
                                        
//                                     }
//                                     else
//                                     {
//                                         device.wifi_IP[0] = 0;
//                                         device.wifi_IP[1] = 0;
//                                         device.wifi_IP[2] = 0;
//                                         device.wifi_IP[3] = 0;
//                                     }
//                                     break;
//                                 }
//                                 case P_WIFI_SSID:
//                                 {
//                                     //printf("P_WIFI_SSID\n");
//                                     uint8_t temp;
//                                     if(param_size <= 32)
//                                     {
//                                         memset(Vent_param_wifi.Wi_Fi_mode_station.ssid, '\0', 33);
//                                         for(temp = 0; temp < param_size; temp++)
//                                             Vent_param_wifi.Wi_Fi_mode_station.ssid[temp] = rx_buffer[r_pos+1+temp];
//                                         //Send_to_USB(1, wi_fi_setup.ssid, strlen(wi_fi_setup.ssid));///////////////////
//                                         printf("SSID: %s\n", Vent_param_wifi.Wi_Fi_mode_station.ssid);
//                                         Vent_param_wifi.setup_mode = 2;
//                                     }
//                                     break;
//                                 }
//                                 case P_WIFI_PWD:
//                                 {
//                                     //printf("P_WIFI_PWD\n");
//                                     uint8_t temp, size_id;
//                                     if(param_size <= 64)
//                                     {
//                                         memset(Vent_param_wifi.Wi_Fi_mode_station.PWD, '\0', 65);
//                                         for(temp = 0; temp < param_size; temp++)
//                                             Vent_param_wifi.Wi_Fi_mode_station.PWD[temp] = rx_buffer[r_pos+1+temp];
//                                         //Send_to_USB(1, wi_fi_setup.password, strlen(wi_fi_setup.password));///////////////////
//                                         printf("PWD: %s\n", Vent_param_wifi.Wi_Fi_mode_station.PWD);
                                        
//                                     }
//                                     size_id = rx_buffer[3];
//                                     if(size_id <= 24)
//                                     {
//                                         memset(device.current_id, '\0', 25);
//                                         for(temp = 0; temp < size_id; temp++)
//                                             device.current_id[temp] = rx_buffer[temp+4];
//                                     }
//                                     break;
//                                 }
                                
//                                 default: // если параметр не поддерживается
//                                 {
//                                     break;
//                                 }
//                             }
//                             //*******************************//
//                             r_pos += jump_size;
//                         }

//                     }
//                 }
//             }
//         }
//         vTaskDelay(100 / portTICK_PERIOD_MS);
//     }
// }


// void udp_client_task(void *pvParameters)
// {
//     char rx_buffer[128];

//     int addr_family = 0;
//     int ip_protocol = 0;

//     char temp_transmit_data[256+10+23];
//     char current_pwd[9] = "";
//     char current_id[25] = "";
//     char id_setup_mode[17] = "DEFAULT_DEVICEID";
//     uint8_t ping_conn_master = 0;
//     uint8_t send = 1;
//     counters.udpate_data_counter = 0;   
//     uint16_t i = 0, chksum;
//     int err = 0;
//     char tmp[255];
    
//     while (1) 
//     {
//         struct sockaddr_in dest_addr;
//         struct sockaddr_in test_addr;
        
//         if(device.wifi_IP[0] + device.wifi_IP[1] + device.wifi_IP[2] + device.wifi_IP[3] > 0)
//         {
//             device.skynet_IP[0] = device.wifi_IP[0];
//             device.skynet_IP[1] = device.wifi_IP[1];
//             device.skynet_IP[2] = device.wifi_IP[2];
//             device.skynet_IP[3] = device.wifi_IP[3];
//         }
//         else if(device.eth_IP[0] + device.eth_IP[1] + device.eth_IP[2] + device.eth_IP[3] > 0)
//         {
//             device.skynet_IP[0] = device.eth_IP[0];
//             device.skynet_IP[1] = device.eth_IP[1];
//             device.skynet_IP[2] = device.eth_IP[2];
//             device.skynet_IP[3] = device.eth_IP[3];
//         }

//         memset(tmp, '\0', 255); 
//         if(Vent_param_wifi.setup_mode == 1)
//         { 
//             sprintf(tmp,"setup_mode: %d.%d.%d.%d\n", device.my_GW[0],device.my_GW[1],device.my_GW[2], device.my_GW[3]);
//         }
//         else if (device.name_send_func == Send_Get_IP)
//         {
//             sprintf(tmp,"Send_Get_IP: %d.%d.%d.%d\n", device.my_broadcast[0],device.my_broadcast[1],device.my_broadcast[2], device.my_broadcast[3]);
//         }
//         else
//         { 
//             sprintf(tmp,"%d.%d.%d.%d\n", device.skynet_IP[0],device.skynet_IP[1],device.skynet_IP[2], device.skynet_IP[3]);
//         }
//         dest_addr.sin_addr.s_addr = inet_addr(tmp); //inet_addr(tmp);
//         dest_addr.sin_family = AF_INET;
//         dest_addr.sin_port = htons(local_PORT);
//         addr_family = AF_INET;
//         ip_protocol = IPPROTO_IP;

//         test_addr.sin_addr.s_addr = inet_addr("1.1.1.1"); //inet_addr(tmp);
//         test_addr.sin_family = AF_INET;
//         test_addr.sin_port = htons(local_PORT);

//         device.sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
//         if (device.sock < 0) {
//             ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
//             break;
//         }
//         else
//         {
//             ESP_LOGI(TAG, "Socket created, sending to %s:%d", tmp, local_PORT);
//             device.udp_sta = 1;
//         }
        
        
//         if(eTaskGetState(udp_recv_task_handle) == 4) xTaskCreate(udp_client_recv_task, "udp_client_recv_task",4*1024, NULL, 2 | portPRIVILEGE_BIT, &udp_recv_task_handle);
//         else if(eTaskGetState(udp_recv_task_handle) == 3) xTaskResumeFromISR(udp_recv_task_handle);

//         while (1) 
//         {
//             //if(eTaskGetState(udp_recv_task_handle) == 4) xTaskCreate(udp_client_recv_task, "udp_client_recv_task",10*1024, NULL, 2 | portPRIVILEGE_BIT, &udp_recv_task_handle);
//             memset(temp_transmit_data, '\0', sizeof(temp_transmit_data));
//             i = 0;
//             temp_transmit_data[i++] = 0xFD;
//             temp_transmit_data[i++] = 0xFD;
//             temp_transmit_data[i++] = BGCP_ETH;
//             temp_transmit_data[i++] = strlen(device.current_id);
//             strcat(temp_transmit_data, device.current_id);
//             i += strlen(device.current_id);
//             temp_transmit_data[i++] = strlen(Vent_param_wifi.DevPWD);
//             strcat(temp_transmit_data, Vent_param_wifi.DevPWD);
//             i += strlen(Vent_param_wifi.DevPWD);

//             if(device.name_send_func == Send_Custom_data) // отправляем данные о нажатии кнопок
//             {
//                 if(device.button_3 == 0xff || device.button_6 == 0xFF) temp_transmit_data[i++] = BGCP_FUNC_W; //RW
//                 else temp_transmit_data[i++] = BGCP_FUNC_RW;
//                 temp_transmit_data[i++] = P_POWER;
//                 temp_transmit_data[i++] = device.button_1;
//                 temp_transmit_data[i++] = P_SPEED_MODE;
//                 temp_transmit_data[i++] = device.button_2;

//                 if(device.button_4 == 1)
//                 {
//                     temp_transmit_data[i++] = P_BPS_MODE;
//                     temp_transmit_data[i++] = device.button_4;
//                     temp_transmit_data[i++] = P_SetBpsRotorMANUAL;
//                     temp_transmit_data[i++] = 100;
//                 }
//                 else if(device.button_4 == 2)
//                 {
//                     temp_transmit_data[i++] = P_BPS_MODE;
//                     temp_transmit_data[i++] = device.button_4;
//                     temp_transmit_data[i++] = P_SetBpsRotorMANUAL;
//                     temp_transmit_data[i++] = 0;    
//                 }

//                 if(device.button_6 == 0xff)
//                 {
//                     temp_transmit_data[i++] = C_RESET_ALARM;
//                     temp_transmit_data[i++] = device.button_6;
//                     device.button_6 = 0;
//                 }
                    
//                 if(device.button_3 == 0xff)
//                 {
//                     temp_transmit_data[i++] = C_RESET_FILTER_TIMER;
//                     temp_transmit_data[i++] = device.button_3;
//                     device.button_3 = 0; 
//                 }
                    
//                 chksum = calculate_chksum_BGCP(&temp_transmit_data[2], i-2);
//                 temp_transmit_data[i++] = (uint8_t)chksum;
//                 temp_transmit_data[i++] = (uint8_t)(chksum >> 8);
//                 err = sendto(device.sock, temp_transmit_data, i, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
//                 memset(temp_transmit_data, '\0', sizeof(temp_transmit_data));   
//                 device.name_send_func = Send_Sync_data_control;    
//             }
//             else if(device.name_send_func == Send_Sync_data_control && counters.udpate_data_counter == 10)  // запрашиваем данные
//             {
//                 temp_transmit_data[i++] = BGCP_FUNC_R; // номер функции
//                 temp_transmit_data[i++] = P_POWER;
//                 temp_transmit_data[i++] = P_SPEED_MODE;
//                 temp_transmit_data[i++] = P_BPS_MODE;
//                 temp_transmit_data[i++] = P_StateFILTER;
//                 temp_transmit_data[i++] = P_WIFI_CONN;
//                 temp_transmit_data[i++] = P_ALARM;
//                 chksum = calculate_chksum_BGCP(&temp_transmit_data[2], i-2);
//                 temp_transmit_data[i++] = (uint8_t)chksum;
//                 temp_transmit_data[i++] = (uint8_t)(chksum >> 8);
//                 err = sendto(device.sock, temp_transmit_data, i, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
//                 memset(temp_transmit_data, '\0', sizeof(temp_transmit_data));
//                 counters.udpate_data_counter = 0;
//             }
//             else if(device.name_send_func == Send_Get_IP) // Запрашиваем IP адрес приточки
//             {
//                 temp_transmit_data[i++] = BGCP_FUNC_R; // номер функции
//                 temp_transmit_data[i++] = P_CurETHR_IP;
//                 temp_transmit_data[i++] = P_CurWIFI_IP;
//                 chksum = calculate_chksum_BGCP(&temp_transmit_data[2], i-2);
//                 temp_transmit_data[i++] = (uint8_t)chksum;
//                 temp_transmit_data[i++] = (uint8_t)(chksum >> 8);
//                 err = sendto(device.sock, temp_transmit_data, i, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
//                 memset(temp_transmit_data, '\0', sizeof(temp_transmit_data));
//                 //device.name_send_func = Send_Sync_data_control;
//             }
//             else if(device.name_send_func == Send_Get_wifi_param) // запрашиваем параметры Wi-Fi и пароль приточки
//             {
//                 memset(temp_transmit_data, '\0', sizeof(temp_transmit_data));
//                 i = 0;
//                 temp_transmit_data[i++] = 0xFD;
//                 temp_transmit_data[i++] = 0xFD;
//                 temp_transmit_data[i++] = BGCP_ETH;
//                 temp_transmit_data[i++] = strlen(id_setup_mode);
//                 strcat(temp_transmit_data, id_setup_mode);
//                 i += strlen(id_setup_mode);
//                 temp_transmit_data[i++] = strlen(current_pwd);
//                 strcat(temp_transmit_data, current_pwd);
//                 i += strlen(current_pwd);
                        
//                 temp_transmit_data[i++] = BGCP_FUNC_R; // номер функции
//                 temp_transmit_data[i++] = P_DevicePWD;
//                 temp_transmit_data[i++] = P_WIFI_SSID;
//                 temp_transmit_data[i++] = P_WIFI_PWD;
//                 chksum = calculate_chksum_BGCP(&temp_transmit_data[2], i-2);
//                 temp_transmit_data[i++] = (uint8_t)chksum;
//                 temp_transmit_data[i++] = (uint8_t)(chksum >> 8);
//                 err = sendto(device.sock, temp_transmit_data, i, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
//                 memset(temp_transmit_data, '\0', sizeof(temp_transmit_data));     
//                 device.name_send_func = Send_Get_IP;    
//             }
//             else if(device.name_send_func == Send_Test_data)  // отправляем тестовые данные для проверки Wi-Fi модуля
//             {
//                 err = sendto(device.sock, "test", 4, 0, (struct sockaddr *)&test_addr, sizeof(test_addr));
//             }

//             if (err < 0) {
//                 ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
//                 break;
//             }

//             //printf("send data\n");
//             if(device.name_send_func != Send_Get_IP) device.disconnect_with_skynet++;

//             if(device.disconnect_with_skynet >= 5*10)
//                 device.button_5_state = 0;
                
//             if(device.disconnect_with_skynet >= 20*10)
//             {   
//                 printf("disconnect\n");
//                 uint8_t len1;    
//                 vTaskDelay(100 / portTICK_PERIOD_MS);  
//                 device.skynet_IP[0] = 0;
//                 device.skynet_IP[1] = 0;
//                 device.skynet_IP[2] = 0;
//                 device.skynet_IP[3] = 0;

//                 device.eth_IP[0] = 0;
//                 device.eth_IP[1] = 0;
//                 device.eth_IP[2] = 0;
//                 device.eth_IP[3] = 0;

//                 device.wifi_IP[0] = 0;
//                 device.wifi_IP[1] = 0;
//                 device.wifi_IP[2] = 0;
//                 device.wifi_IP[3] = 0;
                
//                 device.name_send_func = Send_Get_IP;
//                 compile_wifi_parameters(Vent_param_wifi.parameters, &len1);
//                 write2memory(Vent_param_wifi.parameters);
//                 device.disconnect_with_skynet = 0;
                
//                 flags.socket_reset_flag = 1;
//             }

//             if(flags.socket_reset_flag == 1)
//             {
//                 flags.socket_reset_flag = 0;
//                 vTaskDelay(100/ portTICK_PERIOD_MS);
//                 break;
//             }
//             counters.udpate_data_counter++;
//             vTaskDelay(100/ portTICK_PERIOD_MS);
//         }
//         if (device.sock != -1) 
//         {
//             ESP_LOGE(TAG, "Shutting down socket and restarting...");
//             shutdown(device.sock, 0);
//             closesocket(device.sock);
//             device.udp_sta = 0;
//             //vTaskDelete(udp_recv_task_handle);
//             //vTaskDelete(udp_send_task_handle);
//         }
//     }

//     vTaskDelete(NULL);
// }
