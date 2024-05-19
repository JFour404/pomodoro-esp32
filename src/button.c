#include "button.h"

void button_task(void* arg)
{
    uint32_t io_num;

    while (1) 
    {
        if (xQueueReceive(button_evt_queue, &io_num, portMAX_DELAY)) 
        {
            printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
            
            gpio_set_level(LED_BUILDIN, 1);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(LED_BUILDIN, 0);
        }
    }
}