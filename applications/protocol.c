/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include "protocol.h"
#include "handler.h"
#include "crc8.h"

#define DBG_TAG                 "protocol"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

static struct {
    struct handler_index_table *set;
    handler_t *handler_table;
    channel_type channel_type;
} current_protocol;

static struct handler_index_table protocol_set_yk[] = {
    {FUNC_CODE_YK_QUERY_STATE, INDEX_YK_QUERY_STATE},
    {FUNC_CODE_YK_CONTROL, INDEX_YK_CONTROL},
    {FUNC_CODE_YK_MODIFY_ADDR, INDEX_YK_MODIFY_ADDR},
    {FUNC_CODE_YK_REPORT_STATE, INDEX_YK_REPORT_STATE},
    {FUNC_CODE_YK_PRODUCTION_MODIFY_SN, INDEX_YK_PRODUCTION_MODIFY_SN},
};

static struct handler_index_table protocol_set_avc[] = {
    {FUNC_CODE_AVC_QUERY_STATE, IDNEX_AVC_QUERY_STATE},
    {FUNC_CODE_AVC_CONTROL_ENGAGE, INDEX_AVC_CONTROL_UPPER},
    {FUNC_CODE_AVC_CONTROL_DISENGAGE, INDEX_AVC_CONTROL_LOWER},
    {FUNC_CODE_AVC_MODIFY_DEVICE, INDEX_AVC_MODIFY_ADDR_TYPE},
    {FUNC_CODE_AVC_MODIFY_SN, INDEX_AVC_MODIFY_SN},
    {FUNC_CODE_AVC_PRODUCTION_QUERY, INDEX_AVC_PRODUCTION_QUERY},
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

    if (current_protocol.channel_type != CHANNEL_HW_USING_CAN) {
        //如果不是CAN通讯，需要判断帧头是否匹配
    }
    
    int size = ARRAY_SIZE(current_protocol.set);
    uint8_t code = data[4];
    for (int i = 0; i < size; i++) {
        if (code == current_protocol.set[i].func_code) {
            buff->index = i;
            buff->data = &data[4];   /* 数据从第五字节开始，前四字节：3字节帧头 + 1字节长度信息 */
            buff->len = data[3];     /* 长度信息 */
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
    handler = current_protocol.handler_table[data->index];
    return handler(data->data, data->len);
}

int protocol_init(struct protocol *protocol, struct channel *channel, device_type dev_type, channel_type ch_type)
{
	switch (dev_type) {
    case YK_LOCK:
        current_protocol.set = protocol_set_yk;
        break;
    case YK_COLD_LOCK:
        current_protocol.set = protocol_set_cold_lock;
        break;
    case AVC:
        current_protocol.set = protocol_set_avc;
        break;
    case AVC_COLD_LOCK:
        current_protocol.set = protocol_set_cold_lock;
        break;
    case AVC_SWITCH:
        current_protocol.set = protocol_set_yk;
        break;
    default:
        return RT_ERROR;
    }
    current_protocol.channel_type = ch_type;
    current_protocol.handler_table = get_handler_table();
    handler_init(channel);
    protocol->parser = protocol_parser;
    protocol->handler = protocol_handler;
    return RT_EOK;
}




