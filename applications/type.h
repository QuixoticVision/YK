#ifndef __TYPE_H__
#define __TYPE_H__

#include <rtthread.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef enum {
    YK_LOCK,        //YK��
    YK_COLD_LOCK,   //�����
    AVC,            //AVC
    AVC_COLD_LOCK,  //AVC�����
    AVC_SWITCH      //AVC����
} device_type;

typedef struct {
	void *p_data;
	uint32_t len;
} data;

#endif
