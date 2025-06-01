idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES    
                        freertos
                        device
                        device_memory
                        device_gpio
                        event_manager
                        app_update
                    )
