/*
 * config_mdrv_time_base.h
 *
 *  Created on: Mar 13, 2021
 *      Author: nenad
 */

#ifndef CONFIG_MDRV_TIME_BASE_H_
#define CONFIG_MDRV_TIME_BASE_H_

#define MDRV_TIME_BASE_CONFIG__TIM_IRQN     TIM5_IRQn
#define MDRV_TIME_BASE_CONFIG__TIM_CLK_ENABLE() __HAL_RCC_TIM5_CLK_ENABLE()
#define MDRV_TIME_BASE_CONFIG__TIM          TIM5


#endif /* CONFIG_MDRV_TIME_BASE_H_ */
