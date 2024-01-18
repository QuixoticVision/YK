/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     RiceChen    first edition
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "channel_can.h"

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

static struct channel_can_ops can_ops;
static struct channel_can channel_can;

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
    rt_sem_release(channel_can.sem);
    M_PRINTF("can rx interrupt, size: %dbytes\n", size);
    return RT_EOK;
}

static int can_channel_init(struct device_lock *dev_lock)
{
    rt_err_t res;
    rt_thread_t thread;
    rt_device_t dev;
    rt_sem_t rx_sem;

    dev = rt_device_find("can1");
    if (dev == NULL) {
        M_LOG_E("find can1 failed!\n");
        return RT_ERROR;
    }
    channel_can.dev = dev;

    rx_sem = rt_sem_create("rx sem", 0, RT_IPC_FLAG_FIFO);
    if (rx_sem == NULL) {
        M_LOG_E("create can rx sem failed!\n");
        return RT_ERROR;
    }
    channel_can.sem = rx_sem;

    ring_buffer_init(&ring_buffer);

    res = rt_device_open(dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
    RT_ASSERT(res == RT_EOK);

    thread = rt_thread_create(
        THREAD_CAN_RX_NAME,
        thread_can_rx,
        &dev_lock->info,
        THREAD_CAN_RX_STACK_SIZE,
        THREAD_CAN_RX_PRIORITIY,
        THREAD_CAN_RX_TICK);
    if (thread != RT_NULL) {
        rt_thread_startup(thread);
    } else {
        M_LOG_E("create can_rx thread failed!\n");
    }

    return RT_EOK;
}

static void thread_can_rx(void *parameter)
{
    int i, res;
    
    rt_device_t dev = channel_can.dev;
    rt_sem_t rx_sem = channel_can.sem;
    struct rt_can_msg rx_msg;

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(dev, can_rx_call);
    rt_device_control(dev, RT_CAN_CMD_SET_BAUD, (void *)CAN20kBaud);

#ifdef RT_CAN_USING_HDR
    // struct rt_can_filter_item items[5] = {
    //         RT_CAN_FILTER_ITEM_INIT(0x100, 0, 0, 0, 0x700, RT_NULL, RT_NULL), /* std,match ID:0x100~0x1ff，hdr 为 - 1，设置默认过滤表 */
    //         RT_CAN_FILTER_ITEM_INIT(0x300, 0, 0, 0, 0x700, RT_NULL, RT_NULL), /* std,match ID:0x300~0x3ff，hdr 为 - 1 */
    //         RT_CAN_FILTER_ITEM_INIT(0x211, 0, 0, 0, 0x7ff, RT_NULL, RT_NULL), /* std,match ID:0x211，hdr 为 - 1 */
    //         RT_CAN_FILTER_STD_INIT(0x486, RT_NULL, RT_NULL),                  /* std,match ID:0x486，hdr 为 - 1 */
    //         {
    //             0x555,
    //             0,
    //             0,
    //             0,
    //             0x7ff,
    //             7,
    //         } /* std,match ID:0x555，hdr 为 7，指定设置 7 号过滤表 */
    //     };
    struct device_info *info = parameter;
    if (info) {
        uint16_t id = info->id + info->addr << 16;
        struct rt_can_filter_item items[] = {
            RT_CAN_FILTER_EXT_INIT(0x010203, NULL, NULL),
        };
        struct rt_can_filter_config cfg = {1, 1, items}; /* 一共有 5 个过滤表 */
        /* 设置硬件过滤表 */
        res = rt_device_control(dev, RT_CAN_CMD_SET_FILTER, &cfg);
        RT_ASSERT(res == RT_EOK);
    }
#endif
    while (1) {
        /* hdr 值为 - 1，表示直接从 uselist 链表读取数据 */
        rx_msg.hdr_index = -1;
        /* 阻塞等待接收信号量 */
        rt_sem_take(rx_sem, RT_WAITING_FOREVER);
        rt_device_read(dev, 0, &rx_msg, sizeof(rx_msg));
        write_ring_buffer(&ring_buffer, &rx_msg);
        channel_can.rx_callback(ring_buffer.buff[ring_buffer.head].len + 3);
    }
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
    rt_device_t dev = channel_can.dev;

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

static int can_channel_read(uint8_t *buff, uint8_t len)
{
    size_t size;
    rt_device_t dev = channel_can.dev;
    struct rt_can_msg rx_msg;

    if (len < 3) {
        return 0;
    }

    read_ring_buffer(&ring_buffer, &rx_msg);
    buff[0] = (uint8_t)(rx_msg.id >> 0);
    buff[1] = (uint8_t)(rx_msg.id >> 8);
    buff[2] = (uint8_t)(rx_msg.id >> 16);
    for (size = 0; size < len - 3 && size < rx_msg.len; size++) {
        buff[3 + size] = rx_msg.data[size];
    }

    M_LOG_I("ID:%x", rx_msg.id);
    M_PRINTF("%dbytes: ", rx_msg.len);
    for (int i = 0; i < len - 3; i++) {
        M_PRINTF("0x%02x ", rx_msg.data[i]);
    }
    M_PRINTF("\n");
    return size;
}

static int can_ops_init(void)
{
    memset(&channel_can, 0, sizeof(channel_can));
    can_ops.init = can_channel_init;
    can_ops.write = can_channel_write;
    can_ops.read = can_channel_read;

    return RT_EOK;
}
INIT_ENV_EXPORT(can_ops_init);

void set_can_rx_callback(int (*rx_callback) (size_t size))
{
    channel_can.rx_callback = rx_callback;
}

struct channel_can_ops *get_can_ops_handler(void)
{
    return &can_ops;
}
