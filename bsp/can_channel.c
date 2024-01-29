/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "can_channel.h"

#define DBG_TAG                 "can"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

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

#define CAN_RX_BUFF_MAX_LEN     10

#define THREAD_CAN_RX_NAME       "rx"
#define THREAD_CAN_RX_STACK_SIZE 2048
#define THREAD_CAN_RX_PRIORITIY  4
#define THREAD_CAN_RX_TICK       10
static void thread_can_rx(void *para);

struct rx_ring_buffer {
    uint8_t head;
    uint8_t tail;
    uint8_t size;
    struct rt_can_msg buff[CAN_RX_BUFF_MAX_LEN];
};

static struct can_channel can_channel;
static struct rx_ring_buffer ring_buffer;

static void ring_buffer_init(struct rx_ring_buffer *buffer)
{
    buffer->head = 0;
    buffer->tail = 0;
    buffer->size = CAN_RX_BUFF_MAX_LEN;
}

static int write_ring_buffer(struct rx_ring_buffer *buffer, const struct rt_can_msg *data)
{
    uint8_t next = (buffer->tail + 1) % buffer->size;

    // 判断缓冲区是否已满
    if (next != buffer->head) {
        buffer->buff[buffer->tail] = *data;
        buffer->tail = next;
        return 0;  // 写入成功
    } else {
        return -1;  // 缓冲区已满，写入失败
    }
}

static int read_ring_buffer(struct rx_ring_buffer *buffer, struct rt_can_msg *data)
{
    // 判断缓冲区是否为空
    if (buffer->head != buffer->tail) {
        *data = buffer->buff[buffer->head];
        buffer->head = (buffer->head + 1) % buffer->size;
        return 0;  // 读取成功
    } else {
        return -1;  // 缓冲区为空，读取失败
    }
}

static rt_err_t can_rx_call(rt_device_t dev, rt_size_t size)
{
    /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(can_channel.sem);
    return RT_EOK;
}

static int can_channel_open(const struct device_info *info)
{
    rt_err_t res;
    static rt_thread_t thread;
    rt_device_t dev = can_channel.dev;

    res = rt_device_open(dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
    RT_ASSERT(res == RT_EOK);

#ifdef RT_CAN_USING_HDR
    if (info) {
        uint32_t id = (uint32_t)(info->addr << 0) + (uint32_t)(info->net_id << 8) + (uint32_t)(info->dev_id << 16);
        M_PRINTF("can filter: 0x%08x\n", id);
        struct rt_can_filter_item items[] = {
            RT_CAN_FILTER_EXT_INIT(id, NULL, NULL),
        };
        struct rt_can_filter_config cfg = {1, 1, items}; /* 一共有 1 个过滤表 */
        /* 设置硬件过滤表 */
        res = rt_device_control(dev, RT_CAN_CMD_SET_FILTER, &cfg);
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
    rt_device_t dev = can_channel.dev;
    rt_sem_t rx_sem = can_channel.sem;
    struct rt_can_msg rx_msg;

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(dev, can_rx_call);
    rt_device_control(dev, RT_CAN_CMD_SET_BAUD, (void *)CAN20kBaud);

    while (1) {
        /* hdr 值为 - 1，表示直接从 uselist 链表读取数据 */
        rx_msg.hdr_index = -1;
        /* 阻塞等待接收信号量 */
        rt_sem_take(rx_sem, RT_WAITING_FOREVER);
        rt_device_read(dev, 0, &rx_msg, sizeof(rx_msg));
        write_ring_buffer(&ring_buffer, &rx_msg);
        if (can_channel.rx_callback != NULL)
            can_channel.rx_callback(ring_buffer.buff[ring_buffer.head].len + 3, can_channel.parameter);
    }
}

static int can_channel_read(uint8_t *buff, size_t len)
{
    size_t size;
    struct rt_can_msg rx_msg;

    if (len < 3) {
        return 0;
    }

    if (read_ring_buffer(&ring_buffer, &rx_msg)) {
        return 0;
    }
    //低位在前
    buff[0] = (uint8_t)(rx_msg.id >> 16);
    buff[1] = (uint8_t)(rx_msg.id >> 8);
    buff[2] = (uint8_t)(rx_msg.id >> 0);
    for (size = 3; size < len && size < rx_msg.len + 3; size++) {
        buff[size] = rx_msg.data[size - 3];
    }

    // M_PRINTF("\n================ row data =================\n");
    // M_LOG_I("ID:%x", rx_msg.id);
    // M_PRINTF("%dbytes: ", rx_msg.len);
    // for (int i = 0; i < len - 3; i++) {
    //     M_PRINTF("0x%02x ", rx_msg.data[i]);
    // }
    // M_PRINTF("\n================ row data =================\n");
    
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
    rt_device_t dev = can_channel.dev;

    tx_msg.id = (uint32_t)(data[0] << 0) + (uint32_t)(data[1] << 8) + (uint32_t)(data[2] << 16);
    tx_msg.len = len - 3;
    memcpy(tx_msg.data, &data[3], len - 3);

    M_PRINTF("send %dbytes:\n", len - 3);
    M_PRINTF("id:0x%x\n", tx_msg.id);
    for (int i = 0; i < len - 3; i++) {
        M_PRINTF("0x%02x ", tx_msg.data[i]);
    }
    M_PRINTF("\n");

    /* 发送一帧 CAN 数据 */
    size_t size = rt_device_write(dev, 0, &tx_msg, sizeof(tx_msg));
    if (size == 0) {
        M_PRINTF("can dev write data failed!\n");
    }
    return size;
}

int can_channel_init(struct channel *channel)
{
    rt_device_t dev;
    rt_sem_t rx_sem;

    if (can_channel.dev == NULL) {
        dev = rt_device_find("can1");
        if (dev == NULL) {
            M_LOG_E("find can1 failed!\n");
            return RT_ERROR;
        }
        can_channel.dev = dev;
    }

    if (can_channel.sem == NULL) {
        rx_sem = rt_sem_create("can rx", 0, RT_IPC_FLAG_FIFO);
        if (rx_sem == NULL) {
            M_LOG_E("create can rx sem failed!\n");
            return RT_ERROR;
        }
        can_channel.sem = rx_sem;
    }

    ring_buffer_init(&ring_buffer);

    channel->open = can_channel_open;
    channel->read = can_channel_read;
    channel->write = can_channel_write;

    return RT_EOK;
}

void set_can_rx_callback(int (*rx_callback) (size_t len, void *parameter), void *parameter)
{
    can_channel.parameter = parameter;
    can_channel.rx_callback = rx_callback;
}

