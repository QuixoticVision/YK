#ifndef __CHANNEL_CAN_H__
#define __CHANNEL_CAN_H__

#include "common.h"
#include "device_init.h"

typedef int (*hook) (data_t *buff);

struct channel_can_ops {
    int (*init) (struct device_lock *dev_lock);
    int (*write) (uint8_t *data, size_t len);
    int (*read) (uint8_t *buff, uint8_t len);
};

struct channel_can {
    rt_bool_t is_init;
    rt_device_t dev;
    rt_sem_t sem;
    int (*rx_callback) (size_t size);
};

extern struct channel_can_ops *get_can_ops_handler(void);
extern void set_can_rx_callback(int (*rx_callback) (size_t size));

#endif /* __CHANNEL_CAN_H__ */
