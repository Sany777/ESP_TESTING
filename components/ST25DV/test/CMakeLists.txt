idf_component_register(SRC_DIRS .
                        INCLUDE_DIRS .
                        REQUIRES cmock unity 
                        PRIV_REQUIRES 
                            freertos
                            driver
                            json
                            ST25DV
                            device
                            device_i2c
                    )
