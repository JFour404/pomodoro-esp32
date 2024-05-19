#ifndef CONSTANTS_H
#define CONSTANTS_H

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

#endif // CONSTANTS_H