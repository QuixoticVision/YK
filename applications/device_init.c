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

static device m_device;

static int m_device_init(device *device, device_type dev_type, channel_type ch_type)
{
    device->protocol = protocol_select(dev_type);
    device->channel = channel_select(ch_type);

    return TRUE;
}

device *device_select(device_type dev_type, channel_type ch_type)
{
    device *p_dev = &m_device;
    m_device_init(p_dev, dev_type, ch_type);

    return p_dev;
}


