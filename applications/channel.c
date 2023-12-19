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

static channel m_channel;
static rt_device_t dev;
static rt_sem_t rx_sem;

static void can_rx_thread(void *para);

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
	thread = rt_thread_create("can_rx", can_rx_thread, RT_NULL, 1024, 25, 10);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        rt_kprintf("create can_rx thread failed!\n");
    }
	
	m_channel.user_data = dev;
    return RT_EOK;
}

static void can_rx_thread(void *para)
{

}

static void can_channel_recv()
{}

channel channel_init(channel_hw_t type)
{
	rt_thread_t thread;
	switch (type) {
	case CHANNEL_HW_USING_CAN:
		
		break;
	case CHANNEL_HW_USING_UART:
		break;
	default:
		break;
	}
}
