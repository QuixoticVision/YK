/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>

#define DBG_TAG                 "protocol"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

#include "protocol.h"
#include "handler.h"
#include "crc8.h"

static struct {
    uint8_t size;
    struct lock_info *info;
    struct handler_index_table *set;
    handler_t *handler_table;
} self;

static struct handler_index_table protocol_set_yk[] = {
    {FUNC_CODE_YK_QUERY_STATE, INDEX_YK_QUERY_STATE},
    {FUNC_CODE_YK_CONTROL, INDEX_YK_CONTROL},
    {FUNC_CODE_YK_MODIFY_ADDR, INDEX_YK_MODIFY_ADDR},
    {FUNC_CODE_YK_REPORT_STATE, INDEX_YK_REPORT_STATE},
    {FUNC_CODE_YK_PRODUCTION_MODIFY_SN, INDEX_YK_PRODUCTION_MODIFY_SN},

    {FUNC_CODE_AVC_MODIFY_SN, INDEX_AVC_MODIFY_SN},
};

static struct handler_index_table protocol_set_avc_switch[] = {
    {FUNC_CODE_AVC_QUERY_STATE, IDNEX_AVC_QUERY_STATE},
    {FUNC_CODE_AVC_CONTROL_COMMAND, INDEX_AVC_CONTROL_COMMAND},
    {FUNC_CODE_AVC_CONTROL_REQUEST, INDEX_AVC_CONTROL_REQUEST},
    {FUNC_CODE_AVC_MODIFY_DEVICE, INDEX_AVC_MODIFY_ADDR_TYPE},
    {FUNC_CODE_AVC_MODIFY_SN, INDEX_AVC_MODIFY_SN},
    {FUNC_CODE_AVC_PRODUCTION_QUERY, INDEX_AVC_PRODUCTION_QUERY},

    {FUNC_CODE_YK_PRODUCTION_MODIFY_SN, INDEX_YK_PRODUCTION_MODIFY_SN},
};

static struct handler_index_table protocol_set_cold_lock[] = {
    {FUNC_CODE_COLD_LOCK_QUERY_ALL, INDEX_COLD_LOCK_QUERY_ALL},
    {FUNC_CODE_COLD_LOCK_CONTROL, INDEX_COLD_LOCK_CONTROL},
    {FUNC_CODE_COLD_LOCK_MODIFY_TIMEOUT, INDEX_COLD_LOCK_MODIFY_TIMEOUT},
    {FUNC_CODE_COLD_LOCK_MODIFY_ADDR, INDEX_COLD_LOCK_MODIFY_ADDR},
    {FUNC_CODE_COLD_LOCK_MODIFY_SN, INDEX_COLD_LOCK_MODIFY_SN},
    {FUNC_CODE_COLD_LOCK_PRODUCTION_QUERY, INDEX_COLD_LOCK_PRODUCTION_QUERY},
};

static int protocol_parser(struct parsed_data *buff, uint8_t *data, size_t len)
{
    if (data[len - 1] != crc8(data, len - 1)) {
        /* CRC 校验错误 */
        M_LOG_E("crc error, should be 0x%02x", crc8(data, len - 1));
        return RT_ERROR;
    }
    if (data[3] != len - 5) {
        /* 报文中的长度信息与实际的数据长度不符 */
        M_LOG_E("length error");
        return RT_ERROR;
    }

    uint8_t dev_id = data[0];
    uint8_t addr = data[2];
    uint8_t func_code = data[4];

    if (self.info->hardware_type == HW_AVC_SWITCH) {
        if (dev_id != self.info->dev_id &&
            func_code != FUNC_CODE_AVC_MODIFY_SN &&
            func_code != FUNC_CODE_YK_PRODUCTION_MODIFY_SN) {
            return RT_ERROR;
        }
    }
    
    for (int i = 0; i < self.size; i++) {
        if (func_code == self.set[i].func_code) {
            buff->index = self.set[i].index;
            buff->data = data;   /* 数据从第五字节开始，前四字节：3字节帧头 + 1字节长度信息 */
            buff->len = len;     /* 长度信息 */
            return RT_EOK;
        }
    }

    return RT_ERROR;
}

static int protocol_handler(const struct parsed_data *data)
{
    if (data == NULL) {
        return RT_ERROR;
    }
    int (*handler) (uint8_t *data, size_t len);
    handler = self.handler_table[data->index];
    return handler(data->data, data->len);
}

int protocol_init(struct lock *lock)
{
    lock_type_t lock_type = lock->info->lock_type;
    switch (lock_type) {
    case YK_LOCK:
        self.set = protocol_set_yk;
        self.size = ARRAY_SIZE(protocol_set_yk);
        break;
    case YK_COLD_LOCK:
        self.set = protocol_set_cold_lock;
        self.size = ARRAY_SIZE(protocol_set_cold_lock);
        break;
    case AVC:
        self.set = protocol_set_yk;
        self.size = ARRAY_SIZE(protocol_set_yk);
        break;
    case AVC_COLD_LOCK:
        self.set = protocol_set_cold_lock;
        self.size = ARRAY_SIZE(protocol_set_cold_lock);
        break;
    case AVC_SWITCH:
        self.set = protocol_set_avc_switch;
        self.size = ARRAY_SIZE(protocol_set_avc_switch);
        break;
    default:
        return RT_ERROR;
    }
    self.info = lock->info;
    self.handler_table = get_handler_table();
    handler_init(lock);
    lock->protocol->parser = protocol_parser;
    lock->protocol->handler = protocol_handler;
    return RT_EOK;
}




