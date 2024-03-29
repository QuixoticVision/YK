/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */

#include <rtthread.h>
#include <rtdevice.h>

#define DBG_TAG                 "operation"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

#include "lock.h"
#include "board.h"
#include "config.h"
#include "operation.h"
#include "hw_define.h"

/**
 * @brief 
    struct lock_operation {
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
    struct config *config;
    struct lock_operation ops;
    struct {
        uint8_t is_action   : 1;
        uint8_t is_set      : 1;
        uint8_t is_save     : 1;
    } avc_state;
} self;

static int avc_relay_state_save(avc_relay_state_t avc_relay_state)
{
    uint8_t state;
    if (self.config->get(CONFIG_AVC_SWITCH_STATE, &state) != RT_EOK) return RT_ERROR;
    // rt_thread_delay(1);
    if (state == avc_relay_state) return RT_EOK;
    if (self.config->modify(CONFIG_AVC_SWITCH_STATE, &avc_relay_state) != RT_EOK) return RT_ERROR;
    for (int i = 0; i < 10; i++) {
        self.config->get(CONFIG_AVC_SWITCH_STATE, &state);
        if (state == avc_relay_state) return RT_EOK;
        rt_thread_delay(1); // 如果读出数据不对，可能是eeprom还未完全将数据写入，等待一段时间后再读出
    }
    return RT_ERROR;
}

static uint8_t query_state(query_state_t type)
{
    uint8_t state = 0;
    switch (type) {
    case YK_PORT_STATE:
        state |= YK_REALY_K3_STATE() << 7;
        state |= YK_REALY_K2_STATE() << 6;
        state |= YK_REALY_K1_STATE() << 5;
        state |= YK_RELAY_K0_STATE() << 4;
        state |= YK_INPUT_STATE();
        break;
    case AVC_SWITCH_LED_RELAY_STATE:
        state |= rt_pin_read(PIN_AVC_LED_CT1) << 5;
        state |= rt_pin_read(PIN_AVC_LED_CT2) << 4;
        state |= AVC_RELAY_CHECK() ? 0x10 : 0x01;
        break;
    case AVC_SWITCH_RELAY_STATE:
        state |= AVC_RELAY_CHECK() ? 0x10 : 0x01;
        break;
    case AVC_SWITCH_OPERATION_RESULT:
        state |= self.avc_state.is_set << 4;
        state |= self.avc_state.is_save << 1;
        state |= self.avc_state.is_action << 0;    
        break;
    case COLD_LOCK_STATE:
        break;
    default:
        break;
    }
    return state;
}

static int control(lock_control_t type, void *parameter)
{
    int i;
    int result = RT_EOK;
    uint8_t port;
    switch (type) {
    case CONTROL_YK_OPEN:
        port = *(uint8_t *)parameter;
        if (port & (1 << 0)) {
           YK_RELAY_K0_OPEN();
           for (i = 0; i < 50 && YK_RELAY_K0_STATE() != YK_REALY_STATE_OPEN; i++) {
                rt_thread_delay(1);
           }
           M_LOG_I("open time: %d ms", i);
        }
        if (port & (1 << 1)) {
            YK_RELAY_K1_OPEN();
        }
        if (port & (1 << 2)) {
            YK_RELAY_K2_OPEN();
        }
        if (port & (1 << 3)) {
            YK_RELAY_K3_OPEN();
        }
        break;
    case CONTROL_YK_CLOSE:
        port = *(uint8_t *)parameter;
        if (port & (1 << 0)) {
            YK_RELAY_K0_CLOSE();
            for (i = 0; i < 50 && YK_RELAY_K0_STATE() != YK_REALY_STATE_CLOSE; i++) {
                rt_thread_delay(1);
            }
           M_LOG_I("close time: %d ms", i);
        }
        if (port & (1 << 1)) {
            YK_RELAY_K1_CLOSE();
        }
        if (port & (1 << 2)) {
            YK_RELAY_K2_CLOSE();
        }
        if (port & (1 << 3)) {
            YK_RELAY_K3_CLOSE();
        }
        break;
    case CONTROL_AVC_SWITCH_ENGAGE:
        if (AVC_RELAY_CHECK() == AVC_RELAY_STATE_CLOSE) {
            self.avc_state.is_action = FALSE;
            self.avc_state.is_set = TRUE;
            self.avc_state.is_save = avc_relay_state_save(AVC_RELAY_STATE_CLOSE) == RT_EOK ? TRUE : FALSE;
            break;
        }
        AVC_RELAY_ENGAGE();
        if (AVC_RELAY_CHECK() == AVC_RELAY_STATE_CLOSE) {
            self.avc_state.is_action = TRUE;
            self.avc_state.is_set = TRUE;
        } else {
            self.avc_state.is_action = FALSE;
            self.avc_state.is_set = FALSE;
        }
        self.avc_state.is_save = avc_relay_state_save(AVC_RELAY_STATE_CLOSE) == RT_EOK ? TRUE : FALSE;
        break;
    case CONTROL_AVC_SWITCH_DISENGAGE:
       if (AVC_RELAY_CHECK() == AVC_RELAY_STATE_OPEN) {
            self.avc_state.is_action = FALSE;
            self.avc_state.is_set = TRUE;
            self.avc_state.is_save = avc_relay_state_save(AVC_RELAY_STATE_OPEN) == RT_EOK ? TRUE : FALSE;
            break;
        }
        AVC_RELAY_DISENGAGE();
        if (AVC_RELAY_CHECK() == AVC_RELAY_STATE_OPEN) {
            self.avc_state.is_action = TRUE;
            self.avc_state.is_set = TRUE;
        } else {
            self.avc_state.is_action = FALSE;
            self.avc_state.is_set = FALSE;
        }
        self.avc_state.is_save = avc_relay_state_save(AVC_RELAY_STATE_OPEN) == RT_EOK ? TRUE : FALSE;
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

static int modify_addr(uint8_t address)
{
    return self.config->modify(CONFIG_ADDR, &address);
}

static uint8_t get_addr(void)
{
    uint8_t addr;
    self.config->get(CONFIG_ADDR, &addr);
    return addr;
}

static int modify_sn(uint32_t sn)
{
    return self.config->modify(CONFIG_SN, &sn);
}

static uint32_t get_sn(void)
{
    uint32_t sn;
    self.config->get(CONFIG_SN, &sn);
    return sn;
}

static int modify_device(uint8_t type)
{
    return self.config->modify(CONFIG_DEVICE_TYPE, &type);
}

static lock_type_t get_device(void)
{
    uint8_t dev_type;
    self.config->get(CONFIG_DEVICE_TYPE, &dev_type);
    if (dev_type > DEVICE_TYPE_MAX) dev_type = DEFAULT_DEVICE_TYPE;
    return (lock_type_t)dev_type;
}

static int modify_timeout_exec(uint8_t timeout)
{
    return self.config->modify(CONFIG_TIMEOUT_EXEC, &timeout);
}

static uint8_t get_timeout_exec(void)
{
    uint8_t timeout;
    self.config->get(CONFIG_TIMEOUT_EXEC, &timeout);
    return timeout;
}

static int modify_timeout_cmd(uint8_t timeout)
{
    return self.config->modify(CONFIG_TIMEOUT_CMD, &timeout);
}

static uint8_t get_timeout_cmd(void)
{
    uint8_t timeout;
    self.config->get(CONFIG_TIMEOUT_CMD, &timeout);
    return timeout;
}

struct lock_operation *get_lock_operation(void)
{
    return &self.ops;
}

static int lock_operations_init(void)
{
    self.config = get_config_handler();

    self.ops.query_state = query_state;
    self.ops.control = control;
    self.ops.modify_addr = modify_addr;
    self.ops.get_addr = get_addr;
    self.ops.modify_sn = modify_sn;
    self.ops.get_sn = get_sn;
    self.ops.modify_device = modify_device;
    self.ops.get_device = get_device;
    self.ops.modify_timeout_exec = modify_timeout_exec;
    self.ops.get_timeout_exec = get_timeout_exec;
    self.ops.modify_timeout_cmd = modify_timeout_cmd;
    self.ops.get_timeout_cmd = get_timeout_cmd;
    
    return RT_EOK;
}
INIT_ENV_EXPORT(lock_operations_init);