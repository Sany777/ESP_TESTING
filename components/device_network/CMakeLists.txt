idf_component_register(
                    SRC_DIRS .
                    INCLUDE_DIRS "include"
                    EMBED_FILES
                     embedded_files/index.html  
                     embedded_files/script.min.js
                     embedded_files/style.min.css 
                     embedded_files/favicon.ico
                    PRIV_REQUIRES 
                        device_ota
                        json
                        device 
                        esp_http_server 
                        wifi_service
                        event_manager
                        lwip 
                        freertos 
                        esp_netif
                        app_update
                )