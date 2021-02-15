#ifndef APP_USBD_CDC_H_
#define APP_USBD_CDC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** USB Device initialization function. */
void app_usbd_cdc__init(void);
uint8_t app_usbd_cdc__transmit(uint8_t* Buf, uint16_t Len);

#ifdef __cplusplus
}
#endif

#endif /* APP_USBD_CDC_H_ */
