#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/touch_pad.h"

#include "constants.h"
#include "constants.h"
#include "myGpio.h"
#include "button.h"
#include "rotaryEncoder.h"

uint32_t last_interrupt_time = 0;

QueueHandle_t button_evt_queue  = NULL;
QueueHandle_t rotary_evnt_queue = NULL;

bool state_s1  = 1;
bool state_s2  = 1;
bool state_key = 1;

void app_main(void)
{
    button_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    rotary_evnt_queue = xQueueCreate(10, sizeof(uint32_t));
    
    gpio_setup();

    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());

    while (1) 
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}