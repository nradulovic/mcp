/*
 * usbd_cdc_terminal.h
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#ifndef USBD_CDC_TERMINAL_H_
#define USBD_CDC_TERMINAL_H_

struct mdrv__context;

void usbd_cdc_terminal__init(void);
void usbd_cdc_terminal__set_terminal_context(void * terminal_context);
void usbd_cdc_terminal__loop(void);

#endif /* USBD_CDC_TERMINAL_H_ */
