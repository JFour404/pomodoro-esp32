#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include "constants.h"
#include "myGpio.h"

void state_array_print(int arr[4][2]);
void rotary_task(void* arg);

#endif // ROTARY_ENCODER_H