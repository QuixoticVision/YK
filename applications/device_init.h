#ifndef __DEVICE_INIT_H__
#define __DEVICE_INIT_H__

#include "common.h"
#include "protocol.h"
#include "channel.h"

typedef struct {
    protocol            *protocol;
    channel             *channel;
    func                callback;
} device;

#endif /* __DEVICE_INIT_H__ */
