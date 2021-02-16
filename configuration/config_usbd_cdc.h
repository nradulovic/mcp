/*
 * config_usbd_cdc.h
 *
 *  Created on: Feb 15, 2021
 *      Author: nenad
 */

#ifndef CONFIG_USBD_CDC_H_
#define CONFIG_USBD_CDC_H_

/* Define size for the receive and transmit buffer over CDC */
#define CONFIG_USBD_CDC__RX_DATA_SIZE  2048
#define CONFIG_USBD_CDC__TX_DATA_SIZE  2048
#define CONFIG_USBD_CDC__VID     1155
#define CONFIG_USBD_CDC__LANGID_STRING     1033
#define CONFIG_USBD_CDC__MANUFACTURER_STRING     "Senis AG"
#define CONFIG_USBD_CDC__PID_FS     22336
#define CONFIG_USBD_CDC__PRODUCT_STRING_FS     "anyCS proxy"
#define CONFIG_USBD_CDC__CONFIGURATION_STRING_FS     "CDC Config"
#define CONFIG_USBD_CDC__INTERFACE_STRING_FS     "CDC Interface"

#endif /* CONFIG_USBD_CDC_H_ */
