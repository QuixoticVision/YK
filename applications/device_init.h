#ifndef __DEVICE_INIT_H__
#define __DEVICE_INIT_H__

#include "common.h"
#include "protocol.h"
#include "channel.h"

/* 设备信息：设备地址 SN码 设备类型 */
struct device_info {
    uint8_t addr;
    uint8_t sn[3];
    uint8_t id;
    device_type type;
};

/* 锁设备类型定义 */
struct device_lock {
    struct device_info info;
    struct channel *channel;
    protocol *protocol;
};

#endif /* __DEVICE_INIT_H__ */
