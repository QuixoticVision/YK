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

static int func_yk_query_state(uint8_t *data, size_t len);
static int func_yk_control(uint8_t *data, size_t len);
static int func_yk_modify_addr(uint8_t *data, size_t len);
static int func_yk_report_state(uint8_t *data, size_t len);
static int func_yk_production_modify_sn(uint8_t *data, size_t len);

static int func_avc_query_state(uint8_t *data, size_t len);
static int func_avc_control_engage(uint8_t *data, size_t len);
static int func_avc_control_disengage(uint8_t *data, size_t len);
static int func_avc_modify_addr_type(uint8_t *data, size_t len);
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
    func_avc_control_engage,
    func_avc_control_disengage,
    func_avc_modify_addr_type,
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
    buff[6] = sn >> 0;
    buff[7] = sn >> 8;
    buff[8] = sn >> 16;
    buff[9] = self.operation->query_state();
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x02 */
static int func_yk_control(uint8_t *data, size_t len)
{
    self.operation->control(CONTROL_YK_STATE, &data[1]);
    uint8_t buff[8];
    memset(buff, 0, ARRAY_SIZE(buff));

    buff[3] = 0x03;
    buff[4] = 0x02;
    buff[5] = self.operation->get_addr();
    buff[6] = self.operation->query_state();
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
    buff[6] = sn >> 0;
    buff[7] = sn >> 8;
    buff[8] = sn >> 16;
    buff[9] = self.operation->query_state();
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
    buff[6] = sn >> 0;
    buff[7] = sn >> 8;
    buff[8] = sn >> 16;
    buff[9] = self.operation->query_state();
    buff[10] = crc8(buff, ARRAY_SIZE(buff) - 1);

    self.channel->write(buff, ARRAY_SIZE(buff));

    return RT_EOK;
}

/* 0x35 */
static int func_avc_control_engage(uint8_t *data, size_t len)
{
    self.operation->control(CONTROL_AVC_ENGAGE, NULL);
    return RT_EOK;
}

static int func_avc_control_disengage(uint8_t *data, size_t len)
{
    return RT_EOK;
}

static int func_avc_modify_addr_type(uint8_t *data, size_t len)
{
    return RT_EOK;
}

static int func_avc_modify_sn(uint8_t *data, size_t len)
{
    return RT_EOK;
}

static int func_avc_production_query(uint8_t *data, size_t len)
{
    return RT_EOK;
}

//COLD-LOCK
static int func_cold_lock_query_all(uint8_t *data, size_t len)
{
    return RT_EOK;
}

static int func_cold_lock_control(uint8_t *data, size_t len)
{
    return RT_EOK;
}

static int func_cold_lock_modify_timeout(uint8_t *data, size_t len)
{
    return RT_EOK;
}

static int func_cold_lock_modify_addr(uint8_t *data, size_t len)
{
    return RT_EOK;
}

static int func_cold_lock_modify_sn(uint8_t *data, size_t len)
{
    return RT_EOK;
}

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
