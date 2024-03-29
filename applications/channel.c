/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     LYX          first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include "channel.h"
#include "can_channel.h"
#include "uart_channel.h"

channel_type_t current_type;

int channel_init(struct channel *channel, channel_type_t type)
{
	int result;
	current_type = type;
	switch (type) {
	case CHANNEL_HW_USING_CAN:
		result = can_channel_init(channel);
		break;
	case CHANNEL_HW_USING_UART:
		break;
	default:
		break;
	}
    return result;
}
