#include <string.h>

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

#include "dht.h"
#include "mqtt.h"
#include "handlers.h"
#include "commons.h"


void send_temperature(void * params) {
    const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;
    char temperature_topic_name[65];
    char humidity_topic_name[65];

    int16_t temperature, humidity;


    sprintf(temperature_topic_name, "fse2020/%s/%s/temperatura", MATRICULA, roomName);
    sprintf(humidity_topic_name, "fse2020/%s/%s/umidade", MATRICULA, roomName);


    while(true) {
        dht_read_data(DHT_TYPE_DHT11, 4, &humidity, &temperature);

        char temperature_json[65];
        char humidity_json[65];

        sprintf(temperature_json, "{\"temperatura\": \"%d\"}", temperature);
        sprintf(humidity_json, "{\"umidade\": \"%d\"}", humidity);

        publish(temperature_topic_name, temperature_json);
        publish(humidity_topic_name, humidity_json);
        
        printf("Temperatura:: %i, humidade:: %i \n", temperature, humidity);
        vTaskDelay( xDelay );
    }
}