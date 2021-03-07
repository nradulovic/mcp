/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "usbd_cdc_terminal.h"
#include "mdrv.h"

TIM_HandleTypeDef g__mdrv__time_base;

static void system_clock_init(void);
static void gpio_init(void);
static void pin_init_output(void *context, bool value);
static void pin_init_input(void *context);
static void pin_write(void *context, bool value);
static bool pin_read(void *context);
static void mdrv__time_base__start(void *context, mdrv__time_us_t period);
static void mdrv__time_base__stop(void *context);

static const struct mdrv__ll mdrv__ll = {
    .pin_init_input = pin_init_input,
    .pin_init_output = pin_init_output,
    .pin_write = pin_write,
    .pin_read = pin_read,
    .tim_start = mdrv__time_base__start,
    .tim_stop = mdrv__time_base__stop, };

static struct mdrv__context mdrv__context;

int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    system_clock_init();
    SystemCoreClockUpdate();

    /* Initialize all configured peripherals */
    gpio_init();
    mdrv__init(&mdrv__context, &mdrv__ll, NULL);
    usbd_cdc_terminal__init();
    usbd_cdc_terminal__set_terminal_context(&mdrv__context);

    /* Infinite loop */
    while (1) {
        usbd_cdc_terminal__loop();
    }
}

void system_clock_init(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {
        0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {
        0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /**
     * Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1
        | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
        Error_Handler();
    }
}

static void gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {
        0};

    /* Initialize DATA pin */
    MCP_DATA_CLK_ENABLE();
    gpio_init_struct.Pin = MCP_DATA_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_struct.Alternate = 0;
    HAL_GPIO_Init(MCP_DATA_GPIO_PORT, &gpio_init_struct);

    /* Initialize STATUS pin */
    MCP_STATUS_CLK_ENABLE();
    gpio_init_struct.Pin = MCP_STATUS_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init_struct.Alternate = 0;
    HAL_GPIO_Init(MCP_STATUS_GPIO_PORT, &gpio_init_struct);

    /* Initialize ERROR pin */
    MCP_ERROR_CLK_ENABLE();
    gpio_init_struct.Pin = MCP_ERROR_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init_struct.Alternate = 0;
    HAL_GPIO_Init(MCP_ERROR_GPIO_PORT, &gpio_init_struct);
}

static void pin_init_output(void *context, bool value)
{
    (void) context;
    HAL_GPIO_WritePin(MCP_DATA_GPIO_PORT, MCP_DATA_PIN, value ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_ChangeMode(MCP_DATA_GPIO_PORT, MCP_DATA_PIN_NO, GPIO__MODE__OUTPUT_PP);
}

static void pin_init_input(void *context)
{
    (void) context;
    HAL_GPIO_ChangeMode(MCP_DATA_GPIO_PORT, MCP_DATA_PIN_NO, GPIO__MODE__INPUT);
}

static void pin_write(void *context, bool state)
{
    (void) context;
    HAL_GPIO_WritePin(MCP_DATA_GPIO_PORT, MCP_DATA_PIN, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static bool pin_read(void *context)
{
    (void) context;
    GPIO_PinState state = HAL_GPIO_ReadPin(MCP_DATA_GPIO_PORT, MCP_DATA_PIN);
    return state == GPIO_PIN_SET ? true : false;
}

static void mdrv__time_base__start(void *context, mdrv__time_us_t period)
{
    (void) context;

    g__mdrv__time_base.Instance = TIM10;
    g__mdrv__time_base.Init.Prescaler = (uint32_t) ((SystemCoreClock / 1000000) - 1);
    g__mdrv__time_base.Init.CounterMode = TIM_COUNTERMODE_UP;
    g__mdrv__time_base.Init.Period = period - 1u;
    g__mdrv__time_base.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g__mdrv__time_base.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&g__mdrv__time_base) != HAL_OK) {
        Error_Handler();
    }
    HAL_TIM_Base_Start_IT(&g__mdrv__time_base);
}

static void mdrv__time_base__stop(void *context)
{
    (void) context;
    HAL_TIM_Base_Stop_IT(&g__mdrv__time_base);
    HAL_TIM_Base_DeInit(&g__mdrv__time_base);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM10) {
        mdrv__it(&mdrv__context);
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) {
    }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
