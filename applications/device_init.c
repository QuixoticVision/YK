/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include "device_init.h"
#include "operations.h"

#define DBG_TAG                 "lock"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

/**
 *  YK_LOCK = 0,    //YK锁
 *  YK_COLD_LOCK,   //冷闭锁
 *  AVC,            //AVC
 *  AVC_COLD_LOCK,  //AVC冷闭锁
 *  AVC_SWITCH,     //AVC开关
 *  YK_AVC_SWITCH   //YK、AVC开关合体
 */
static const uint8_t device_id_table[DEVICE_TYPE_MAX] = {
    0x03,
    0x03,
    0x03,
    0x03,
    0x19,
    0xff,
};

struct device_lock *device_create(void)
{
    struct device_lock *lock = rt_malloc(sizeof(struct device_lock));
    lock->info = rt_malloc(sizeof(struct device_info));
    lock->channel = rt_malloc(sizeof(struct channel));
    lock->protocol = rt_malloc(sizeof(struct protocol));

    return lock;
}

void device_delete(struct device_lock *lock)
{
    rt_free(lock->info);
    rt_free(lock->channel);
    rt_free(lock->protocol);
    rt_free(lock);
}

int device_init(struct device_lock *lock, device_type_t dev_type, channel_type_t ch_type, uint8_t addr, uint32_t sn)
{
    int result = RT_EOK;

    lock->info->dev_id = device_id_table[dev_type];
    lock->info->net_id = 0x00;
    lock->info->addr = addr;
    lock->info->sn = sn;
    lock->info->dev_type = dev_type;
    lock->info->channel_type = ch_type;

    result = channel_init(lock->channel, ch_type);
    if (result != RT_EOK) {
        return result;
    }

    result = protocol_init(lock->protocol, lock->channel, dev_type, ch_type);
    if (result != RT_EOK) {
        return result;
    }

    lock_operations_init(lock);

    M_LOG_I("lock:0x%08x", lock);
    M_LOG_I("lock->channel:0x%08x", lock->channel);
    M_LOG_I("lock->protocol:0x%08x", lock->protocol);

    return result;
}
