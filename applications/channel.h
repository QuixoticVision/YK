#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "common.h"

typedef int (*callback)  (data *frame);

typedef struct {
    int (*init)     (void);         //通道初始化
    int (*recv)     (data *frame);  //数据接收
    int (*send)     (data *frame);  //数据发送
    void *user_data;                //备用
} channel;

typedef enum {
	CHANNEL_HW_USING_CAN = 0,
	CHANNEL_HW_USING_UART,
} channel_type;

extern channel *channel_select(channel_type type);

#endif /* __CHANNEL_H__ */
