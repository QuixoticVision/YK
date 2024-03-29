#ifndef __CONFIG_H__
#define __CONFIG_H__

typedef enum {
    CONFIG_DEVICE_TYPE = 0,
    CONFIG_ADDR,
    CONFIG_SN,
    CONFIG_TIMEOUT_EXEC,
    CONFIG_TIMEOUT_CMD,
    CONFIG_AVC_SWITCH_STATE,
} config_t;

struct config {
    int (*get) (config_t type, void *parameter);
    int (*modify) (config_t type, void *parameter);
};

struct config *get_config_handler(void);

#endif /* __CONFIG_H__ */
