#ifndef __OPERATE_H__
#define __OPERATE_H__

#include "common.h"
#include "lock.h"

typedef enum {
    CONTROL_YK_OPEN,
    CONTROL_YK_CLOSE,
    CONTROL_AVC_SWITCH_ENGAGE,
    CONTROL_AVC_SWITCH_DISENGAGE,
    CONTROL_COLD_LOCK_POWER_ON,
    CONTROL_COLD_LOCK_POWER_OFF,
} lock_control_t;

typedef enum {
    YK_PORT_STATE,
    AVC_SWITCH_LED_RELAY_STATE,
    AVC_SWITCH_RELAY_STATE,
    AVC_SWITCH_OPERATION_RESULT,
    COLD_LOCK_STATE,
} query_state_t;

typedef enum {
    TIMEOUT_COLD_LOCK_EXEC,
    TIMEOUT_COLD_LOCK_CMD,
} timeout_t;

struct lock_operation {
    uint8_t (*query_state) (query_state_t);
    int (*control) (lock_control_t, void *);
    int (*modify_addr) (uint8_t address);
    uint8_t (*get_addr) (void);
    int (*modify_sn) (uint32_t sn);
    uint32_t (*get_sn) (void);
    int (*modify_device) (uint8_t type);
    lock_type_t (*get_device) (void);
    int (*modify_timeout_exec) (uint8_t timeout);
    uint8_t (*get_timeout_exec) (void);
    int (*modify_timeout_cmd) (uint8_t timeout);
    uint8_t (*get_timeout_cmd) (void);
};

struct lock_operation *get_lock_operation(void);

#endif /* __OPERATE_H__ */
