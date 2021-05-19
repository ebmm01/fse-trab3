#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "button.h"
#include "mqtt.h"
#include "cJSON.h"
#include "handlers.h"
#include "commons.h"

#define BOTAO_1 0

xQueueHandle filaDeInterrupcao;

int button_state = 0;

static void IRAM_ATTR gpio_isr_handler(void *args)
{
  int pino = (int)args;
  xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
}

void trataInterrupcaoBotao(void *params)
{
    int pino;
    char state_topic_name[65];

    while(true)
    {
        if(xQueueReceive(filaDeInterrupcao, &pino, portMAX_DELAY))
        {
        // De-bouncing
            int estado = gpio_get_level(pino);
            if(estado == 1)
            {
                sprintf(state_topic_name, "fse2020/%s/%s/estado", MATRICULA, roomName);
                gpio_isr_handler_remove(pino);
                while(gpio_get_level(pino) == estado) {
                    vTaskDelay(50 / portTICK_PERIOD_MS);
                }

                button_state = 1 - button_state;

                char state_json[65];

                sprintf(state_json, "{\"entradaState\": \"%d\"}", button_state);
                publish(state_topic_name, state_json);

                // Habilitar novamente a interrupção
                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_isr_handler_add(pino, gpio_isr_handler, (void *) pino);
            }

        }
    }
}


void init_button() {
    // Configuração do pino do Botão
    gpio_pad_select_gpio(BOTAO_1);

    // Configura o pino do Botão como Entrada
    gpio_set_direction(BOTAO_1, GPIO_MODE_INPUT);

    // Configura o resistor de Pulldown para o botão (por padrão a entrada estará em Zero)
    gpio_pulldown_en(BOTAO_1);

    // Desabilita o resistor de Pull-up por segurança.
    gpio_pullup_dis(BOTAO_1);

    // Configura pino para interrupção
    gpio_set_intr_type(BOTAO_1, GPIO_INTR_POSEDGE);

    filaDeInterrupcao = xQueueCreate(10, sizeof(int));
    xTaskCreate(trataInterrupcaoBotao, "TrataBotao", 2048, NULL, 1, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BOTAO_1, gpio_isr_handler, (void *) BOTAO_1);
}