#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "common.h"

typedef int (*callback)  (data_t *frame);

struct channel {
    int (*init) (struct device_lock *dev_lock);         //通道初始化
    int (*read) (uint8_t *buff, uint8_t len);  //数据接收
    int (*write) (uint8_t *data, size_t len);  //数据发送
    void *user_data;                //备用
};

typedef enum {
	CHANNEL_HW_USING_CAN = 0,
	CHANNEL_HW_USING_UART,
} channel_type;

extern struct channel *channel_select(channel_type type);

#endif /* __CHANNEL_H__ */
