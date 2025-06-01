idf_component_register(
                    SRC_DIRS .
                    INCLUDE_DIRS "include"
                    EMBED_FILES embedded_files/rsa_private.pem
                    PRIV_REQUIRES 
                        device 
                        esp_http_server 
                        event_manager
                        freertos 
                        app_update
                        mbedtls
                        driver
                )