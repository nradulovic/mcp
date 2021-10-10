/*
 * config_peripherals.h
 *
 *  Created on: Mar 8, 2021
 */

#ifndef CONFIG_PERIPHERALS_H_
#define CONFIG_PERIPHERALS_H_

#include "stm32f4xx_hal.h"

#define MDRV_TIME_BASE_CONFIG__TIM_IRQN             TIM1_BRK_TIM9_IRQn
#define MDRV_TIME_BASE_CONFIG__TIM_IRQ_HANDLER      TIM1_BRK_TIM9_IRQHandler
#define MDRV_TIME_BASE_CONFIG__TIM_CLK_ENABLE()     __HAL_RCC_TIM9_CLK_ENABLE()
#define MDRV_TIME_BASE_CONFIG__TIM                  TIM9


#define MCP_DATA_PIN                    GPIO_PIN_2
#define MCP_DATA_GPIO_PORT              GPIOA
#define MCP_DATA_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define MCP_DATA_TRIGGER_AF             LL_GPIO_AF_3

/*
 * Poll timer is used in MDRV driver, therefore it must have higher priority then MDRV.
 */
#define POLL_TIMER__IRQ_PRIO            0
#define MDRV_TIME_BASE_CONFIG__TIM_IRQ_PRIO         1
#define USB_IRQ_PRIO                    2

#define MCP_STATUS_PIN                  GPIO_PIN_12
#define MCP_STATUS_GPIO_PORT            GPIOD
#define MCP_STATUS_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()

#define MCP_ERROR_PIN                   GPIO_PIN_13
#define MCP_ERROR_GPIO_PORT             GPIOD
#define MCP_ERROR_CLK_ENABLE()          __HAL_RCC_GPIOD_CLK_ENABLE()

#define ERROR_HANDLER_GPIO_PIN          GPIO_PIN_13
#define ERROR_HANDLER_GPIO_PORT         GPIOD
#define ERROR_HANDLER_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

#endif /* CONFIG_PERIPHERALS_H_ */
