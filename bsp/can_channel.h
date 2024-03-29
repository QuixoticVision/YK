#ifndef __CAN_CHANNEL_H__
#define __CAN_CHANNEL_H__

#include "common.h"
#include "channel.h"

struct can_err_status {
    uint32_t rcverrcnt;
    uint32_t snderrcnt;
    uint32_t bitpaderrcnt;
    uint32_t formaterrcnt;
    uint32_t ackerrcnt;
    uint32_t biterrcnt;
    uint32_t crcerrcnt;
    uint32_t errcode;
};

struct can_channel {
    rt_bool_t is_init;
    rt_device_t dev;
    rt_sem_t sem;
    rt_mutex_t lcok;
    void (*set_rx_callback) (int (*rx_callback) (size_t len, void *parameter), void *parameter);
};

int can_channel_init(struct channel *channel);

#endif /* __CAN_CHANNEL_H__ */
