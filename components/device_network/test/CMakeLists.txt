idf_component_register(SRC_DIRS .
                        INCLUDE_DIRS .
                        REQUIRES cmock unity 
                        PRIV_REQUIRES 
                            device_memory
                            device_network
                            device 
                            esp_http_server 
                            wifi_service
                            event_manager
                            lwip 
                            freertos 
                            esp_netif
                            app_update
                            json
                            device_ota
                    )
