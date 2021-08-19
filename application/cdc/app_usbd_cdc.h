#ifndef APP_USBD_CDC_H_
#define APP_USBD_CDC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct app_usbd_cdc__context
{
    void (*receive)(void * arg, const void * buffer, uint16_t length);
    void (*complete)(void * arg);
    void * arg;
};

enum app_usbd_cdc__error
{
    APP_USBD_CDC__ERROR__OK,
    APP_USBD_CDC__ERROR__FAIL,
    APP_USBD_CDC__ERROR__BUSY
};

/** USB Device initialization function. */
void app_usbd_cdc__init(const struct app_usbd_cdc__context * context);
uint8_t app_usbd_cdc__transmit(const void * buffer, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* APP_USBD_CDC_H_ */
