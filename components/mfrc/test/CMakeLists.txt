idf_component_register(SRC_DIRS "."
                        INCLUDE_DIRS "."
                        REQUIRES cmock unity 
                        PRIV_REQUIRES driver help_tools serial_component mfrc
                    )
