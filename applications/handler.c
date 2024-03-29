/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>

#define DBG_TAG                 "handler"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

#include "protocol.h"
#include "handler.h"
#include "operation.h"
#include "crc8.h"
#include "hw_define.h"
#include "board.h"

static int func_yk_query_state(uint8_t *data, size_t len);
static int func_yk_control(uint8_t *data, size_t len);
static int func_yk_modify_addr(uint8_t *data, size_t len);
static int func_yk_report_state(uint8_t *data, size_t len);
static int func_yk_production_modify_sn(uint8_t *data, size_t len);

static int func_avc_query_state(uint8_t *data, size_t len);
static int func_avc_control_command(uint8_t *data, size_t len);
static int func_avc_control_request(uint8_t *data, size_t len);
static int func_avc_modify_device(uint8_t *data, size_t len);
static int func_avc_modify_sn(uint8_t *data, size_t len);
static int func_avc_production_query(uint8_t *data, size_t len);

static int func_cold_lock_query_all(uint8_t *data, size_t len);
static int func_cold_lock_control(uint8_t *data, size_t len);
static int func_cold_lock_modify_timeout(uint8_t *data, size_t len);
static int func_cold_lock_modify_addr(uint8_t *data, size_t len);
static int func_cold_lock_modify_sn(uint8_t *data, size_t len);
static int func_cold_lock_production_query(uint8_t *data, size_t len);

static handler_t handler[] = {
    func_yk_query_state,
    func_yk_control,
    func_yk_modify_addr,
    func_yk_report_state,
    func_yk_production_modify_sn,
    
    func_avc_query_state,
    func_avc_control_command,
    func_avc_control_request,
    func_avc_modify_device,
    func_avc_modify_sn,
    func_avc_production_query,
    
    func_cold_lock_query_all,
    func_cold_lock_control,
    func_cold_lock_modify_timeout,
    func_cold_lock_modify_addr,
    func_cold_lock_modify_sn,
    func_cold_lock_production_query,
};

static struct {
    struct channel *channel;
    struct lock_info *info;
    struct lock_operation *operation;
} self;

// static int write_bytes_with_crc(size_t size, ...)
// {
//     int i;
//     uint8_t data[size];
//     memset(data, 0, size);
//     va_list args;

//     va_start(args, size);
//     for (i = 3; i < size - 1; i++) {    
//         data[i] = va_arg(args, int);
//     }
//     va_end(args);

//     data[i] = crc8(data, size);
//     return self.channel->write(data, size);
// }

//YK
/* 0x01 */
static int func_yk_query_state(uint8_t *data, size_t len)
{
    uint8_t buff[11];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x06;
    buff[4] = 0x01;
    buff[5] = self.operation->get_addr();
    uint32_t sn = self.operation->get_sn();
    buff[6] = sn >> 16;
    buff[7] = sn >> 8;
    buff[8] = sn >> 0;
    buff[9] = self.operation->query_state(YK_PORT_STATE);
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x02 */
static int func_yk_control(uint8_t *data, size_t len)
{
    uint8_t port = data[5];
    uint8_t opt = data[6];
    if (opt == 0x00) {
        self.operation->control(CONTROL_YK_CLOSE, &port);
    } else {
        self.operation->control(CONTROL_YK_OPEN, &port);
    }
    uint8_t buff[8];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x03;
    buff[4] = 0x02;
    buff[5] = self.operation->get_addr();
    buff[6] = self.operation->query_state(YK_PORT_STATE);
    buff[7] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));
    
    return RT_EOK;
}

/* 0x03 */
static int func_yk_modify_addr(uint8_t *data, size_t len)
{
    uint8_t addr = data[5];
    uint32_t sn = 0;
    sn += (uint32_t)(data[6] << 16);
    sn += (uint32_t)(data[7] << 8);
    sn += (uint32_t)(data[8] << 0);

    if (sn != self.operation->get_sn()) {
        return RT_ERROR;    //SN不匹配
    }

    self.operation->modify_addr(addr);
    rt_hw_cpu_reset();

    return RT_EOK;
}

