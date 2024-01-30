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
#include "operations.h"
#include "crc8.h"

#define DBG_TAG                 "handler"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

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
    struct lock_operations *operation;
} self;

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
    if (data[2] == 0x01) {
        self.operation->control(CONTROL_YK_OPEN);
    } else {
        self.operation->control(CONTROL_YK_CLOSE);
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
    uint8_t addr = self.operation->get_addr();
    uint32_t sn = self.operation->get_sn();

    if (addr != data[1]) {
        return RT_ERROR;    //地址不匹配
    }

    if (sn != ((uint32_t)(data[2] << 16) + (uint32_t)(data[1] << 8) + (uint32_t)(data[0] << 0))) {
        return RT_ERROR;    //SN不匹配
    }

    self.operation->modify_addr(data[1]);

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

static int func_yk_production_modify_sn(uint8_t *data, size_t len)
{
    return RT_EOK;
}

//AVC
/* 0x31 */
static int func_avc_query_state(uint8_t *data, size_t len)
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
    buff[9] = self.operation->query_state(AVC_SWITCH_LED_RELAY_STATE);
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x35 */
static int func_avc_control_command(uint8_t *data, size_t len)
{
    self.operation->control(CONTROL_AVC_SWITCH_ENGAGE);
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
    if (data[1] == 0x10) {  /* 同意请求 */
        if (data[2] == 0x10) {
            self.operation->control(CONTROL_AVC_SWITCH_ENGAGE);
        } else if (data[2] == 0x01) {
            self.operation->control(CONTROL_AVC_SWITCH_DISENGAGE);
        } else {
            M_LOG_E("FAIL: 0x32, AGREE, 0x%02x", data[2]);
            return RT_ERROR;
        }
    } else if (data[1] == 0x01) {   /* 拒绝请求 */
        M_LOG_E("FAIL: 0x32, REFUSE, 0x%02x", data[2]);
        return RT_ERROR;
    } else {
        return RT_ERROR;
    }

    //操作完返回操作结果
    self.operation->control(CONTROL_AVC_SWITCH_ENGAGE);
    uint8_t buff[10];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x05;
    buff[4] = 0x32;
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

    uint8_t result;
    if (sn != self.operation->get_sn()) {
        result = 0x01;//失败
    } else {
        self.operation->modify_device(type);
        self.operation->modify_addr(addr);
        result = 0x10;//成功
    }

    uint8_t buff[9];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x04;
    buff[4] = 0x33;
    buff[5] = self.operation->get_device();
    buff[6] = self.operation->get_addr();
    buff[7] = result;
    buff[8] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x37 */
static int func_avc_modify_sn(uint8_t *data, size_t len)
{
    uint32_t sn = 0;
    sn += (uint32_t)(data[3] << 16);
    sn += (uint32_t)(data[4] << 8);
    sn += (uint32_t)(data[5] << 0);

    uint8_t result;
    result = self.operation->modify_sn(sn);

    uint8_t buff[11];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x06;
    buff[4] = 0x37;
    buff[5] = self.operation->get_addr();
    sn = self.operation->get_sn();
    buff[6] = sn >> 16;
    buff[7] = sn >> 8;
    buff[8] = sn >> 0;
    buff[9] = result ? 0x10 : 0x01;
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));
    return RT_EOK;
}

static int func_avc_production_query(uint8_t *data, size_t len)
{
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
    if (data[1] == 0x00) {
        self.operation->control(CONTROL_COLD_LOCK_POWER_OFF);
    } else if (data[1] == 0x01) {
        self.operation->control(CONTROL_COLD_LOCK_POWER_ON);
    } else {
        return RT_ERROR;
    }

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

/* 0x2A */
static int func_cold_lock_modify_timeout(uint8_t *data, size_t len)
{
    uint8_t timeout_before1 = self.operation->get_timeout(TIMEOUT_COLD_LOCK_EXEC);
    uint8_t timeout_before2 = self.operation->get_timeout(TIMEOUT_COLD_LOCK_CMD);

    self.operation->set_timeout(data[1], data[2]);

    uint8_t timeout_after1 = self.operation->get_timeout(TIMEOUT_COLD_LOCK_EXEC);
    uint8_t timeout_after2 = self.operation->get_timeout(TIMEOUT_COLD_LOCK_CMD);

    uint8_t buff[11];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x06;
    buff[4] = 0x2A;
    buff[5] = self.operation->get_addr();
    buff[6] = timeout_before1;
    buff[7] = timeout_before2;
    buff[8] = timeout_after1;
    buff[9] = timeout_after2;
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x23 */
static int func_cold_lock_modify_addr(uint8_t *data, size_t len)
{
    uint32_t sn = 0;
    sn += data[2] << 16;
    sn += data[3] << 8;
    sn += data[4] << 0;

    if (sn != self.operation->get_sn()) {
        M_LOG_E("sn not match");
        return RT_ERROR;
    }

    self.operation->modify_addr(data[1]);

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

int handler_init(struct channel *current_channel)
{
    self.channel = current_channel;
    self.operation = get_lock_operations();
    return RT_EOK;
}
