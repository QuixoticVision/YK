#ifndef __LOCK_H__
#define __LOCK_H__

#include "common.h"
#include "channel.h"

/* 锁设备类型定义 */
struct lock {
    int (*init) (void);
    struct lock_info *info;
    struct channel *channel;
    struct protocol *protocol;
    struct lock_operation *ops;
};

struct lock *get_lock_handler(void);
uint8_t get_device_id_by_lock_type(lock_type_t type);
void lock_info_update(void);

#endif /* __LOCK_H__ */
