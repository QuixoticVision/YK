#ifndef __DEVICE_INIT_H__
#define __DEVICE_INIT_H__

#include "common.h"
#include "protocol.h"
#include "channel.h"

/* 锁设备类型定义 */
struct device_lock {
    struct device_info *info;
    struct channel *channel;
    struct protocol *protocol;
};

extern struct device_lock *device_create(void);
extern void device_delete(struct device_lock *lock);
extern int device_init(struct device_lock *lock, device_type dev_type, channel_type ch_type, uint8_t addr, const uint8_t *sn);

#endif /* __DEVICE_INIT_H__ */
