#ifndef MYGPIO_H
#define MYGPIO_H

#include "button.h"
#include "rotaryEncoder.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "constants.h"

extern uint32_t last_interrupt_time;

extern QueueHandle_t button_evt_queue;
extern QueueHandle_t rotary_evnt_queue;

extern bool state_s1;
extern bool state_s2;
extern bool state_key;

void gpio_setup();

#endif // MYGPIO_H