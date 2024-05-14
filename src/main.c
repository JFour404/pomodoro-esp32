#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "driver/gpio.h"

#define LED_BUILD_IN 2

void app_main(void) 
{
    char *taskName = pcTaskGetName(NULL);
    ESP_LOGI(taskName, "Hello, staring up!\n");

    gpio_reset_pin(LED_BUILD_IN);
    gpio_set_direction(LED_BUILD_IN, GPIO_MODE_OUTPUT);

    while (1)
    {
        gpio_set_level(LED_BUILD_IN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED_BUILD_IN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}