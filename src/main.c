#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/touch_pad.h"

#define BUTTON_0                22
#define BUTTON_1                23
#define GPIO_INPUT_PIN_SEL  ((1ULL<<BUTTON_0) | (1ULL<<BUTTON_1))

#define ESP_INTR_FLAG_DEFAULT   0

#define DEBOUNCE_TIME           50
static uint32_t last_interrupt_time = 0; 

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    uint32_t now = xTaskGetTickCount();

    if (now - last_interrupt_time < DEBOUNCE_TIME)
    {
        return;
    }
    last_interrupt_time = now;
    
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void button_task(void* arg)
{
    uint32_t io_num;
    while (1) 
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) 
        {
            printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

void app_main(void)
{
    gpio_config_t io_conf = {};
    
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(BUTTON_0, gpio_isr_handler, (void*) BUTTON_0);
    gpio_isr_handler_add(BUTTON_1, gpio_isr_handler, (void*) BUTTON_1);

    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());

    while (1) 
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}