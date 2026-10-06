idf_component_register(
    SRCS
        "main.c"
        "gps_subsystem.c"

    INCLUDE_DIRS
        "."

    PRIV_REQUIRES
        esp_http_client
        json
        mbedtls
        esp_event
        esp_netif
        nvs_flash
        protocol_examples_common
)