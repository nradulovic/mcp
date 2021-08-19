#ifndef USBD_CONF_H_
#define USBD_CONF_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

/* Needed for __STATIC_INLINE */
#include "stm32f4xx.h"

#define USBD_MAX_NUM_INTERFACES     1U
#define USBD_MAX_NUM_CONFIGURATION     1U
#define USBD_MAX_STR_DESC_SIZ     512U
#define USBD_DEBUG_LEVEL     0U
#define USBD_LPM_ENABLED     0U
#define USBD_SELF_POWERED     1U

enum usbd_conf__device_mode
{
    USBD_CONF__DEV_MODE__FULL_SPEED = 0,
    USBD_CONF__DEV_MODE__HIGH_SPEED = 1
};

/** Alias for memory allocation. */
#define USBD_malloc         malloc

/** Alias for memory release. */
#define USBD_free           free

/* DEBUG macros */

#if (USBD_DEBUG_LEVEL > 0)
#define USBD_UsrLog(...)    printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 1)

#define USBD_ErrLog(...)    printf("ERROR: ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_ErrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 2)
#define USBD_DbgLog(...)    printf("DEBUG : ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* USBD_CONF_H_ */
