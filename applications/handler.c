/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     RiceChen    first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include "handler.h"
#include "protocol.h"

#define THREAD_HANDLER_NAME                     "handler"
#define THREAD_HANDLER_STACK_SIZE               1024
#define THREAD_HANDLER_PRIORITIY                5
#define THREAD_HANDLER_TICK                     10
static void thread_handler_entry(void *para);

static int func_yk_query_state(data *frame);
static int func_yk_control(data *frame);
static int func_yk_modify_sn(data *frame);
static int func_yk_report_state(data *frame);
static int func_yk_production_modify_sn(data *frame);

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

static protocol_handler_table protocol_handler_set[] = {
    {FUNC_CODE_YK_QUERY_STATE               , func_yk_query_state               },
    {FUNC_CODE_YK_CONTROL                   , func_yk_control                   },
    {FUNC_CODE_YK_MODIFY_SN                 , func_yk_modify_sn                 },
    {FUNC_CODE_YK_REPORT_STATE              , func_yk_report_state              },
    {FUNC_CODE_YK_PRODUCTION_MODIFY_SN      , func_yk_production_modify_sn      },
    
    {FUNC_CODE_AVC_QUERY_STATE				, func_avc_query_state              },
    {FUNC_CODE_AVC_CONTROL_UPPER            , func_avc_control_uppper           },
    {FUNC_CODE_AVC_CONTROL_LOWER            , func_avc_control_lower            },
    {FUNC_CODE_AVC_MODIFY_ADDR_TYPE			, func_avc_modify_addr_type         },
    {FUNC_CODE_AVC_MODIFY_SN                , func_avc_modify_sn                },
    {FUNC_CODE_AVC_PRODUCTION_QUERY			, func_avc_production_query         },
    
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

static int func_yk_production_modify_sn(data *frame)
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


void thread_handler_entry(void *para)
{
    while(1) {
        rt_thread_delay(1);
    }
}

static int handler_init(void)
{
    rt_thread_t thread;
    thread = rt_thread_create(
        THREAD_HANDLER_NAME,
        thread_handler_entry,
        NULL,
        THREAD_HANDLER_STACK_SIZE,
        THREAD_HANDLER_PRIORITIY,
        THREAD_HANDLER_TICK
    );
    if (thread == NULL) {
        rt_kprintf("thread %s create fail!", THREAD_HANDLER_NAME);
        return RT_ERROR;
    }
    rt_thread_startup(thread);
    return RT_EOK;
}

int handler_entry(info *info)
{
    return 0;
}
