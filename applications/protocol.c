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

static void rx_thread(void *para);
#define THREAD_RX_STACK_SIZE                1024
#define THREAD_RX_PRIORITIY                 5
#define THREAD_RX_TICK                      10
static protocol *current_protocol_set;


static int func_yk_query_state(data *frame);
static int func_yk_control(data *frame);
static int func_yk_modify_sn(data *frame);
static int func_yk_report_state(data *frame);
static int func_yk_production_modify_sn();

static int func_avc_query_state(data *frame);
static int func_avc_control_uppper(data *frame);
static int func_avc_control_lower(data *frame);
static int func_avc_modify_addr_type(data *frame);
static int func_avc_modify_sn(data *frame);
static int func_avc_production_query(data *frame);

static int func_cold_lock_query_all(data *frame);
static int func_cold_lock_control(data *frame);
static int func_cold_lock_modify_timeout(data *frame);
static int func_cold_lock_modify_addr(data *frame);
static int func_cold_lock_modify_sn(data *frame);
static int func_cold_lock_production_query(data *frame);

static protocol protocol_set_yk[] = {
    {FUNC_CODE_YK_QUERY_STATE               , func_yk_query_state               },
    {FUNC_CODE_YK_CONTROL                   , func_yk_control                   },
    {FUNC_CODE_YK_MODIFY_SN                 , func_yk_modify_sn                 },
    {FUNC_CODE_YK_REPORT_STATE              , func_yk_report_state              },
    {FUNC_CODE_YK_PRODUCTION_MODIFY_SN      , func_yk_production_modify_sn      },
};
static protocol protocol_set_avc[] = {
    {FUNC_CODE_AVC_QUERY_STATE				, func_avc_query_state              },
    {FUNC_CODE_AVC_CONTROL_UPPER            , func_avc_control_uppper           },
    {FUNC_CODE_AVC_CONTROL_LOWER            , func_avc_control_lower            },
    {FUNC_CODE_AVC_MODIFY_ADDR_TYPE			, func_avc_modify_addr_type         },
    {FUNC_CODE_AVC_MODIFY_SN                , func_avc_modify_sn                },
    {FUNC_CODE_AVC_PRODUCTION_QUERY			, func_avc_production_query         },
};
static protocol protocol_set_cold_lock[] = {
    {FUNC_CODE_COLD_LOCK_QUERY_ALL			, func_cold_lock_query_all          },
    {FUNC_CODE_COLD_LOCK_CONTROL            , func_cold_lock_control            },
    {FUNC_CODE_COLD_LOCK_MODIFY_TIMEOUT		, func_cold_lock_modify_timeout     },
    {FUNC_CODE_COLD_LOCK_MODIFY_ADDR		, func_cold_lock_modify_addr        },
    {FUNC_CODE_COLD_LOCK_MODIFY_SN			, func_cold_lock_modify_sn          },
    {FUNC_CODE_COLD_LOCK_PRODUCTION_QUERY	, func_cold_lock_production_query   },
};



//YK
static int func_yk_query_state(data *frame)
{
    return RT_EOK;
}

static int func_yk_control(data *frame)
{
    return RT_EOK;
}

static int func_yk_modify_sn(data *frame)
{
    return RT_EOK;
}

static int func_yk_report_state(data *frame)
{
    return RT_EOK;
}

static int func_yk_production_modify_sn()
{
    return RT_EOK;
}

//AVC
static int func_avc_query_state(data *frame)
{
    return RT_EOK;
}

static int func_avc_control_uppper(data *frame)
{
    return RT_EOK;
}

static int func_avc_control_lower(data *frame)
{
    return RT_EOK;
}

static int func_avc_modify_addr_type(data *frame)
{
    return RT_EOK;
}

static int func_avc_modify_sn(data *frame)
{
    return RT_EOK;
}

static int func_avc_production_query(data *frame)
{
    return RT_EOK;
}

//COLD-LOCK
static int func_cold_lock_query_all(data *frame)
{
    return RT_EOK;
}

static int func_cold_lock_control(data *frame)
{
    return RT_EOK;
}

static int func_cold_lock_modify_timeout(data *frame)
{
    return RT_EOK;
}

static int func_cold_lock_modify_addr(data *frame)
{
    return RT_EOK;
}

static int func_cold_lock_modify_sn(data *frame)
{
    return RT_EOK;
}

static int func_cold_lock_production_query(data *frame)
{
    return RT_EOK;
}

static uint8_t get_function_code(data *frame)
{
    
    return 0;
}

protocol *protocol_select(device_type type)
{
	switch (type) {
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
    return current_protocol_set;
}

int protocol_handler(data *frame)
{
    int res = -1;
    uint8_t func_code = get_function_code(frame);
    int protocol_set_max = ARRAY_SIZE(current_protocol_set);
    for (int i = 0; i < protocol_set_max; i++) {
        if (current_protocol_set[i].function_code == func_code) {
            res = current_protocol_set[i].handler(frame);
        }
    }
    return res;
}
