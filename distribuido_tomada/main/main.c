#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "dht.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "wifi.h"

#define LED_1 2

xSemaphoreHandle conexaoWifiSemaphore;

void app_main()
{
  
    // Configuração do Timer
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_config);

    // Configuração do Canal
    ledc_channel_config_t channel_config = {
        .gpio_num = LED_1,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_config);
    
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
    int16_t humidity, temperature;
    
    ledc_fade_func_install(0);

    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    conexaoWifiSemaphore = xSemaphoreCreateBinary();

    
    wifi_start();

    while(true)
    {
        dht_read_data(DHT_TYPE_DHT11, 4, &humidity, &temperature);
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 1000, LEDC_FADE_WAIT_DONE);
        ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 255, 1000, LEDC_FADE_WAIT_DONE);
        vTaskDelay( xDelay );
        printf("Temperatura:: %i, humidade:: %i \n", temperature, humidity);
    }
}
