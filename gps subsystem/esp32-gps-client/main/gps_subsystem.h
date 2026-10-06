#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

typedef struct
{
    double latitude;
    double longitude;
    double accuracy;
    double altitude;
    double speed;

    int64_t timestamp;

    bool valid;
} heimdall_gps_data_t;


/*
 * Downloads the latest phone GPS position from Firebase.
 *
 * ESP_OK  = new data received successfully
 * other   = request/JSON error
 */
esp_err_t heimdall_gps_fetch(heimdall_gps_data_t *data);