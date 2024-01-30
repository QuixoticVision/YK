#ifndef __OPERATE_H__
#define __OPERATE_H__

#include "common.h"
#include "device_init.h"

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

struct lock_operations {
    uint8_t (*query_state) (query_state_t);
    uint8_t (*control) (lock_control_t);
    uint8_t (*modify_addr) (uint8_t address);
    uint8_t (*get_addr) (void);
    uint8_t (*modify_sn) (uint32_t sn);
    uint32_t (*get_sn) (void);
    uint8_t (*modify_device) (uint8_t type);
    uint8_t (*get_device) (void);
    uint8_t (*set_timeout) (uint8_t exec_timeout, uint8_t cmd_timeout);
    uint8_t (*get_timeout) (timeout_t);
};

struct lock_operations *get_lock_operations(void);
int lock_operations_init(struct device_lock *lock);

#endif /* __OPERATE_H__ */
