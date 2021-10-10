
/*
 * Copyright (C) IngBiro - Jovan Jovanovic
 * 
 * +381 63 477858 Jovan Jovanovic (info@ingbiro.net)
 * Vizantijski Bulevar 98/12
 */
/*
 * usbd_cdc_terminal.h
 *
 *  Created on: Mar 1, 2021
 */

#ifndef USBD_CDC_TERMINAL_H_
#define USBD_CDC_TERMINAL_H_

struct mdrv__context;

struct usbd_cdc_terminal__context
{
    void * mdrv;
    void * config;
};

void usbd_cdc_terminal__init(void);
void usbd_cdc_terminal__set_terminal_context(struct usbd_cdc_terminal__context * terminal_context);
void usbd_cdc_terminal__loop(void);

#endif /* USBD_CDC_TERMINAL_H_ */
