#include "rotaryEncoder.h"

static void state_array_add(int memory[4][2], int state_new, int pin_index) 
{
    for (int i = 0; i < 3; i++)
    {
        memory[i][0] = memory[i + 1][0];
        memory[i][1] = memory[i + 1][1];
    }

    memory[3][pin_index] = state_new;
}

static int state_array_equal(int arr1[4][2], int arr2[4][2]) 
{
    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 2; j++) 
        {
            if (arr1[i][j] != arr2[i][j]) 
            {
                return 0;
            }
        }
    }

    return 1;
}

static int check_pattern(int memory[4][2])
{
    int clockwise[4][2]      = {{0, 1}, {0, 0}, {1, 0}, {1, 1}};
    int anti_clockwise[4][2] = {{1, 0}, {0, 0}, {0, 1}, {1, 1}};
    
    if (state_array_equal(memory, clockwise)) 
        return 1;

    if (state_array_equal(memory, anti_clockwise)) 
        return -1;

    return 0;
}

void state_array_print(int arr[4][2])
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


void rotary_task(void* arg)
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