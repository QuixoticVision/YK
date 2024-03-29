#ifndef __HANDLER_H__
#define __HANDLER_H__

#include "common.h"

typedef int (*handler_t) (uint8_t *, size_t);

handler_t *get_handler_table(void);
int handler_init(struct lock *lock);

#endif /* __HANDLER_H__ */
