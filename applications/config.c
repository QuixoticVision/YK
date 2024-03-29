/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include <common.h>

#define DBG_TAG                 "config"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

#include "config.h"
#include "at24cxx.h"
#include "lock.h"

#define I2C_BUS             "i2c1"
#define I2C_ADDR            (0xA0 >> 1)

#define CONFIG_BEGIN_ADDR   0

struct info {
    union {
        uint8_t avc_relay_state;
    } device_state;
    uint8_t dev_type;
    uint8_t timeout_exec;
    uint8_t timeout_cmd;
    uint32_t addr : 8;
    uint32_t sn : 24;
};

static struct {
    struct {
        struct info info;
        struct config ops;
    } config;
    at24cxx_device_t dev;
} self;

static int config_get(config_t type, void *parameter)
{
    struct info *info = &self.config.info;
    if (at24cxx_read(self.dev, CONFIG_BEGIN_ADDR, (uint8_t *)info, sizeof(struct info)) != RT_EOK) return RT_ERROR;
    if (parameter == NULL) return RT_ERROR;
    switch (type) {
    case CONFIG_DEVICE_TYPE:
        *(uint8_t *)parameter = info->dev_type;
        break;
    case CONFIG_ADDR:
        *(uint8_t *)parameter = info->addr;
        break;
    case CONFIG_SN:
        *(uint32_t *)parameter = info->sn;
        break;
    case CONFIG_TIMEOUT_EXEC:
        *(uint8_t *)parameter = info->timeout_exec;
        break;
    case CONFIG_TIMEOUT_CMD:
        *(uint8_t *)parameter = info->timeout_cmd;
        break;
    case CONFIG_AVC_SWITCH_STATE:
        *(uint8_t *)parameter = info->device_state.avc_relay_state;
        break;
    default:
        return RT_ERROR;
    }
    return RT_EOK;
}

static int config_modify(config_t type, void *parameter)
{
    struct info info = self.config.info;
    switch (type) {
    case CONFIG_DEVICE_TYPE:
        info.dev_type = *(uint8_t *)parameter;
        break;
    case CONFIG_ADDR:
        info.addr = *(uint8_t *)parameter;
        break;
    case CONFIG_SN:
        info.sn = *(uint32_t *)parameter;
        break;
    case CONFIG_TIMEOUT_EXEC:
        info.timeout_exec = *(uint8_t *)parameter;
        break;
    case CONFIG_TIMEOUT_CMD:
        info.timeout_cmd = *(uint8_t *)parameter;
        break;
    case CONFIG_AVC_SWITCH_STATE:
        info.device_state.avc_relay_state = *(uint8_t *)parameter;
        break;
    default:
        return RT_ERROR;
    }
    if (at24cxx_write(self.dev, CONFIG_BEGIN_ADDR, (uint8_t *)&info, sizeof(info)) == RT_EOK) {
        self.config.info = info;
        lock_info_update();
        return RT_EOK;
    }
    return RT_ERROR;
}

static int config_pre_init(void)
{
    self.config.ops.get = config_get;
    self.config.ops.modify = config_modify;

    return RT_EOK;
}
INIT_ENV_EXPORT(config_pre_init);

struct config *get_config_handler(void)
{
    return &self.config.ops;
}

static int config_init(void)
{
    struct info info;
    self.dev = at24cxx_init(I2C_BUS, I2C_ADDR);
    at24cxx_read(self.dev, CONFIG_BEGIN_ADDR, (uint8_t *)&info, sizeof(info));
    self.config.info.addr = info.addr ? info.addr : 0x01;
    self.config.info.sn = info.sn ? info.sn : 0x000001;
    self.config.info.dev_type = info.dev_type < DEVICE_TYPE_MAX ? info.dev_type : DEFAULT_DEVICE_TYPE;
    char *dev_t[] = {"YK", "COLD_YK", "AVC", "COLD_AVC", "AVC_SWITCH", "YK_AVC_SWITCH"};
    M_LOG_I("DEVICE_TYPE: %s", dev_t[self.config.info.dev_type]);
    M_LOG_I("ADDRESS: 0x%02X", self.config.info.addr);
    M_LOG_I("SN: %02X %02X %02X",
            (uint8_t)(self.config.info.sn >> 16),
            (uint8_t)(self.config.info.sn >> 8),
            (uint8_t)(self.config.info.sn >> 0));
    M_LOG_I("AVC STATE: %d", self.config.info.device_state.avc_relay_state);

    return RT_EOK;
}
INIT_APP_EXPORT(config_init);
