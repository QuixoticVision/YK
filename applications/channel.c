/*
 * Copyright (c) 2006-2023, CYG
 *
 * Change Logs:
 * Date           Author       LYX
 * 2023-12-18     RiceChen    first edition
 */
 
#include <rtthread.h>
#include <rtdevice.h>
#include "channel.h"
#include "channel_can.h"

static struct channel channel;

struct channel *channel_select(channel_type type)
{
	struct channel *p = &channel;
	struct channel_can_ops *can_ops;
	switch (type) {
	case CHANNEL_HW_USING_CAN:
		can_ops = get_can_ops_handler();
		p->read = can_ops->read;
		p->write = can_ops->write;
		p->init = can_ops->init;
		break;
	case CHANNEL_HW_USING_UART:
		break;
	default:
		break;
	}
    return p;
}
