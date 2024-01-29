#ifndef __HANDLER_H__
#define __HANDLER_H__

#include "common.h"

typedef int (*handler_t) (uint8_t *, size_t);

extern handler_t *get_handler_table(void);
extern int handler_init(struct channel *current_channel);

#endif /* __HANDLER_H__ */
