#ifndef __CAN_CHANNEL_H__
#define __CAN_CHANNEL_H__

#include "common.h"
#include "channel.h"

struct can_channel {
    rt_bool_t is_init;
    rt_device_t dev;
    rt_sem_t sem;
    int (*rx_callback) (size_t len, void *parameter);
    void *parameter;
};

extern int can_channel_init(struct channel *channel);
extern void set_can_rx_callback(int (*rx_callback) (size_t len, void *parameter), void *parameter);

#endif /* __CAN_CHANNEL_H__ */
