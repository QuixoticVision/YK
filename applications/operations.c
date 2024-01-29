/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "operations.h"
#include "device_init.h"

static struct {
    uint8_t address;
    uint32_t sn;
    device_type dev_type;
    struct lock_operations ops;
} self;

void power_on(void);
void power_off(void);
void lock(void);
void unlock(void);
void modify_sn(uint8_t *sn);
uint8_t *read_sn(void);
void led_on(device_type dev_type, void *p);
void led_off(device_type dev_type, void *p);

static uint8_t query_state(void)
{
    uint8_t state;
    switch (self.dev_type) {
    case YK_LOCK:
        break;
    case YK_COLD_LOCK:
        break;
    case AVC:
        break;
    case AVC_COLD_LOCK:
        break;
    case AVC_SWITCH:
        break;
    default:
        break;
    }
    return state;
}

struct lock_operations *get_lock_operations(void)
{
    return &self.ops;
}

int lock_operations_init(struct device_lock *lock)
{
    ops.query_state = query_state;
    return RT_EOK;
}