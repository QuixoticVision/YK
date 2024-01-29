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

channel_type current_type;

void set_rx_callback(int (*rx_callback) (size_t len, void *parameter), void *parameter)
{
	if (rx_callback == NULL)
		return;
	switch (current_type) {
	case CHANNEL_HW_USING_CAN:
		set_can_rx_callback(rx_callback, parameter);
		break;
	case CHANNEL_HW_USING_UART:
		break;
	default:
		set_can_rx_callback(rx_callback, parameter);
		break;
	}
}

int channel_init(struct channel *channel, channel_type type)
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
