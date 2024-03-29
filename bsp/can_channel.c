/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */

#include <rtthread.h>
#include <rtdevice.h>

#define DBG_TAG                 "can"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

#include "can_channel.h"

#define CAN_TX_DEFAULT_MESSAGE                 \
    {                                          \
        .id        = 0,                        \
        .ide       = RT_CAN_EXTID,             \
        .rtr       = RT_CAN_DTR,               \
        .len       = 0,                        \
        .priv      = 0,                        \
        .hdr_index = 0,                        \
        .rxfifo    = 0,                        \
        .reserved  = 0,                        \
        .data      = {0, 0, 0, 0, 0, 0, 0, 0}, \
    }                                          \

#define THREAD_CAN_RX_NAME       "can"
#define THREAD_CAN_RX_STACK_SIZE 2048
#define THREAD_CAN_RX_PRIORITIY  5
#define THREAD_CAN_RX_TICK       10
static void thread_can_rx(void *para);

static struct {
    struct can_channel channel;
    struct rt_can_msg msg_buff;
    int (*rx_callback) (size_t len, void *parameter);
    void *parameter;
} self;

static rt_err_t can_rx_call(rt_device_t dev, rt_size_t size)
{
    /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(self.channel.sem);
    return RT_EOK;
}

static int can_channel_open(void *parameter)
{
    rt_err_t res;
    static rt_thread_t thread = NULL;
    rt_device_t dev = self.channel.dev;

    res = rt_device_open(dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
    RT_ASSERT(res == RT_EOK);

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(dev, can_rx_call);
    rt_device_control(dev, RT_CAN_CMD_SET_BAUD, (void *)CAN20kBaud);

#ifdef RT_CAN_USING_HDR
    if (parameter) {
        struct rt_can_filter_config *cfg = parameter;
        M_PRINTF("can filter: ");
        for (int i = 0; i < cfg->count; i++) {
            M_PRINTF("0x%08x ", cfg->items[i].id);
        }
        M_PRINTF("\n");
        /* 设置硬件过滤表 */
        res = rt_device_control(dev, RT_CAN_CMD_SET_FILTER, cfg);
        RT_ASSERT(res == RT_EOK);
    }
#endif
    if (thread != NULL) {
        return RT_EOK;
    }
    thread = rt_thread_create(
        THREAD_CAN_RX_NAME,
        thread_can_rx,
        NULL,
        THREAD_CAN_RX_STACK_SIZE,
        THREAD_CAN_RX_PRIORITIY,
        THREAD_CAN_RX_TICK);
    if (thread == RT_NULL) {
        M_LOG_E("create can_rx thread failed!\n");
        return -RT_ERROR;
    }
    rt_thread_startup(thread);

    return RT_EOK;
}

static void thread_can_rx(void *parameter)
{
    rt_device_t dev = self.channel.dev;
    rt_sem_t rx_sem = self.channel.sem;
    struct rt_can_msg *rx_msg = &self.msg_buff;

    while (1) {
        /* hdr 值为 - 1，表示直接从 uselist 链表读取数据 */
        rx_msg->hdr_index = -1;
        /* 阻塞等待接收信号量 */
        rt_sem_take(rx_sem, RT_WAITING_FOREVER);
        rt_device_read(dev, 0, rx_msg, sizeof(struct rt_can_msg));
        if (self.rx_callback != NULL) self.rx_callback(rx_msg->len + 3, self.parameter);
    }
}

static int can_channel_read(uint8_t *buff, size_t len)
{
    size_t size;
    const struct rt_can_msg *rx_msg = &self.msg_buff;

    if (len < 3) {
        return 0;
    }

    //高位在前
    buff[0] = (uint8_t)(rx_msg->id >> 16);
    buff[1] = (uint8_t)(rx_msg->id >> 8);
    buff[2] = (uint8_t)(rx_msg->id >> 0);
    for (size = 3; size < len && size < rx_msg->len + 3; size++) {
        buff[size] = rx_msg->data[size - 3];
    }

    M_PRINTF("\n================ row data =================\n");
    M_LOG_I("ID:%x", rx_msg->id);
    M_PRINTF("%dbytes: ", rx_msg->len);
    for (int i = 0; i < len - 3; i++) {
        M_PRINTF("0x%02x ", rx_msg->data[i]);
    }
    M_PRINTF("\n================ row data =================\n");

    return size;
}

static int can_channel_write(uint8_t *data, size_t len)
{
    /*
    ** 帧头：3字节
    */
    if (len < 3) {
        return 0;
    }

    struct rt_can_msg tx_msg = CAN_TX_DEFAULT_MESSAGE;
    rt_device_t dev = self.channel.dev;

    tx_msg.id += (uint32_t)(data[0] << 16);
    tx_msg.id += (uint32_t)(data[1] << 8);
    tx_msg.id += (uint32_t)(data[2] << 0);
    tx_msg.len = len - 3;
    memcpy(tx_msg.data, &data[3], len - 3);

    M_PRINTF("\n================ write =================\n");
    M_PRINTF("0x%06x: ", tx_msg.id);
    for (int i = 0; i < len - 3; i++) {
        M_PRINTF("0x%02x ", tx_msg.data[i]);
    }
    M_PRINTF("(len = %d)", len - 3);
    M_PRINTF("\n================ write =================\n");

    /* 发送一帧 CAN 数据 */
    rt_mutex_take(self.channel.lcok, RT_WAITING_FOREVER);
    size_t size = rt_device_write(dev, 0, &tx_msg, sizeof(tx_msg));
    if (size == 0) {
        M_PRINTF("can dev write data failed!\n");
    }
    rt_mutex_release(self.channel.lcok);
    return size;
}

static void can_channel_set_rx_callbck(int (*rx_callback) (size_t len, void *parameter), void *parameter)
{
    self.rx_callback = rx_callback;
    self.parameter = parameter;
}

int can_channel_init(struct channel *channel)
{
    rt_device_t dev;
    rt_sem_t rx_sem;
    rt_mutex_t lock;

    if (self.channel.dev == NULL) {
        dev = rt_device_find("can1");
        if (dev == NULL) {
            M_LOG_E("find can1 failed!\n");
            return RT_ERROR;
        }
        self.channel.dev = dev;
    }

    if (self.channel.sem == NULL) {
        rx_sem = rt_sem_create("can rx", 0, RT_IPC_FLAG_PRIO);
        if (rx_sem == NULL) {
            M_LOG_E("create can rx sem failed!\n");
            return RT_ERROR;
        }
        self.channel.sem = rx_sem;
    }

    if (self.channel.lcok == NULL) {
        lock = rt_mutex_create("can tx", RT_IPC_FLAG_PRIO);
        if (lock == NULL) {
            M_LOG_E("create mutex failed!\n");
            return RT_ERROR;
        }
        self.channel.lcok = lock;
    }

    channel->open = can_channel_open;
    channel->read = can_channel_read;
    channel->write = can_channel_write;
    channel->set_rx_callback = can_channel_set_rx_callbck;

    return RT_EOK;
}


