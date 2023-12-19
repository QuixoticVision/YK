/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     RiceChen    first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include <device_type.h>


static device_type m_device_type = YK_LOCK;
static device m_device;

void m_device_init(device dev, device_type type)
{
    m_device.handler = protocol_select(m_device_type);
}