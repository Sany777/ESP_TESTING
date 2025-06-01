idf_component_register(SRC_DIRS .
                        INCLUDE_DIRS .
                        REQUIRES cmock unity 
                        PRIV_REQUIRES 
                            wifi_service
                            device_memory
                            device
                            esp_wifi
                            event_manager
                    )
