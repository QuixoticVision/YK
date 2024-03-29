/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */

#include "common.h"
#include "can_channel.h"
#include "device_init.h"

#define TEST_ON                 0
#if TEST_ON
#define DBG_TAG                 "test"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

struct {
    struct device_lock *lock;
    rt_timer_t timer;
    uint32_t count;
    uint8_t buff[12];
    size_t len;
} test;

static int test_rx_callback(size_t size)
{
    // uint8_t *buff = test.buff;
    // test.channel.read(buff, size);
    uint8_t buff[size];
    if (test.lock->channel->read(buff, size) == 0) {
        M_LOG_E("read fail");
        return 0;
    }
    memcpy(test.buff, buff, size);
    test.len = size;
    test.count++;
    rt_timer_start(test.timer);

    uint32_t frame_head = (uint32_t)(buff[0] << 0) + (uint32_t)(buff[1] << 8) + (uint32_t)(buff[2] << 16);
    M_PRINTF("0x%06x: ", frame_head);
    for (int i = 3; i < test.len; i++) {
        M_PRINTF("0x%02x ", buff[i]);
    }
    M_PRINTF("\n");
    
    return 0;
}

static void test_timeout(void *parameter)
{
    uint8_t *buff = test.buff;
    uint32_t frame_head = (uint32_t)(buff[0] << 0) + (uint32_t)(buff[1] << 8) + (uint32_t)(buff[2] << 16);
    M_PRINTF("%d 0x%06x: ", test.count, frame_head);
    for (int i = 3; i < test.len; i++) {
        M_PRINTF("0x%02x ", buff[i]);
    }
    M_PRINTF("\n");
    test.count = 0;
}

int test_init(void)
{
    test.timer = rt_timer_create("test", test_timeout, NULL, 10, RT_TIMER_FLAG_ONE_SHOT);
    test.lock = device_create();
    test.lock->channel->open(NULL);
    set_can_rx_callback(test_rx_callback);
    return 0;
}
INIT_APP_EXPORT(test_init);

/* TEST CMD */
static int test_can_send(int argc, char **argv)
{
    if (argc < 2) {
        M_LOG_E("missing parameter");
        return -1;
    }
    struct device_lock *lock = test.lock;
    uint8_t data[11];
    memset(data, 0 , 11);
    data[0] = lock->info->dev_id;
    data[1] = lock->info->net_id;
    data[2] = lock->info->addr;
    for (int i = 0; (i < argc - 1) && (i < 8); i++) {
        sscanf(argv[i + 1], "%x", &data[3]);
        M_PRINTF("0x%02x ", data[3 + i]);
    }
    M_PRINTF("\n");
    lock->channel->write(data, argc - 1 + 3);
    return 0;
}
MSH_CMD_EXPORT(test_can_send, test_can_send);

static int test_can_open(int argc, char **argv)
{
    if (argc < 4) {
        M_LOG_E("missing parameter");
        return -1;
    }
    struct device_info *info = test.lock->info;
    sscanf(argv[1], "%x", &info->dev_id);
    sscanf(argv[2], "%x", &info->net_id);
    sscanf(argv[3], "%x", &info->addr);
    test.lock->channel->open(info);
    return 0;
}
MSH_CMD_EXPORT(test_can_open, test_can_open);

#endif
