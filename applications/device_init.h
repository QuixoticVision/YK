#ifndef __DEVICE_INIT_H__
#define __DEVICE_INIT_H__

#include "common.h"
#include "protocol.h"
#include "channel.h"

typedef struct {
    channel             *channel;
    protocol            *protocol;
    func                callback;
} device;

#endif /* __DEVICE_INIT_H__ */
