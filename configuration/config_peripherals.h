/*
 * config_peripherals.h
 *
 *  Created on: Mar 8, 2021
 *      Author: nenad
 */

#ifndef CONFIG_PERIPHERALS_H_
#define CONFIG_PERIPHERALS_H_

#define ERROR_HANDLER_GPIO_PIN          GPIO_PIN_13
#define ERROR_HANDLER_GPIO_PORT         GPIOD
#define ERROR_HANDLER_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

#endif /* CONFIG_PERIPHERALS_H_ */