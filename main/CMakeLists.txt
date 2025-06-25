idf_component_register(SRCS "http_server.c" "http_client_request.c" "auto_location.c" "str.c" "nvs_config.c" "provisioning.c" "font3x5.c" "clock_hands.c" "weather.c" "font3x7.c" "led_matrix.c" "font2x7.c" "main.c" "spi_shiftout.c" "tzones.c"
                    REQUIRES esp_driver_gpio esp_driver_ledc driver nvs_flash protocomm wifi_provisioning esp_event mbedtls mjson esp_http_client esp_http_server
                    PRIV_REQUIRES spi_flash
                    INCLUDE_DIRS ".")
