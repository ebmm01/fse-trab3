
#include <string.h>
#include <stdio.h>

#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "handlers.h"
#include "temperatures.h"
#include "mqtt.h"
#include "cJSON.h"

char roomName[30];

void apply_handler(char* msg) {
    cJSON* json = NULL;
    cJSON* event_json = NULL;

    json = cJSON_Parse(msg);

    event_json = cJSON_GetObjectItem(json, "event");

    printf("EVENT::: %s\n", event_json->valuestring);

    if (strcmp(event_json->valuestring, ADDED_DEVICE) == 0) {
        cJSON* room_json = NULL;

        room_json = cJSON_GetObjectItem(json, "room");

        strncpy(roomName, room_json->valuestring, strlen(room_json->valuestring));

        xTaskCreate(&send_temperature,  "Handler de envio de temperatura", 4096, NULL, 1, NULL);
    } 
}