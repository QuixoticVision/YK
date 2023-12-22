#ifndef __HANDLER_H__
#define __HANDLER_H__

#include "common.h"


typedef struct {
    uint8_t         func_code;
    int (*func)     (data* frame);
    rt_slist_t       next;
} protocol_handler_list;

typedef int (*p_function_handler)(data *frame);

typedef struct {
	uint8_t function_code;
	int (*handler) (data *frame);
} protocol_handler_table;

#endif /* __HANDLER_H__ */
