/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2024-03-11     LYX          first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include <common.h>

#define DBG_TAG                 "cmd"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

#include "lock.h"
#include "operation.h"
#include "command.h"
#include "hw_define.h"
#include "board.h"

struct lock *lock;

static int cmd_pre_init(void)
{
    lock = get_lock_handler();
    return RT_EOK;
}
INIT_APP_EXPORT(cmd_pre_init);

static int cmd_device_modify(int argc, char *argv[])
{
    if (argc < 2) {
        M_LOG_E("missing parameters.");
        return RT_ERROR;
    }

    int type;
    sscanf(argv[1], "%d", &type);
    switch (type) {
    case YK_LOCK:
    case YK_COLD_LOCK:
    case AVC:
    case AVC_COLD_LOCK:
    case AVC_SWITCH:
        lock->ops->modify_device(type);
        break;
    default:
        break;
    }

    return RT_EOK;
}
MSH_CMD_EXPORT(cmd_device_modify, cmd_device_modify);

static int cmd_modify_device(int argc, char *argv[])
{
    if (argc < 2) {
        M_LOG_E("missing parameters.");
        return RT_ERROR;
    }

    int type;
    sscanf(argv[1], "%d", &type);
    switch (type) {
    case YK_LOCK:
    case YK_COLD_LOCK:
    case AVC:
    case AVC_COLD_LOCK:
    case AVC_SWITCH:
        lock->ops->modify_device(type);
        break;
    default:
        break;
    }

    return RT_EOK;
}
MSH_CMD_EXPORT(cmd_modify_device, cmd_modify_device);

static int cmd_modify_addr(int argc, char *argv[])
{
    if (argc == 1) {
        M_LOG_E("missing parameters.");
        return RT_ERROR;
    }

    int addr;
    sscanf(argv[1], "%d", &addr);
    lock->ops->modify_addr((uint8_t)addr);

    return RT_EOK;
}
MSH_CMD_EXPORT(cmd_modify_addr, cmd_modify_addr);

static int cmd_modify_sn(int argc, char *argv[])
{
    if (argc == 1) {
        M_LOG_E("missing parameters.");
        return RT_ERROR;
    }

    int sn;
    sscanf(argv[1], "%d", &sn);
    lock->ops->modify_sn((uint32_t)sn);

    return RT_EOK;
}
MSH_CMD_EXPORT(cmd_modify_sn, cmd_modify_sn);

static int cmd_modify_sn_random(int argc, char *argv[])
{
    srand(rt_tick_get());
    lock->ops->modify_sn((uint32_t)rand());

    return RT_EOK;
}
MSH_CMD_EXPORT(cmd_modify_sn_random, cmd_modify_sn_random);


static int cmd_led_green_control(int argc, char *argv[])
{
    if (argc == 1) {
        M_LOG_E("missing parameters.");
        return RT_ERROR;
    }

    int state;
    sscanf(argv[1], "%d", &state);
    if (state) {
        rt_pin_write(PIN_AVC_LED_CT1, PIN_LOW);
        rt_pin_write(PIN_AVC_LED_CT2, PIN_HIGH);
    } else {
        rt_pin_write(PIN_AVC_LED_CT1, PIN_LOW);
        rt_pin_write(PIN_AVC_LED_CT2, PIN_LOW);
    }
    return RT_EOK;
}
MSH_CMD_EXPORT(cmd_led_green_control, cmd_led_green_control);
