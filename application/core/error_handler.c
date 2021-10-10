
/*
 * Copyright (C) IngBiro - Jovan Jovanovic
 * 
 * +381 63 477858 Jovan Jovanovic (info@ingbiro.net)
 * Vizantijski Bulevar 98/12
 */
/*
 * error_handler.c
 *
 *  Created on: Mar 8, 2021
 */

#include "error_handler.h"
#include "config_peripherals.h"

#include "stm32f4xx_ll_gpio.h"

void error_handler__init(void)
{
    LL_GPIO_InitTypeDef gpio_init = {
        .Mode = LL_GPIO_MODE_OUTPUT,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pin = ERROR_HANDLER_GPIO_PIN,
        .Speed = LL_GPIO_SPEED_FREQ_LOW};
    ERROR_HANDLER_GPIO_CLK_ENABLE();
    LL_GPIO_Init(ERROR_HANDLER_GPIO_PORT, &gpio_init);
}

void error_handler__handle(void)
{
    __disable_irq();
    LL_GPIO_SetOutputPin(ERROR_HANDLER_GPIO_PORT, ERROR_HANDLER_GPIO_PIN);
    while (1) {
    }
}
