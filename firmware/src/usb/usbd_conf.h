#ifndef USBD_CONF_H
#define USBD_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USBD_MAX_NUM_INTERFACES     1U
#define USBD_MAX_NUM_CONFIGURATION  1U
#define USBD_MAX_STR_DESC_SIZ      512U
#define USBD_SELF_POWERED           1U
#define USBD_DEBUG_LEVEL            0U
#define USBD_LPM_ENABLED            0U

#define USBD_CDC_INTERVAL           2000U

#define USBD_malloc(size)           ((void *)usbd_static_buf)
#define USBD_free(ptr)              ((void)0)
#define USBD_memset                 memset
#define USBD_memcpy                 memcpy
#define USBD_Delay                  HAL_Delay

extern uint32_t usbd_static_buf[];

#if (USBD_DEBUG_LEVEL > 0U)
#define USBD_UsrLog(...)
#define USBD_ErrLog(...)
#define USBD_DbgLog(...)
#else
#define USBD_UsrLog(...)
#define USBD_ErrLog(...)
#define USBD_DbgLog(...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* USBD_CONF_H */
