/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "common.h"

#define DBG_TAG                 "test.eeprom"
#define DBG_LVL                 DBG_LOG
#include <rtdbg.h>

#include "board.h"
#include "at24cxx.h"
#include "hw_define.h"

#define RELAY_CT_CLOSE              GET_PIN(C, 9)
#define RELAY_CT_OPEN               GET_PIN(C, 8)
#define RELAY_CHECK                 GET_PIN(B, 1)
#define LED_CT1                     GET_PIN(C, 12)
#define LED_CT2                     GET_PIN(C, 11)
#define LED_CT3                     GET_PIN(C, 10)
#define LED_CT4                     GET_PIN(A, 15)
#define LED1_RED_ON()               do {rt_pin_write(LED_CT1, PIN_HIGH);rt_pin_write(LED_CT2, PIN_LOW);}while(0)
#define LED1_GREEN_ON()             do {rt_pin_write(LED_CT2, PIN_HIGH);rt_pin_write(LED_CT1, PIN_LOW);}while(0)
#define LED1_OFF()                  do {rt_pin_write(LED_CT2, PIN_LOW);rt_pin_write(LED_CT1, PIN_LOW);}while(0)

#include "config.h"

static struct {
    struct config *config;
} self;

static int test_bl14_pre_init(void)
{
    self.config = get_config_handler();
    rt_pin_mode(PIN_AVC_RELAY_CLOSE, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_AVC_RELAY_OPEN, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_AVC_RELAY_CHECK, PIN_MODE_INPUT);

    return RT_EOK;
}
INIT_ENV_EXPORT(test_bl14_pre_init);

static int test_config_modify(int argc, char **argv)
{
    if (argc < 3) {
        M_LOG_E("missing parameters.");
        return RT_ERROR;
    }

    char *key = argv[1];

    if (strstr(key, "addr")) {
        uint8_t addr;
        sscanf(argv[2], "%02x", &addr);
        self.config->modify(CONFIG_ADDR, &addr);
    }
    if (strstr(key, "sn")) {
        if (strstr(argv[2], "random")) {
            srand(rt_tick_get());
            uint32_t sn = rand();
            M_PRINTF("\nsn: 0x%06X\n", sn);
            self.config->modify(CONFIG_SN, &sn);
            return RT_EOK;
        } else if (argc < 5) {
            M_LOG_E("missing parameters.");
            return RT_ERROR;
        }
        uint8_t ch;
        uint32_t sn = 0;
        sscanf(argv[2], "%02x", &ch);
        sn += (uint32_t)(ch << 16);
        sscanf(argv[3], "%02x", &ch);
        sn += (uint32_t)(ch << 8);
        sscanf(argv[4], "%02x", &ch);
        sn += (uint32_t)(ch << 0);
        M_PRINTF("\nsn: 0x%06X\n", sn);
        self.config->modify(CONFIG_SN, &sn);
    }
    return RT_EOK;
}
MSH_CMD_EXPORT(test_config_modify, test_config_modify);

static int test_realy_check(int argc, char **argv)
{
    M_LOG_I("check:%d", rt_pin_read(RELAY_CHECK));
    return 0;
}
MSH_CMD_EXPORT(test_realy_check, test_realy_check);

static int test_realy_close(int argc, char **argv)
{
    rt_pin_write(RELAY_CT_CLOSE, PIN_HIGH);
    M_LOG_I("close");
    rt_thread_delay(10);
    rt_pin_write(RELAY_CT_CLOSE, PIN_LOW);

    M_LOG_I("close, state: %d", rt_pin_read(PIN_AVC_RELAY_CHECK));

    return RT_EOK;
}
MSH_CMD_EXPORT(test_realy_close, test_realy_close);

static int test_realy_open(int argc, char **argv)
{
    rt_pin_write(RELAY_CT_OPEN, PIN_HIGH);
    M_LOG_I("open");
    rt_thread_delay(10);
    rt_pin_write(RELAY_CT_OPEN, PIN_LOW);

    M_LOG_I("open, state: %d", rt_pin_read(PIN_AVC_RELAY_CHECK));

    return RT_EOK;
}
MSH_CMD_EXPORT(test_realy_open, test_realy_open);


static int test_yk_relay_switch(int argc, char **argv)
{
    static int state;
    state = !state;
    rt_pin_write(PIN_YK_DOUT, state);
    M_LOG_I("%d", state);

    return RT_EOK;
}
MSH_CMD_EXPORT(test_yk_relay_switch, test_yk_relay_switch);

static int test_yk_output_ctl(int argc, char **argv)
{
    if (argc < 3) {
        M_LOG_E("missing parameter");
        return RT_ERROR;
    }
    int index, state;
    sscanf(argv[1], "%d", &index);
    sscanf(argv[2], "%d", &state);
    switch (index) {
    case 1:
        rt_pin_write(PIN_YK_RELAY_CT1, state ? PIN_HIGH : PIN_LOW);
        break;
    case 2:
        rt_pin_write(PIN_YK_RELAY_CT2, state ? PIN_HIGH : PIN_LOW);
        break;
    case 3:
        rt_pin_write(PIN_YK_RELAY_CT3, state ? PIN_HIGH : PIN_LOW);
        break;
    default:
        break;
    }
    M_LOG_I("k%d = %d", index, state);

    return RT_EOK;
}
MSH_CMD_EXPORT(test_yk_output_ctl, test_yk_output_ctl);

