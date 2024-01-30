/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-18     LYX          first edition
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "device_init.h"
#include "channel.h"

#define DBG_TAG                 "main"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

static int rx_cb(size_t len, void *parameter)
{
    if (parameter == NULL) {
        return 0;
    }

    struct device_lock *lock = parameter;
    size_t size;
    uint8_t buff[16];
    size = lock->channel->read(buff, len);
    if (size == 0) {
        M_LOG_E("read error");
        return RT_ERROR;
    }

    int res;
    struct parsed_data data;
    res = lock->protocol->parser(&data, buff, size);
    if (res != RT_EOK) {
        M_LOG_E("parse error");
        return RT_ERROR;
    }

    res = lock->protocol->handler(&data);
    if (res != RT_EOK) {
        M_LOG_E("handle error");
        return RT_ERROR;
    }

    uint32_t frame_head = (uint32_t)(buff[0] << 0) + (uint32_t)(buff[1] << 8) + (uint32_t)(buff[2] << 16);
    M_PRINTF("0x%06x: ", frame_head);
    for (int i = 3; i < len; i++) {
        M_PRINTF("0x%02x ", buff[i]);
    }
    M_PRINTF("(len = %d)", len);
    M_PRINTF("\n");

    return 0;
}

int main(void)
{
    struct device_lock *lock = device_create();
    /* SN码顺序为从高到低 */
    if (device_init(lock, YK_LOCK, CHANNEL_HW_USING_CAN, 0x10, 0x332211) != RT_EOK) {
        return -1;
    }
    lock->channel->open(lock->info);
    set_rx_callback(rx_cb, lock);
    while (1) {
        rt_thread_mdelay(500);
    }
}