/* 0x04 */
static int func_yk_report_state(uint8_t *data, size_t len)
{
    uint8_t buff[11];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x06;
    buff[4] = 0x04;
    buff[5] = self.operation->get_addr();
    uint32_t sn = self.operation->get_sn();
    buff[6] = sn >> 16;
    buff[7] = sn >> 8;
    buff[8] = sn >> 0;
    buff[9] = self.operation->query_state(YK_PORT_STATE);
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x07 */
static int func_yk_production_modify_sn(uint8_t *data, size_t len)
{
    uint32_t sn = 0;
    sn += (uint32_t)(data[6] << 16);
    sn += (uint32_t)(data[7] << 8);
    sn += (uint32_t)(data[8] << 0);

    self.operation->modify_sn(sn);
    if (self.info->hardware_type == HW_AVC_SWITCH && self.info->lock_type != YK_LOCK) {
        self.operation->modify_device(YK_LOCK);
    }

    uint8_t buff[10];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x05;
    buff[4] = 0x07;
    buff[5] = 0x00;
    sn = self.operation->get_sn();
    buff[6] = sn >> 16;
    buff[7] = sn >> 8;
    buff[8] = sn >> 0;
    buff[9] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

//AVC
/* 0x31 */
static int func_avc_query_state(uint8_t *data, size_t len)
{
    uint8_t buff[11];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x06;
    buff[4] = 0x31;
    buff[5] = self.operation->get_addr();
    uint32_t sn = self.operation->get_sn();
    buff[6] = sn >> 16;
    buff[7] = sn >> 8;
    buff[8] = sn >> 0;
    buff[9] = self.operation->query_state(AVC_SWITCH_LED_RELAY_STATE);
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x35 */
static int func_avc_control_command(uint8_t *data, size_t len)
{
    uint8_t opt = data[5];

    if (opt == 0x10) {
        self.operation->control(CONTROL_AVC_SWITCH_ENGAGE, NULL);
    } else if (opt == 0x01) {
        self.operation->control(CONTROL_AVC_SWITCH_DISENGAGE, NULL);
    } else {
        return RT_ERROR;
    }

    uint8_t buff[10];
    memset(buff, 0, ARRAY_SIZE(buff));
    
    buff[3] = 0x05;
    buff[4] = 0x35;
    buff[5] = 0x03;
    buff[6] = self.operation->get_addr();
    buff[7] = self.operation->query_state(AVC_SWITCH_RELAY_STATE);
    buff[8] = self.operation->query_state(AVC_SWITCH_OPERATION_RESULT);
    buff[9] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x32 上位机收到请求后的返回报文 */
static int func_avc_control_request(uint8_t *data, size_t len)
{
    uint8_t answer = data[5];
    uint8_t opt = data[6];
    if (answer == 0x10) {  /* 同意请求 */
        if (opt == 0x10) {
            self.operation->control(CONTROL_AVC_SWITCH_ENGAGE, NULL);
        } else if (opt == 0x01) {
            self.operation->control(CONTROL_AVC_SWITCH_DISENGAGE, NULL);
        } else {
            M_LOG_E("FAIL: 0x10, AGREE, 0x%02x", opt);
            return RT_ERROR;
        }
    } else if (answer == 0x01) {   /* 拒绝请求 */
        M_LOG_E("FAIL: 0x01, REFUSE, 0x%02x", opt);
        return RT_ERROR;
    } else {
        return RT_ERROR;
    }

    //操作并返回操作结果
    self.operation->control(CONTROL_AVC_SWITCH_ENGAGE, NULL);
    uint8_t buff[10];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x05;
    buff[4] = 0x35; //该命令以0x35功能码返回
    buff[5] = 0x03;
    buff[6] = self.operation->get_addr();
    buff[7] = self.operation->query_state(AVC_SWITCH_RELAY_STATE);
    buff[8] = self.operation->query_state(AVC_SWITCH_OPERATION_RESULT);
    buff[9] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));    

    return RT_EOK;
}

/* 0x33 */
static int func_avc_modify_device(uint8_t *data, size_t len)
{
    uint8_t type = data[1];
    uint8_t addr = data[2];
    uint32_t sn = 0;
    sn += (uint32_t)(data[3] << 16);
    sn += (uint32_t)(data[4] << 8);
    sn += (uint32_t)(data[5] << 0);

    int result = RT_ERROR;
    if (sn == self.operation->get_sn() &&
        self.operation->modify_device(type) == RT_EOK &&
        self.operation->modify_addr(addr) == RT_EOK) {
        result = RT_EOK;//成功
    }

    uint8_t buff[9];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x04;
    buff[4] = 0x33;
    buff[5] = self.operation->get_device();
    buff[6] = self.operation->get_addr();
    buff[7] = result == RT_EOK ? 0x10 : 0x01;
    buff[8] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x37 */
static int func_avc_modify_sn(uint8_t *data, size_t len)
{
    uint32_t sn = 0;
    sn += (uint32_t)(data[6] << 16);
    sn += (uint32_t)(data[7] << 8);
    sn += (uint32_t)(data[8] << 0);

    uint8_t result;
    result = self.operation->modify_sn(sn);
    if (self.info->hardware_type == HW_AVC_SWITCH && self.info->lock_type != AVC_SWITCH) {
        self.operation->modify_device(AVC_SWITCH);
    }

    uint8_t buff[11];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x06;
    buff[4] = 0x37;
    buff[5] = self.operation->get_addr();
    sn = self.operation->get_sn();
    buff[6] = sn >> 16;
    buff[7] = sn >> 8;
    buff[8] = sn >> 0;
    buff[9] = result == RT_EOK ? 0x10 : 0x01;
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));
    return RT_EOK;
}

/* 0x3F */
static int func_avc_production_query(uint8_t *data, size_t len)
{
    uint8_t buff[11];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x06;
    buff[4] = 0x31;
    buff[5] = self.operation->get_addr();
    uint32_t sn = self.operation->get_sn();
    buff[6] = sn >> 16;
    buff[7] = sn >> 8;
    buff[8] = sn >> 0;
    buff[9] = self.operation->query_state(AVC_SWITCH_LED_RELAY_STATE);
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

//COLD-LOCK
/* 0x21 */
static int func_cold_lock_query_all(uint8_t *data, size_t len)
{
    uint8_t buff[11];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x06;
    buff[4] = 0x21;
    buff[5] = self.operation->get_addr();
    uint32_t sn = self.operation->get_sn();
    buff[6] = sn >> 16;
    buff[7] = sn >> 8;
    buff[8] = sn >> 0;
    buff[9] = self.operation->query_state(COLD_LOCK_STATE);
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x25 */
static int func_cold_lock_control(uint8_t *data, size_t len)
{
    uint8_t opt = data[5];
    if (opt == 0x00) {
        self.operation->control(CONTROL_COLD_LOCK_POWER_OFF, NULL);
    } else if (opt == 0x01) {
        self.operation->control(CONTROL_COLD_LOCK_POWER_ON, NULL);
    } else {
        //未定义
    }

    uint8_t buff[11];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x06;
    buff[4] = 0x25;
    buff[5] = self.operation->get_addr();
    uint32_t sn = self.operation->get_sn();
    buff[6] = sn >> 16;
    buff[7] = sn >> 8;
    buff[8] = sn >> 0;
    buff[9] = self.operation->query_state(COLD_LOCK_STATE);
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x2A */
static int func_cold_lock_modify_timeout(uint8_t *data, size_t len)
{
    uint8_t timeout_exec_before = self.operation->get_timeout_exec();
    uint8_t timeout_cmd_before = self.operation->get_timeout_cmd();

    self.operation->modify_timeout_exec(data[5]);
    self.operation->modify_timeout_cmd(data[6]);

    uint8_t timeout_exec_after = self.operation->get_timeout_exec();
    uint8_t timeout_cmd_after = self.operation->get_timeout_cmd();

    uint8_t buff[11];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x06;
    buff[4] = 0x2A;
    buff[5] = self.operation->get_addr();
    buff[6] = timeout_exec_before;
    buff[7] = timeout_cmd_before;
    buff[8] = timeout_exec_after;
    buff[9] = timeout_cmd_after;
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x23 */
static int func_cold_lock_modify_addr(uint8_t *data, size_t len)
{
    uint8_t addr = data[5];
    uint32_t sn = 0;
    sn += data[6] << 16;
    sn += data[7] << 8;
    sn += data[8] << 0;

    if (sn == self.operation->get_sn()) {
        self.operation->modify_addr(addr);
    } else {
        M_LOG_E("sn not match");
    }

    uint8_t buff[11];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x06;
    buff[4] = 0x23;
    buff[5] = self.operation->get_addr();
    buff[6] = sn >> 16;
    buff[7] = sn >> 8;
    buff[8] = sn >> 0;
    buff[9] = self.operation->query_state(COLD_LOCK_STATE);
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x27 */
static int func_cold_lock_modify_sn(uint8_t *data, size_t len)
{
    return RT_EOK;
}

/* 0x2F */
static int func_cold_lock_production_query(uint8_t *data, size_t len)
{
    return RT_EOK;
}

void thread_handler_entry(void *para)
{
    while(1) {
        rt_thread_delay(1);
    }
}

handler_t *get_handler_table(void)
{
    return handler;
}

int handler_init(struct lock *lock)
{
    self.channel = lock->channel;
    self.info = lock->info;
    self.operation = lock->ops;
    return RT_EOK;
}
