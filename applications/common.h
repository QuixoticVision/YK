#ifndef __COMMON_H__
#define __COMMON_H__

#include <rtthread.h>
#include <string.h>
#include <stdio.h>

#define DEBUG_ENABLE                    1
#if DEBUG_ENABLE
#define M_PRINTF                        rt_kprintf
#define M_LOG_I                         LOG_I
#define M_LOG_E                         LOG_E
#else
#define M_PRINTF(...)                   do {} while(0)
#define M_LOG_I(...)                    do {} while(0)
#define M_LOG_E(...)                    do {} while(0)
#endif

#define TRUE                            1
#define FALSE                           0
#define ARRAY_SIZE(arr)                 (sizeof(arr) / sizeof((arr)[0]))
#define DEFAULT_DEVICE_TYPE             YK_LOCK                 //默认设备类型：YK锁
#define DEFAULT_CHANNEL_TYPE            CHANNEL_HW_USING_CAN    //默认通道类型：CAN

typedef void (*func) (void);

typedef enum {
    YK_LOCK = 0,    //YK锁
    YK_COLD_LOCK,   //冷闭锁
    AVC,            //AVC
    AVC_COLD_LOCK,  //AVC冷闭锁
    AVC_SWITCH,     //AVC开关
    DEVICE_TYPE_MAX
} device_type;

typedef enum {
	CHANNEL_HW_USING_CAN = 0,
	CHANNEL_HW_USING_UART,
} channel_type;

/* 设备信息：设备地址 SN码 设备类型 */
struct device_info {
    uint8_t dev_id; //设备ID
    uint8_t net_id; //CAN网区号
    uint8_t addr;   //设备地址
    uint32_t sn;  //SN号
    device_type dev_type;       //设备类型
    channel_type channel_type;  //通道类型
};

typedef struct {
	uint8_t *data;
	uint32_t len;
} data_t;


#endif /* __COMMON_H__ */
