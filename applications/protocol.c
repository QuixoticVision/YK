/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     RiceChen    first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include "protocol.h"


protocol m_protocol;
static uint8_t *current_protocol_set;

static uint8_t protocol_set_yk[] = {
    FUNC_CODE_YK_QUERY_STATE,
    FUNC_CODE_YK_CONTROL,
    FUNC_CODE_YK_MODIFY_SN,
    FUNC_CODE_YK_REPORT_STATE,
    FUNC_CODE_YK_PRODUCTION_MODIFY_SN,
};

static uint8_t protocol_set_avc[] = {
    FUNC_CODE_AVC_QUERY_STATE,
    FUNC_CODE_AVC_CONTROL_UPPER,
    FUNC_CODE_AVC_CONTROL_LOWER,
    FUNC_CODE_AVC_MODIFY_ADDR_TYPE,
    FUNC_CODE_AVC_MODIFY_SN,
    FUNC_CODE_AVC_PRODUCTION_QUERY,
};

static uint8_t protocol_set_cold_lock[] = {
    FUNC_CODE_COLD_LOCK_QUERY_ALL,
    FUNC_CODE_COLD_LOCK_CONTROL,
    FUNC_CODE_COLD_LOCK_MODIFY_TIMEOUT,
    FUNC_CODE_COLD_LOCK_MODIFY_ADDR,
    FUNC_CODE_COLD_LOCK_MODIFY_SN,
    FUNC_CODE_COLD_LOCK_PRODUCTION_QUERY,
};

static uint8_t get_function_code(data *frame);
static data *get_data(data *frame);


static int protocol_init(void)
{
    
}

static void protocol_analysis_code_data(data *frame, uint8_t *code, data *data)
{

}

static info *protocol_parser(data *frame)
{
    int size = ARRAY_SIZE(current_protocol_set);
    uint8_t code;
    data data;
    protocol_analysis_code_data(frame, &code, &data);
    
    for (int i = 0; i < size; i++) {
        if (code == current_protocol_set[i]) {

            break;
        }
    }
    
    return RT_EOK;
}

protocol *protocol_select(device_type dev_type)
{
    protocol *p = &m_protocol;
	switch (dev_type) {
    case YK_LOCK:
        current_protocol_set = protocol_set_yk;
        break;
    case YK_COLD_LOCK:
        current_protocol_set = protocol_set_cold_lock;
        break;
    case AVC:
        current_protocol_set = protocol_set_avc;
        break;
    case AVC_COLD_LOCK:
        current_protocol_set = protocol_set_cold_lock;
        break;
    case AVC_SWITCH:
        current_protocol_set = protocol_set_yk;
        break;
    }
    p->init = protocol_init;
    p->parser = protocol_parser;
    return p;
}




