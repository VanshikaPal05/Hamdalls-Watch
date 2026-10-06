#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "protocol_examples_common.h"

#include "gps_subsystem.h"


static const char *TAG = "HEIMDALL_MAIN";


void app_main(void)
{
    esp_err_t ret =
        nvs_flash_init();


    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(
            nvs_flash_erase()
        );

        ESP_ERROR_CHECK(
            nvs_flash_init()
        );
    }


    ESP_ERROR_CHECK(
        esp_netif_init()
    );


    ESP_ERROR_CHECK(
        esp_event_loop_create_default()
    );


    ESP_LOGI(
        TAG,
        "Connecting to Wi-Fi..."
    );


    ESP_ERROR_CHECK(
        example_connect()
    );


    ESP_LOGI(
        TAG,
        "Wi-Fi connected"
    );


    heimdall_gps_data_t gps;


    while (1)
    {
        if (heimdall_gps_fetch(&gps) == ESP_OK &&
            gps.valid)
        {
            printf("\n");
            printf("===== HEIMDALL GPS =====\n");

            printf(
                "Latitude : %.6f\n",
                gps.latitude
            );

            printf(
                "Longitude: %.6f\n",
                gps.longitude
            );

            printf(
                "Accuracy : %.1f m\n",
                gps.accuracy
            );

            printf(
                "Altitude : %.1f m\n",
                gps.altitude
            );

            printf(
                "Speed    : %.2f m/s\n",
                gps.speed
            );

            printf("========================\n\n");
        }
        else
        {
            ESP_LOGW(
                TAG,
                "Could not obtain latest GPS position"
            );
        }


        vTaskDelay(
            pdMS_TO_TICKS(5000)
        );
    }
}