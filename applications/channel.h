#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "common.h"

typedef int (*init_t)   (void);
typedef void (*recv_t)  (void);
typedef void (*send_t)  (data *frame);

typedef struct {
    int (*init)     (void);
    int (*recv)     (rt_tick_t timeout);
    int (*send)     (data *frame);
    void *user_data;        //备用
} channel;

typedef enum {
	CHANNEL_HW_USING_CAN = 0,
	CHANNEL_HW_USING_UART,
} channel_type;

extern channel *channel_select(channel_type type);

#endif /* __CHANNEL_H__ */
