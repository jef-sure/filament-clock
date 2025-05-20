idf_component_register(SRCS "main.c" "spi_shiftout.c"
                    REQUIRES esp_driver_gpio esp_driver_ledc driver
                    PRIV_REQUIRES spi_flash
                    INCLUDE_DIRS ".")
