#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "common.h"

struct channel {
    int (*open) (void *parameter);
    int (*read) (uint8_t *buff, size_t len);  //数据接收
    int (*write) (uint8_t *data, size_t len);  //数据发送
    void (*set_rx_callback) (int (*rx_callback) (size_t len, void *parameter), void *parameter);
    void *user_data;                //备用
};

int channel_init(struct channel *channel, channel_type_t type);
#endif /* __CHANNEL_H__ */
