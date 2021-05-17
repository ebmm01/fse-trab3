#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "esp_log.h"
#include "esp_system.h"

#include "mqtt.h"
#include "commons.h"
#include "cJSON.h"
#include "handlers.h"

#define ESP_MQTT_BROKER      CONFIG_ESP_MQTT_BROKER
#define TAG "MQTT"

extern xSemaphoreHandle conexaoMQTTSemaphore;
esp_mqtt_client_handle_t client;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client = event->client;
    char topic[100];
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xSemaphoreGive(conexaoMQTTSemaphore);
            
            uint8_t mac[6] = {0};
            char mac_address[14];

            esp_efuse_mac_get_default(mac);
            sprintf(mac_address, "%x%x%x%x%x%x",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            
            sprintf(topic, "fse2020/%s/dispositivos/%s", MATRICULA, mac_address);

            cJSON* id_to_send = NULL;

            /* Create a JSON data object (linked list node) */
            id_to_send = cJSON_CreateObject();

            /* Add a string of JSON data (add a linked list node) */
            cJSON_AddStringToObject(id_to_send, "id", mac_address);

            printf("TOPIC=%s\n", topic);

            esp_mqtt_client_publish(client, topic, cJSON_Print(id_to_send), 0,0,0);
            esp_mqtt_client_subscribe(client, topic, 0);
            
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            apply_handler(event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}


void publish(char* topic, char* content) {
    esp_mqtt_client_publish(client, topic, content, 0,0,0);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}


void mqtt_start() {
    esp_mqtt_client_config_t mqtt_config = {
        .uri = ESP_MQTT_BROKER
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

}