
/*
 * Copyright (C) IngBiro - Jovan Jovanovic
 * 
 * +381 63 477858 Jovan Jovanovic (info@ingbiro.net)
 * Vizantijski Bulevar 98/12
 */
#include "stm32f4xx_hal.h"

void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}
