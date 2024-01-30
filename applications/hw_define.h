#ifndef __HW_DEFINE_H__
#define __HW_DEFINE_H__

#include <rtthread.h>

/* YK */
#define PIN_YK_WDI         GET_PIN(A, 0)
#define PIN_YK_RELAY1      GET_PIN(C, 6)
#define PIN_YK_RELAY2      GET_PIN(B, 15)
#define PIN_YK_RELAY3      GET_PIN(B, 14)
#define PIN_YK_DIN         GET_PIN(B, 1)
#define PIN_YK_DOUT        GET_PIN(B, 12)
#define PIN_YK_D5V0_OUT    GET_PIN(B, 13)

/* COLD_LOCK_YK */
#define PIN_COLD_LOCK_YK_WDI            GET_PIN(A, 0)
#define PIN_COLD_LOCK_YK_DC48V_OUT      GET_PIN(C, 7)

/* AVC、YK(AVC)、AVC_SWITCH */
#define PIN_AVC_WDI                     GET_PIN(A, 0)
#define PIN_AVC_LED1                    GET_PIN(C, 12)
#define PIN_AVC_LED2                    GET_PIN(C, 11)
#define PIN_AVC_LED3                    GET_PIN(C, 10)
#define PIN_AVC_LED4                    GET_PIN(A, 15)
#define PIN_AVC_RELAY_CLOSE             GET_PIN(C, 9)
#define PIN_AVC_RELAY_OPEN              GET_PIN(C, 8)
#define PIN_AVC_RELAY_CHECK             GET_PIN(B, 0)
#define PIN_AVC_DIN                     GET_PIN(B, 1)

/* COLD_LOCK_AVC */
#define PIN_COLD_LOCK_AVC_WDI           GET_PIN(A, 0)
#define PIN_COLD_LOCK_AVC_RELAY_CHECK   GET_PIN(B, 0)
#define PIN_COLD_LOCK_AVC_DC48V_OUT     GET_PIN(C, 7)

#endif /* __HW_DEFINE_H__ */
