/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "operations.h"
#include "device_init.h"

/**
 * @brief 
    struct lock_operations {
        uint8_t (*query_state) (query_state_t);
        uint8_t (*control) (lock_control_t);
        void (*modify_addr) (uint8_t address);
        uint8_t (*get_addr) (void);
        uint8_t (*modify_sn) (uint32_t sn);
        uint32_t (*get_sn) (void);
        void (*modify_device) (uint8_t type);
        uint8_t (*get_device) (void);
        uint8_t (*set_timeout) (uint8_t exec_timeout, uint8_t cmd_timeout);
        uint8_t (*get_timeout) (timeout_t);
    };
 * 
 */

static struct {
    uint8_t *address;
    uint32_t *sn;
    uint8_t *timeout_exec;
    uint8_t *timeout_cmd;
    device_type_t dev_type;
    struct lock_operations ops;
} self;

static uint8_t query_state(query_state_t type)
{
    uint8_t state;
    switch (type) {
    case YK_PORT_STATE:
        break;
    case AVC_SWITCH_LED_RELAY_STATE:
        break;
    case AVC_SWITCH_RELAY_STATE:
        break;
    case AVC_SWITCH_OPERATION_RESULT:
        break;
    case COLD_LOCK_STATE:
        break;
    default:
        break;
    }
    return state;
}

static uint8_t control (lock_control_t type)
{
    uint8_t result = RT_EOK;
    switch (type) {
    case CONTROL_YK_OPEN:
        break;
    case CONTROL_YK_CLOSE:
        break;
    case CONTROL_AVC_SWITCH_ENGAGE:
        break;
    case CONTROL_AVC_SWITCH_DISENGAGE:
        break;
    case CONTROL_COLD_LOCK_POWER_ON:
        break;
    case CONTROL_COLD_LOCK_POWER_OFF:
        break;
    default:
        break;
    }
    return result;
}

static uint8_t modify_addr(uint8_t address)
{
    *self.address = address;
    return RT_EOK;
}

static uint8_t get_addr(void)
{
    return *self.address;
}

static uint8_t modify_sn(uint32_t sn)
{
    *self.sn = sn;
    return RT_EOK;
}

static uint32_t get_sn(void)
{
    return *self.sn;
}

static uint8_t modify_device(uint8_t type)
{
    return 0xff;
}

static uint8_t get_device(void)
{
    return 0xff;
}

static uint8_t set_timeout(uint8_t timeout_exec, uint8_t timeout_cmd)
{
    *self.timeout_exec = timeout_exec;
    *self.timeout_cmd = timeout_cmd;
    return RT_EOK;
}

static uint8_t get_timeout(timeout_t type)
{
    uint8_t timeout;
    switch (type) {
    case TIMEOUT_COLD_LOCK_EXEC:
        timeout = *self.timeout_exec;
        break;
    case TIMEOUT_COLD_LOCK_CMD:
        timeout = *self.timeout_cmd;
        break;
    }
    return timeout;
}

struct lock_operations *get_lock_operations(void)
{
    return &self.ops;
}

int lock_operations_init(struct device_lock *lock)
{
    self.address = &lock->info->addr;
    self.sn = &lock->info->sn;

    self.ops.query_state = query_state;
    self.ops.control = control;
    self.ops.modify_addr = modify_addr;
    self.ops.get_addr = get_addr;
    self.ops.modify_sn = modify_sn;
    self.ops.get_sn = get_sn;
    self.ops.modify_device = modify_device;
    self.ops.get_device = get_device;
    self.ops.set_timeout = set_timeout;
    self.ops.get_timeout = get_timeout;
    
    return RT_EOK;
}