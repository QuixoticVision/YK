#ifndef __COMMON_H__
#define __COMMON_H__

#include <rtthread.h>

#define TRUE                            1
#define FALSE                           0
#define ARRAY_SIZE(arr)                 (sizeof(arr) / sizeof((arr)[0]))
#define DEFAULT_DEVICE_TYPE             YK_LOCK                 //默认设备类型：YK锁
#define DEFAULT_CHANNEL_TYPE            CHANNEL_HW_USING_CAN    //默认通道类型：CAN

typedef void (*func) (void);

typedef enum {
    YK_LOCK,        //YK锁
    YK_COLD_LOCK,   //冷闭锁
    AVC,            //AVC
    AVC_COLD_LOCK,  //AVC冷闭锁
    AVC_SWITCH      //AVC开关
} device_type;

typedef struct {
	uint8_t *p_data;
	uint32_t len;
} data;

#endif /* __COMMON_H__ */
