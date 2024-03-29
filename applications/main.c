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

#define DBG_TAG                 "main"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

#include "lock.h"
#include "channel.h"
#include "protocol.h"
#include "button.h"
#include "crc8.h"
#include "operation.h"
#include "hw_define.h"
#include "board.h"

#define THREAD_BTN_NAME             "btn"
#define THREAD_BTN_STACK_SIZE       512
#define THREAD_BTN_PRIORITY         6
#define THREAD_BTN_TICK             10
static void thread_btn_press(void *);
rt_mutex_t mutex;

static int rx_cb(size_t len, void *parameter)
{
    if (parameter == NULL) {
        return 0;
    }

    struct lock *lock = parameter;
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
        M_LOG_E("parse error\n");
        return RT_ERROR;
    }

    uint32_t frame_head = (uint32_t)(buff[0] << 16) + (uint32_t)(buff[1] << 8) + (uint32_t)(buff[2] << 0);
    M_PRINTF("\n================ read =================\n");
    M_PRINTF("0x%06x: ", frame_head);
    for (int i = 3; i < len; i++) {
        M_PRINTF("0x%02x ", buff[i]);
    }
    M_PRINTF("(len = %d)", len - 3);
    M_PRINTF("\n================ read =================\n");

    res = lock->protocol->handler(&data);
    if (res != RT_EOK) {
        M_LOG_E("handle error");
        return RT_ERROR;
    }

    return 0;
}

static void btn_press_cb(void)
{
    if (rt_mutex_take(mutex, RT_WAITING_NO) == RT_ERROR) return;
    rt_thread_t thread;
    thread = rt_thread_create(
                THREAD_BTN_NAME,
                thread_btn_press,
                mutex,
                THREAD_BTN_STACK_SIZE,
                THREAD_BTN_PRIORITY,
                THREAD_BTN_TICK
            );
    rt_thread_startup(thread);
}

static void thread_btn_press(void *parameter)
{
    rt_mutex_t mutex = parameter;
    struct lock *lock = get_lock_handler();
    if (lock->info->lock_type != AVC_SWITCH) return;
    uint8_t buff[8];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x03;
    buff[4] = 0x32;
    buff[5] = lock->ops->get_addr();
    buff[6] = AVC_RELAY_CHECK() ? 0x01 : 0x10;
    buff[7] = crc8(buff, ARRAY_SIZE(buff) - 1);

    lock->channel->write(buff, ARRAY_SIZE(buff));

    rt_mutex_release(mutex);
}

int main(void)
{
    struct lock *lock = get_lock_handler();
    lock->init();

    lock->info->data = (void *)CAN20kBaud;

    uint8_t addr = lock->info->addr;
    uint8_t net_id = lock->info->net_id;
    uint8_t dev_id = lock->info->dev_id;
    uint8_t lock_type = lock->info->lock_type;
    uint8_t hw_type = lock->info->hardware_type;
    
    uint32_t id = (uint32_t)(addr << 0) + (uint32_t)(net_id << 8) + (uint32_t)(dev_id << 16);
    struct rt_can_filter_item items[] = {
        RT_CAN_FILTER_EXT_INIT(id, NULL, NULL),
        RT_CAN_FILTER_EXT_INIT(0, NULL, NULL),
    };
    struct rt_can_filter_config cfg = {1, 1, items};

    if (hw_type == HW_AVC_SWITCH) {
        items[0].id = (uint32_t)(addr << 0) + (uint32_t)(net_id << 8) + (uint32_t)(get_device_id_by_lock_type(YK_LOCK) << 16);
        items[1].id = (uint32_t)(addr << 0) + (uint32_t)(net_id << 8) + (uint32_t)(get_device_id_by_lock_type(AVC_SWITCH) << 16);
        cfg.count = 2;
    }

    if ((lock->info->lock_type == AVC_COLD_LOCK) || (lock->info->lock_type == YK_COLD_LOCK)) {
        /* 冷闭锁需要支持当功能码为0x2F时设备地址为任意数据的报文，因此硬件上不过滤设备地址，在报文解析中再判断 */
        items[0].mask <<= 8; /* 0xFFFF FF00 */
    }

    lock->channel->open(&cfg);
    lock->channel->set_rx_callback(rx_cb, lock);

    bind_button_press_event(btn_press_cb);
    mutex = rt_mutex_create("btn", RT_IPC_FLAG_PRIO);

    uint8_t data[10] = {1, 2, 3, 4 ,5 ,6 ,7 ,8 ,9, 10};
    while (1) {
        //lock->channel->write(data,10);
        rt_thread_mdelay(1000);
    }
}
