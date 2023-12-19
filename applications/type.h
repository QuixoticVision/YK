#ifndef __TYPE_H__
#define __TYPE_H__

#include <rtthread.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef enum {
    YK_LOCK,        //YKËø
    YK_COLD_LOCK,   //Àä±ÕËø
    AVC,            //AVC
    AVC_COLD_LOCK,  //AVCÀä±ÕËø
    AVC_SWITCH      //AVC¿ª¹Ø
} device_type;

typedef struct {
	void *p_data;
	uint32_t len;
} data;

#endif
