#ifndef BUTTON_H
#define BUTTON_H

#include "constants.h"
#include "myGpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

void button_task(void* arg);

#endif // BUTTON_H