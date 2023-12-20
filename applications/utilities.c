/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     RiceChen    first edition
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "utilities.h"

void power_on(void);
void power_off(void);
void lock(void);
void unlock(void);
void modify_sn(uint8_t *sn);
uint8_t *read_sn(void);
int query_state(void);
void led_on(device_type dev_type, void *p);
void led_off(device_type dev_type, void *p);