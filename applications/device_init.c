/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     RiceChen    first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include <device_init.h>

static struct device_lock dev_lock;

static struct device_lock *device_init(device_type dev_type, channel_type ch_type, uint8_t addr, uint8_t *sn)
{
    struct device_lock *dev = &dev_lock;
    dev->info.addr = addr;
    memcpy(dev->info.sn, sn, 3);

    dev->channel = channel_select(ch_type);

    dev->protocol = protocol_select(dev_type);
    dev->protocol->init();

    return dev;
}

struct device_lock *get_device_lock_handler(void)
{
    return &dev_lock;
}
