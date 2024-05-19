#include "myGpio.h"

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
            xQueueSendFromISR(button_evt_queue, &gpio_num, NULL);
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

void gpio_setup()
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
