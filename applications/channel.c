/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     RiceChen    first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include "channel.h"

#define THREAD_CAN_RX_NAME                     "rx"
#define THREAD_CAN_RX_STACK_SIZE               2048
#define THREAD_CAN_RX_PRIORITIY                4
#define THREAD_CAN_RX_TICK                     10
static void thread_can_rx_entry(void *para);

static channel m_channel;
static rt_device_t dev;
static rt_sem_t rx_sem;

static uint8_t rx_buff[32];

static int can_channel_init(void)
{
	rt_err_t res;
	rt_thread_t thread;
	dev = rt_device_find("can1");
	if (dev == NULL) {
        rt_kprintf("find can1 failed!\n");
        return RT_ERROR;
    }

	rx_sem = rt_sem_create("rx sem", 0, RT_IPC_FLAG_FIFO);
	if (rx_sem == NULL) {
        rt_kprintf("create rx sem failed!\n");
        return RT_ERROR;
    }

	res = rt_device_open(dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
	RT_ASSERT(res == RT_EOK);
    
	thread = rt_thread_create(
        THREAD_CAN_RX_NAME,
        thread_can_rx_entry,
        NULL,
        THREAD_CAN_RX_STACK_SIZE,
        THREAD_CAN_RX_PRIORITIY,
        THREAD_CAN_RX_TICK
    );
    if (thread != RT_NULL) {
        rt_thread_startup(thread);
    }
    else {
        rt_kprintf("create can_rx thread failed!\n");
    }
	
	m_channel.user_data = dev;
    return RT_EOK;
}

/* 接收数据回调函数 */
static rt_err_t can_rx_call(rt_device_t dev, rt_size_t size)
{
    /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(rx_sem);

    return RT_EOK;
}

static void thread_can_rx_entry(void *para)
{
    int i;
    rt_err_t res;
    struct rt_can_msg rxmsg = {0};

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(dev, can_rx_call);

#ifdef RT_CAN_USING_HDR
    struct rt_can_filter_item items[5] =
    {
        RT_CAN_FILTER_ITEM_INIT(0x100, 0, 0, 0, 0x700, RT_NULL, RT_NULL), /* std,match ID:0x100~0x1ff，hdr 为 - 1，设置默认过滤表 */
        RT_CAN_FILTER_ITEM_INIT(0x300, 0, 0, 0, 0x700, RT_NULL, RT_NULL), /* std,match ID:0x300~0x3ff，hdr 为 - 1 */
        RT_CAN_FILTER_ITEM_INIT(0x211, 0, 0, 0, 0x7ff, RT_NULL, RT_NULL), /* std,match ID:0x211，hdr 为 - 1 */
        RT_CAN_FILTER_STD_INIT(0x486, RT_NULL, RT_NULL),                  /* std,match ID:0x486，hdr 为 - 1 */
        {0x555, 0, 0, 0, 0x7ff, 7,}                                       /* std,match ID:0x555，hdr 为 7，指定设置 7 号过滤表 */
    };
    struct rt_can_filter_config cfg = {5, 1, items}; /* 一共有 5 个过滤表 */
    /* 设置硬件过滤表 */
    res = rt_device_control(can_dev, RT_CAN_CMD_SET_FILTER, &cfg);
    RT_ASSERT(res == RT_EOK);
#endif

    while (1) {
        /* hdr 值为 - 1，表示直接从 uselist 链表读取数据 */
        rxmsg.hdr_index = -1;
        /* 阻塞等待接收信号量 */
        rt_sem_take(rx_sem, RT_WAITING_FOREVER);
        /* 从 CAN 读取一帧数据 */
        rt_device_read(dev, 0, &rxmsg, sizeof(rxmsg));
        /* 打印数据 ID 及内容 */
        rt_kprintf("ID:%x", rxmsg.id);
        for (i = 0; i < 8; i++) {
            rt_kprintf("%2x", rxmsg.data[i]);
        }

        rt_kprintf("\n");
    }
}

static int can_channel_send(data *frame)
{
    int res;
    res = rt_device_write(dev, 0, frame->p_data, frame->len);
    return res;
}

static int can_channel_recv(void)
{
    data frame;
    uint32_t len;
    len = rt_device_read(dev, 0, rx_buff, 8);
    
    frame.len = len;
    frame.p_data = rx_buff;
    
    m_channel.recv(&frame);
}

channel *channel_select(channel_type type)
{
	channel *p_channel = &m_channel;
	switch (type) {
	case CHANNEL_HW_USING_CAN:
        p_channel->init = can_channel_init;
		p_channel->send = can_channel_send;
		break;
	case CHANNEL_HW_USING_UART:
		break;
	default:
		break;
	}
    return p_channel;
}
