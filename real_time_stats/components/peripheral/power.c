/**
 * @file power.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "driver/gpio.h"

#define POW_GPIO 4

void pow_init(void)
{
    gpio_set_direction(POW_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(POW_GPIO, 1);
}

