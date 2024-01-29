#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "common.h"

struct channel {
    int (*open) (const struct device_info *info);
    int (*read) (uint8_t *buff, size_t len);  //数据接收
    int (*write) (uint8_t *data, size_t len);  //数据发送
    void *user_data;                //备用
};

extern int channel_init(struct channel *channel, channel_type type);
extern void set_rx_callback(int (*rx_callback) (size_t len, void *parameter), void *parameter);
#endif /* __CHANNEL_H__ */
