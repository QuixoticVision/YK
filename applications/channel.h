#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "type.h"

typedef int (*init_t)   (void);
typedef void (*recv_t)  (void);
typedef void (*send_t)  (data *frame);

typedef struct {
    const init_t init;
    const recv_t recv;
    const send_t send;
    void *user_data;        //±∏”√
} channel;

typedef enum {
	CHANNEL_HW_USING_CAN = 0,
	CHANNEL_HW_USING_UART,
} channel_hw_t;

#endif
