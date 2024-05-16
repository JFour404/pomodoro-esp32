#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/touch_pad.h"

// OUTPUTS
#define LED_BUILDIN 2

// INPUTS
#define BUTTON_0    22
#define BUTTON_1    23
#define ROTARY_ENCODER_S1  27
#define ROTARY_ENCODER_S2  26
#define ROTARY_ENCODER_KEY 25

#define GPIO_INPUT_PIN_SEL_BUTTON  (\
    (1ULL << BUTTON_0) | \
    (1ULL << BUTTON_1)   \
)

#define GPIO_INPUT_PIN_SEL_ROTARY_ENCODER (\
    (1ULL << ROTARY_ENCODER_S1) | \
    (1ULL << ROTARY_ENCODER_S2) | \
    (1ULL << ROTARY_ENCODER_KEY)  \
)

#define ESP_INTR_FLAG_DEFAULT 0
#define DEBOUNCE_TIME 50

static uint32_t last_interrupt_time = 0; 

static QueueHandle_t gpio_evt_queue    = NULL;
static QueueHandle_t rotary_evnt_queue = NULL;

static bool state_s1  = 1;
static bool state_s2  = 1;
static bool state_key = 1;

static void state_array_add(int memory[4][2], int state_new, int pin_index) 
{
    for (int i = 0; i < 3; i++)
    {
        memory[i][0] = memory[i + 1][0];
        memory[i][1] = memory[i + 1][1];
    }

    memory[3][pin_index] = state_new;
}

void printArray(int arr[4][2]) 
{
    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 2; j++) 
        {
            printf("%d ", arr[i][j]);
        }
        printf("| ");
    }
    printf("\n");
}

bool areEqual(int arr1[4][2], int arr2[4][2]) 
{
    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 2; j++) 
        {
            if (arr1[i][j] != arr2[i][j]) 
            {
                return false;
            }
        }
    }
    return true;
}

static int check_pattern(int memory[4][2])
{
    int clockwise[4][2]      = {{0, 1}, {0, 0}, {1, 0}, {1, 1}};
    int anti_clockwise[4][2] = {{1, 0}, {0, 0}, {0, 1}, {1, 1}};
    
    if (areEqual(memory, clockwise)) 
    {
        return 1;
    } 
    else if (areEqual(memory, anti_clockwise)) 
    {
        return -1;
    } 
    else 
    {
        return 0;
    }
}

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    uint32_t now = xTaskGetTickCount();

    switch (gpio_num)
    {
        case BUTTON_0:
        case BUTTON_1:
        case ROTARY_ENCODER_KEY:
            if (now - last_interrupt_time < DEBOUNCE_TIME)
                return;
            else
                last_interrupt_time = now;
            xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
            break;
        case ROTARY_ENCODER_S1:
            if (state_s1 == gpio_get_level(ROTARY_ENCODER_S1))
                return;
            else
                state_s1 = gpio_get_level(ROTARY_ENCODER_S1);
            xQueueSendFromISR(rotary_evnt_queue, &gpio_num, NULL);
            break;
        case ROTARY_ENCODER_S2:
            if (state_s2 == gpio_get_level(ROTARY_ENCODER_S2))
                return;
            else
                state_s2 = gpio_get_level(ROTARY_ENCODER_S2);
            xQueueSendFromISR(rotary_evnt_queue, &gpio_num, NULL);
            break;
        default:
            break;
    }
}

static void button_task(void* arg)
{
    uint32_t io_num;

    while (1) 
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) 
        {
            printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
            
            gpio_set_level(LED_BUILDIN, 1);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(LED_BUILDIN, 0);
        }
    }
}

static void rotary_task(void* arg)
{
    uint32_t io_num;
    static int last_values[28] = {0};
    static int memory_rotary[4][2] = {0};
    static int position_rotary = 0;

    while (1) 
    {
        if (xQueueReceive(rotary_evnt_queue, &io_num, portMAX_DELAY)) 
        {
            int current_value = gpio_get_level(io_num);

            if (current_value != last_values[io_num]) 
            {
                switch (io_num)
                {
                case ROTARY_ENCODER_S1:
                    state_array_add(memory_rotary, current_value, 0);
                    break;
                case ROTARY_ENCODER_S2:
                    state_array_add(memory_rotary, current_value, 1);
                    break;
                default:
                    break;
                }
                
                int position_change = check_pattern(memory_rotary);

                if (position_change != 0)
                {
                    position_rotary += check_pattern(memory_rotary);
                    printf("Encoder value: %d\n", position_rotary);

                    gpio_set_level(LED_BUILDIN, 1);
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                    gpio_set_level(LED_BUILDIN, 0);
                }
                
                last_values[io_num] = current_value;     
            }
        }
    }
}

static void gpio_setup()
{
    gpio_config_t io_conf = {};
    
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL_BUTTON;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en   = 0;
    io_conf.pull_down_en = 1;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL_ROTARY_ENCODER;
    io_conf.pull_up_en   = 0;
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);    

    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);
    xTaskCreate(rotary_task, "rotary_task", 2048, NULL, 10, NULL);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(BUTTON_0, gpio_isr_handler, (void*) BUTTON_0);
    gpio_isr_handler_add(BUTTON_1, gpio_isr_handler, (void*) BUTTON_1);
    gpio_isr_handler_add(ROTARY_ENCODER_S1, gpio_isr_handler, (void*) ROTARY_ENCODER_S1);
    gpio_isr_handler_add(ROTARY_ENCODER_S2, gpio_isr_handler, (void*) ROTARY_ENCODER_S2);
    gpio_isr_handler_add(ROTARY_ENCODER_KEY, gpio_isr_handler, (void*) ROTARY_ENCODER_KEY);


    gpio_set_direction(LED_BUILDIN, GPIO_MODE_OUTPUT);
}

void app_main(void)
{
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    rotary_evnt_queue = xQueueCreate(10, sizeof(uint32_t));
    
    gpio_setup();

    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());

    while (1) 
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}