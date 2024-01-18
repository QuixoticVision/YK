/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     RiceChen    first edition
 */

#include "common.h"
#include "channel_can.h"

#define DBG_TAG                 "test"
#define DBG_LVL                 DBG_LOG

struct test {
    int (*init) (struct device_lock *dev_lock);
    int (*write) (uint8_t *data, size_t len);
    int (*read) (uint8_t *buff, size_t len);
};

struct test test;

static int test_rx_callback(size_t size)
{
    M_PRINTF("test rx cb: %dbytes\n", size);
    uint8_t buff[size];
    test.read(buff, size);
    return 0;
}

static int test_can_send(int argc, char **argv)
{
    if (argc < 2) {
        M_LOG_E("missing parameter");
        return -1;
    }
    uint8_t data[11];
    memset(data, 0 , 11);
    data[0] = 0x01;
    data[1] = 0x02;
    data[2] = 0x03;
    for (int i = 0; (i < argc - 1) && (i < 8); i++) {
        sscanf(argv[i + 1], "%x", &data[3]);
        M_PRINTF("0x%02x ", data[3 + i]);
    }
    M_PRINTF("\n");
    test.write(data, argc - 1 + 3);
}
MSH_CMD_EXPORT(test_can_send, test_can_send);

int test_init(void)
{
    struct channel_can_ops *ops = get_can_ops_handler();

    test.init = ops->init;
    if (test.init == NULL)  {
        LOG_E("test: set write error");
    }
    test.read = ops->read;
    if (test.read == NULL) {
        LOG_E("test: set read error");
    }
    test.write = ops->write;
    if (test.write == NULL)  {
        LOG_E("test: set write error");
    }

    struct device_lock lock;
    lock.info.addr = 0x01;
    test.init(&lock);
    set_can_rx_callback(test_rx_callback);
}
INIT_APP_EXPORT(test_init);
