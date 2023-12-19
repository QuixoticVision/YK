#ifndef __DEVICE_TYPE_H__
#define __DEVICE_TYPE_H__

#include "type.h"
#include "protocol.h"

typedef struct {
    protocol_handler *handler;
} device;

#endif
