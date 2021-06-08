/*
 * mdrv_time_base.c
 *
 *  Created on: Mar 8, 2021
 *      Author: nenad
 */

#include "mdrv_application.h"
#include "config_mdrv_application.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_gpio.h"
#include "error_handler.h"
#include "mdrv.h"

#define COMPENSATION_START 1u

static struct mdrv__context *g__mdrv_context;

void mdrv__application__init(struct mdrv__context *mdrv_context)
{
    GPIO_InitTypeDef gpio_init = {
        0};

    g__mdrv_context = mdrv_context;

    /* Initialize time base timer */
    MDRV_TIME_BASE_CONFIG__TIM_CLK_ENABLE();
    HAL_NVIC_SetPriority(MDRV_TIME_BASE_CONFIG__TIM_IRQN, 0, 0);
    HAL_NVIC_EnableIRQ(MDRV_TIME_BASE_CONFIG__TIM_IRQN);
    MDRV_TIME_BASE_CONFIG__TIM->CR1 = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->DIER = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->SR = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->EGR = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->CCMR1 = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->CCER = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->CNT = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->PSC = (SystemCoreClock / 1) / 1000000u - 1u;
    MDRV_TIME_BASE_CONFIG__TIM->ARR = 9u;
    MDRV_TIME_BASE_CONFIG__TIM->CCR1 = 0u;
    MDRV_TIME_BASE_CONFIG__TIM->OR = 0u;
    LL_TIM_GenerateEvent_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);

    /* Initialize DATA pin */
    MCP_DATA_CLK_ENABLE();
    gpio_init.Pin = MCP_DATA_PIN;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init.Alternate = 0;
    HAL_GPIO_Init(MCP_DATA_GPIO_PORT, &gpio_init);
}

void mdrv__application__tim__start(void *context, uint32_t period)
{
    (void) context;

    LL_TIM_SetAutoReload(MDRV_TIME_BASE_CONFIG__TIM, period - 1u);
    LL_TIM_GenerateEvent_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
    LL_TIM_EnableIT_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
    LL_TIM_SetCounter(MDRV_TIME_BASE_CONFIG__TIM, COMPENSATION_START);
    LL_TIM_EnableCounter(MDRV_TIME_BASE_CONFIG__TIM);
}

void mdrv__application__tim__start_on_trigger(void *context, uint32_t period)
{
    (void) context;
    TIM_HandleTypeDef tim;
    tim.Instance = MDRV_TIME_BASE_CONFIG__TIM;
    tim.Init.Period = period - 1;
    tim.Init.Prescaler = (SystemCoreClock / 1) / 1000000u - 1u;
    tim.Init.ClockDivision = 0;
    tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Stop_IT(&tim) != HAL_OK) {
        error_handler__handle();
    }
    if (HAL_TIM_Base_Init(&tim) != HAL_OK) {
        error_handler__handle();
    }

    TIM_SlaveConfigTypeDef tim_slave = {
        .SlaveMode = TIM_SLAVEMODE_TRIGGER,
        .InputTrigger = TIM_TS_TI1FP1,
        .TriggerPolarity = TIM_TRIGGERPOLARITY_FALLING,
        .TriggerPrescaler = TIM_ETRPRESCALER_DIV1,
        .TriggerFilter = 0};

    if (HAL_TIM_SlaveConfigSynchro(&tim, &tim_slave) != HAL_OK) {
        error_handler__handle();
    }
    if (HAL_TIM_Base_Start_IT(&tim) != HAL_OK) {
        error_handler__handle();
    }
}

void mdrv__application__tim__stop(void *context)
{
    (void) context;
    LL_TIM_SetSlaveMode(MDRV_TIME_BASE_CONFIG__TIM, LL_TIM_SLAVEMODE_DISABLED);
    LL_TIM_DisableCounter(MDRV_TIME_BASE_CONFIG__TIM);
    LL_TIM_DisableIT_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
    LL_TIM_GenerateEvent_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
    LL_TIM_ClearFlag_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
}

void mdrv__application__tim__isr(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(MDRV_TIME_BASE_CONFIG__TIM)
        && LL_TIM_IsEnabledIT_UPDATE(MDRV_TIME_BASE_CONFIG__TIM)) {
        mdrv__it(g__mdrv_context);
        LL_TIM_ClearFlag_UPDATE(MDRV_TIME_BASE_CONFIG__TIM);
    }
}

void mdrv__application__pin__init_output(void *context, bool value)
{
    mdrv__application__pin__write(context, value);
    LL_GPIO_SetPinMode(MCP_DATA_GPIO_PORT, MCP_DATA_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(MCP_DATA_GPIO_PORT, MCP_DATA_PIN, LL_GPIO_PULL_NO);
}

void mdrv__application__pin__init_input(void *context)
{
    (void) context;
    LL_GPIO_SetPinPull(MCP_DATA_GPIO_PORT, MCP_DATA_PIN, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinMode(MCP_DATA_GPIO_PORT, MCP_DATA_PIN, LL_GPIO_MODE_INPUT);
    /* Do not pull the input down because that will interfere with edge detection during input */
}

void mdrv__application__pin__init_trigger(void *context)
{
    (void) context;

    GPIO_InitTypeDef input_pin = {
        .Pin = GPIO_PIN_2,
        .Mode = GPIO_MODE_AF_PP,
        .Alternate = GPIO_AF3_TIM9,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FAST};

    HAL_GPIO_Init(GPIOA, &input_pin);
}

void mdrv__application__pin__write(void *context, bool state)
{
    (void) context;
    if (state) {
        LL_GPIO_SetOutputPin(MCP_DATA_GPIO_PORT, MCP_DATA_PIN);
    } else {
        LL_GPIO_ResetOutputPin(MCP_DATA_GPIO_PORT, MCP_DATA_PIN);
    }
}

bool mdrv__application__pin__read(void *context)
{
    (void) context;
    return LL_GPIO_ReadInputPort(MCP_DATA_GPIO_PORT) & MCP_DATA_PIN;
}
