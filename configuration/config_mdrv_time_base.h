/*
 * config_mdrv_time_base.h
 *
 *  Created on: Mar 13, 2021
 *      Author: nenad
 */

#ifndef CONFIG_MDRV_TIME_BASE_H_
#define CONFIG_MDRV_TIME_BASE_H_

#define MDRV_TIME_BASE_CONFIG__TIM_IRQN             TIM1_BRK_TIM9_IRQn
#define MDRV_TIME_BASE_CONFIG__TIM_IRQ_HANDLER      TIM1_BRK_TIM9_IRQHandler
#define MDRV_TIME_BASE_CONFIG__TIM_CLK_ENABLE()     __HAL_RCC_TIM9_CLK_ENABLE()
#define MDRV_TIME_BASE_CONFIG__TIM                  TIM9


#endif /* CONFIG_MDRV_TIME_BASE_H_ */
