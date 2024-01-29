#ifndef __OPERATE_H__
#define __OPERATE_H__

#include "common.h"

typedef enum {
    CONTROL_YK_STATE,
    CONTROL_AVC_ENGAGE,
    CONTROL_AVC_DISENGAGE,
} lock_control_t;

struct lock_operations {
    uint8_t (*query_state) (void);
    uint8_t (*control) (uint8_t flag, void *parameter);
    void (*modify_addr) (uint8_t address);
    uint8_t (*get_addr) (void);
    void (*modify_sn) (uint32_t sn);
    uint32_t (*get_sn) (void);
};

struct lock_operations *get_lock_operations(void);

#endif /* __OPERATE_H__ */
