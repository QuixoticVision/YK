#ifndef __HW_DEFINE_H__
#define __HW_DEFINE_H__

#include <common.h>

/* YK */
#define PIN_YK_WDI                      GET_PIN(A, 0)
#define PIN_YK_RELAY_CT1                GET_PIN(C, 6)
#define PIN_YK_RELAY_CT2                GET_PIN(B, 15)
#define PIN_YK_RELAY_CT3                GET_PIN(B, 14)
#define PIN_YK_DIN                      GET_PIN(B, 1)
#define PIN_YK_DOUT                     GET_PIN(B, 12)
#define PIN_YK_D5V0_OUT                 GET_PIN(B, 13)

#if DEFAULT_HARDWARE_TYPE
#define YK_INPUT_STATE() rt_pin_read(PIN_YK_DIN)
#define YK_RELAY_K0_STATE() \
    do {                    \
    } while (0)
#define YK_RELAY_K0_OPEN()              rt_pin_write(PIN_YK_DOUT, PIN_HIGH)
#define YK_RELAY_K0_CLOSE()             rt_pin_write(PIN_YK_DOUT, PIN_LOW)
#define YK_REALY_K0_STATE()             rt_pin_read(PIN_YK_DOUT)
#define YK_RELAY_K1_OPEN()              YK_REALY_OPEN(PIN_YK_RELAY_CT1)
#define YK_RELAY_K1_CLOSE()             YK_RELAY_CLOSE(PIN_YK_RELAY_CT1)
#define YK_REALY_K1_STATE()             rt_pin_read(PIN_YK_RELAY_CT1)
#define YK_RELAY_K2_OPEN()              YK_REALY_OPEN(PIN_YK_RELAY_CT2)
#define YK_RELAY_K2_CLOSE()             YK_RELAY_CLOSE(PIN_YK_RELAY_CT2)
#define YK_REALY_K2_STATE()             rt_pin_read(PIN_YK_RELAY_CT2)
#define YK_RELAY_K3_OPEN()              YK_REALY_OPEN(PIN_YK_RELAY_CT3)
#define YK_RELAY_K3_CLOSE()             YK_RELAY_CLOSE(PIN_YK_RELAY_CT3)
#define YK_REALY_K3_STATE()             rt_pin_read(PIN_YK_RELAY_CT2)
#else
#define YK_REALY_STATE_OPEN             1
#define YK_REALY_STATE_CLOSE            0
#define YK_INPUT_STATE()                rt_pin_read(PIN_AVC_DIN)
#define YK_RELAY_K0_STATE()             AVC_RELAY_CHECK()
#define YK_REALY_K1_STATE()             rt_pin_read(PIN_AVC_CT1)
#define YK_REALY_K2_STATE()             rt_pin_read(PIN_AVC_CT2)
#define YK_REALY_K3_STATE()             0
#define YK_RELAY_K0_OPEN()              AVC_RELAY_DISENGAGE()
// #define YK_RELAY_K0_CLOSE()             AVC_RELAY_ENGAGE()
// #define YK_REALY_K0_STATE()             AVC_RELAY_CHECK()
#define YK_RELAY_K0_CLOSE()             rt_pin_write(PIN_AVC_RELAY_CLOSE, 1);rt_pin_write(PIN_AVC_RELAY_OPEN, 0);
#define YK_REALY_K0_STATE()             rt_pin_write(PIN_AVC_RELAY_CLOSE, 0);rt_pin_write(PIN_AVC_RELAY_OPEN, 1);
#define YK_RELAY_K1_OPEN()                      \
        do {                                    \
            rt_pin_write(PIN_AVC_CT1, PIN_HIGH);\
            rt_pin_write(PIN_AVC_K1, PIN_LOW);  \
        } while (0)
#define YK_RELAY_K1_CLOSE()                     \
        do {                                    \
            rt_pin_write(PIN_AVC_CT1, PIN_LOW); \
            rt_pin_write(PIN_AVC_K1, PIN_HIGH); \
        } while (0)
#define YK_RELAY_K2_OPEN()                      \
        do {                                    \
            rt_pin_write(PIN_AVC_CT2, PIN_HIGH);\
            rt_pin_write(PIN_AVC_K2, PIN_LOW);  \
        } while (0)
#define YK_RELAY_K2_CLOSE()                     \
        do {                                    \
            rt_pin_write(PIN_AVC_CT2, PIN_LOW); \
            rt_pin_write(PIN_AVC_K2, PIN_HIGH); \
        } while (0)

#define YK_RELAY_K3_OPEN()                      \
        do {                                    \
        } while (0)
#define YK_RELAY_K3_CLOSE()                     \
        do {                                    \
        } while (0)
#define YK_REALY_OPEN(pin)              rt_pin_write(pin, PIN_HIGH)
#define YK_RELAY_CLOSE(pin)             rt_pin_write(pin, PIN_LOW)
#endif

/* COLD_LOCK_YK */
#define PIN_COLD_LOCK_YK_WDI            GET_PIN(A, 0)
#define PIN_COLD_LOCK_YK_DC48V_OUT      GET_PIN(C, 7)

/* AVC、YK(AVC)、AVC_SWITCH */
#define PIN_AVC_WDI                     GET_PIN(A, 0)
#define PIN_AVC_LED_CT1                 GET_PIN(C, 12)
#define PIN_AVC_LED_CT2                 GET_PIN(C, 11)
#define PIN_AVC_LED_CT3                 GET_PIN(C, 10)
#define PIN_AVC_LED_CT4                 GET_PIN(A, 15)
#define PIN_AVC_CT1                     PIN_AVC_LED_CT1
#define PIN_AVC_K1                      PIN_AVC_LED_CT2
#define PIN_AVC_CT2                     PIN_AVC_LED_CT4
#define PIN_AVC_K2                      PIN_AVC_LED_CT3
#define PIN_AVC_RELAY_CLOSE             GET_PIN(C, 9)
#define PIN_AVC_RELAY_OPEN              GET_PIN(C, 8)
#define PIN_AVC_RELAY_CHECK             GET_PIN(B, 0)
#define PIN_AVC_DIN                     GET_PIN(B, 1)
#define TIME_RELAY_CONTROL              10
#define AVC_RELAY_CONTROL(pin, t)           \
        do {                                \
            rt_pin_write(pin, PIN_HIGH);    \
            rt_thread_delay(t);             \
            rt_pin_write(pin, PIN_LOW);     \
        } while(0)
#define AVC_RELAY_ENGAGE()              AVC_RELAY_CONTROL(PIN_AVC_RELAY_CLOSE, TIME_RELAY_CONTROL)
#define AVC_RELAY_DISENGAGE()           AVC_RELAY_CONTROL(PIN_AVC_RELAY_OPEN, TIME_RELAY_CONTROL)
#define AVC_RELAY_CHECK()               rt_pin_read(PIN_AVC_RELAY_CHECK)


/* COLD_LOCK_AVC */
#define PIN_COLD_LOCK_AVC_WDI           GET_PIN(A, 0)
#define PIN_COLD_LOCK_AVC_RELAY_CHECK   GET_PIN(B, 0)
#define PIN_COLD_LOCK_AVC_DC48V_OUT     GET_PIN(C, 7)

typedef enum {
    AVC_RELAY_STATE_OPEN = 0,
    AVC_RELAY_STATE_CLOSE,
} avc_relay_state_t;

#endif /* __HW_DEFINE_H__ */
