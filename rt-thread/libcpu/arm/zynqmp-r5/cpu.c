/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-19     WangHuachen  first version
 */

#include <rthw.h>
#include <rtthread.h>
#include "zynqmp-r5.h"

void rt_hw_cpu_reset()
{
    __REG32(ZynqMP_CRL_APB_BASEADDR + ZynqMP_CRL_APB_RESET_CTRL) |= ZynqMP_RESET_MASK;
    while (1);  /* loop forever and wait for reset to happen */
    /* NEVER REACHED */
}
