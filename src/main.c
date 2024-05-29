#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/touch_pad.h"

#include "constants.h"
#include "constants.h"
#include "myGpio.h"
#include "button.h"
#include "rotaryEncoder.h"
#include "i2cLcd.h"

static const char *TAG = "pomodoro-main";

uint32_t last_interrupt_time = 0;

QueueHandle_t button_evt_queue  = NULL;
QueueHandle_t rotary_evnt_queue = NULL;

bool state_s1  = 1;
bool state_s2  = 1;
bool state_key = 1;

char buffer_lcd[16];

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_NUM_0;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

void app_main(void)
{
    button_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    rotary_evnt_queue = xQueueCreate(10, sizeof(uint32_t));
    
    gpio_setup();

    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());

    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    lcd_init();
    lcd_clear();

    sprintf(buffer_lcd, "Hello, world!");
    lcd_put_cur(0, 0);
    lcd_send_string(buffer_lcd);

    while (1) 
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}