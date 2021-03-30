/*
 * config_mdrv_time_base.h
 *
 *  Created on: Mar 13, 2021
 *      Author: nenad
 */

#ifndef CONFIG_MDRV_APPLICATION_H_
#define CONFIG_MDRV_APPLICATION_H_

#define MDRV_TIME_BASE_CONFIG__TIM_IRQN             TIM1_BRK_TIM9_IRQn
#define MDRV_TIME_BASE_CONFIG__TIM_IRQ_HANDLER      TIM1_BRK_TIM9_IRQHandler
#define MDRV_TIME_BASE_CONFIG__TIM_CLK_ENABLE()     __HAL_RCC_TIM9_CLK_ENABLE()
#define MDRV_TIME_BASE_CONFIG__TIM                  TIM9

#define MCP_DATA_PIN                    GPIO_PIN_2
#define MCP_DATA_PIN_NO                 2
#define MCP_DATA_GPIO_PORT              GPIOA
#define MCP_DATA_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define MCP_DATA_TRIGGER_AF             LL_GPIO_AF_3

#endif /* CONFIG_MDRV_APPLICATION_H_ */
