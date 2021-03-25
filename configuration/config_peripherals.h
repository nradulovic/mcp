/*
 * config_peripherals.h
 *
 *  Created on: Mar 8, 2021
 *      Author: nenad
 */

#ifndef CONFIG_PERIPHERALS_H_
#define CONFIG_PERIPHERALS_H_

#include "stm32f4xx_hal.h"

#define MCP_DATA_PIN                    GPIO_PIN_2
#define MCP_DATA_PIN_NO                 2
#define MCP_DATA_GPIO_PORT              GPIOA
#define MCP_DATA_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define MCP_DATA_TRIGGER_AF             LL_GPIO_AF_3

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
