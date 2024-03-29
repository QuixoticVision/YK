/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include <common.h>

#define DBG_TAG                 "lock"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

#include "lock.h"
#include "operation.h"
#include "protocol.h"
#include "hw_define.h"
#include "board.h"

static struct {
    struct lock lock;
    struct channel channel;
    struct protocol protocol;
    struct lock_info info;
} self;

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
};

uint8_t get_device_id_by_lock_type(lock_type_t type)
{
    return device_id_table[type];
}

static void lock_pin_init(void)
{
    int type = self.info.hardware_type;
    switch (type) {
    case HW_YK:
        rt_pin_mode(PIN_YK_RELAY_CT1, PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_YK_RELAY_CT2, PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_YK_RELAY_CT3, PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_YK_D5V0_OUT, PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_YK_DOUT, PIN_MODE_OUTPUT);
        break;
    case HW_YK_COLD_LOCK:
        break;
    case HW_AVC_COLD_LOCK:
        break;
    case HW_AVC_SWITCH:
        rt_pin_mode(PIN_AVC_CT1, PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_AVC_CT2, PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_AVC_K1, PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_AVC_K1, PIN_MODE_OUTPUT);
        rt_pin_mode(PIN_AVC_RELAY_CHECK, PIN_MODE_INPUT);
        break;
    default:
        break;
    }
}

void lock_info_update(void)
{
    struct lock *lock = &self.lock;
    uint8_t addr = lock->ops->get_addr();
    uint32_t sn = lock->ops->get_sn();
    lock_type_t lock_type = lock->ops->get_device();

    lock->info->dev_id = get_device_id_by_lock_type(lock_type);
    lock->info->net_id = 0x00;
    lock->info->addr = addr;
    lock->info->sn = sn;
    lock->info->lock_type = lock_type;
    lock->info->channel_type = DEFAULT_CHANNEL_TYPE;
    lock->info->hardware_type = DEFAULT_HARDWARE_TYPE;
}

static int lock_init(void)
{
    struct lock *lock = &self.lock;
    int result = RT_EOK;
    
    lock_info_update();

    lock_pin_init();

    result = channel_init(lock->channel, lock->info->channel_type);
    if (result != RT_EOK) {
        return result;
    }

    result = protocol_init(lock);
    if (result != RT_EOK) {
        return result;
    }
    return result;
}

static int lock_pre_init(void)
{
    self.lock.ops = get_lock_operation();
    self.lock.channel = &self.channel;
    self.lock.protocol = &self.protocol;
    self.lock.info = &self.info;
    self.lock.init = lock_init;
    return RT_EOK;
}
INIT_ENV_EXPORT(lock_pre_init);

struct lock *get_lock_handler(void)
{
    return &self.lock;
}
