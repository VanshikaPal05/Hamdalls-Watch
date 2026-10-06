#include <stdio.h>
#include <string.h>

#include "gps_subsystem.h"

#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_log.h"

#include "cJSON.h"


static const char *TAG = "GPS_SUBSYSTEM";

#define FIREBASE_GPS_URL \
    "https://heimdall-s-watch-default-rtdb.asia-southeast1.firebasedatabase.app/live_location.json"


static char response_buffer[1024];
static int response_length = 0;


static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA)
    {
        if (response_length + evt->data_len <
            sizeof(response_buffer) - 1)
        {
            memcpy(
                response_buffer + response_length,
                evt->data,
                evt->data_len
            );

            response_length += evt->data_len;
            response_buffer[response_length] = '\0';
        }
    }

    return ESP_OK;
}


esp_err_t heimdall_gps_fetch(heimdall_gps_data_t *data)
{
    if (data == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    data->valid = false;

    response_length = 0;
    memset(response_buffer, 0, sizeof(response_buffer));


    esp_http_client_config_t config = {
        .url = FIREBASE_GPS_URL,
        .event_handler = http_event_handler,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .timeout_ms = 10000,
    };


    esp_http_client_handle_t client =
        esp_http_client_init(&config);

    if (client == NULL)
    {
        ESP_LOGE(TAG, "Failed to create HTTP client");
        return ESP_FAIL;
    }


    esp_err_t err =
        esp_http_client_perform(client);


    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Firebase request failed: %s",
            esp_err_to_name(err)
        );

        esp_http_client_cleanup(client);

        return err;
    }


    int status =
        esp_http_client_get_status_code(client);


    if (status != 200)
    {
        ESP_LOGE(
            TAG,
            "Firebase HTTP status: %d",
            status
        );

        esp_http_client_cleanup(client);

        return ESP_FAIL;
    }


    cJSON *root =
        cJSON_Parse(response_buffer);


    if (root == NULL)
    {
        ESP_LOGE(TAG, "Invalid Firebase JSON");

        esp_http_client_cleanup(client);

        return ESP_FAIL;
    }


    cJSON *latitude =
        cJSON_GetObjectItem(root, "latitude");

    cJSON *longitude =
        cJSON_GetObjectItem(root, "longitude");

    cJSON *accuracy =
        cJSON_GetObjectItem(root, "accuracy");

    cJSON *altitude =
        cJSON_GetObjectItem(root, "altitude");

    cJSON *speed =
        cJSON_GetObjectItem(root, "speed");

    cJSON *timestamp =
        cJSON_GetObjectItem(root, "timestamp");


    if (!cJSON_IsNumber(latitude) ||
        !cJSON_IsNumber(longitude))
    {
        ESP_LOGE(TAG, "Latitude/longitude missing");

        cJSON_Delete(root);
        esp_http_client_cleanup(client);

        return ESP_FAIL;
    }


    data->latitude =
        latitude->valuedouble;

    data->longitude =
        longitude->valuedouble;


    data->accuracy =
        cJSON_IsNumber(accuracy)
        ? accuracy->valuedouble
        : 0;


    data->altitude =
        cJSON_IsNumber(altitude)
        ? altitude->valuedouble
        : 0;


    data->speed =
        cJSON_IsNumber(speed)
        ? speed->valuedouble
        : 0;


    data->timestamp =
        cJSON_IsNumber(timestamp)
        ? (int64_t)timestamp->valuedouble
        : 0;


    data->valid = true;


    cJSON_Delete(root);

    esp_http_client_cleanup(client);

    return ESP_OK;
}