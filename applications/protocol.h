#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "common.h"

/* ----------------------- FUNCTION CODE BEGIN ------------------------------------------*/
//YK电气编码锁部分通信规约
#define FUNC_CODE_YK_QUERY_STATE                    0x01    //上位机查询YK电气编码锁状态
#define FUNC_CODE_YK_CONTROL                        0x02    //命令特定地址的YK电气编码锁执行分/合操作
#define FUNC_CODE_YK_MODIFY_SN                      0x03    //该指令是把SN码为Xx Xx Xx的Xx地址改为YY地址，16进制
#define FUNC_CODE_YK_REPORT_STATE                   0x04    //YK电气编码锁主动上发变位信息
#define FUNC_CODE_YK_PRODUCTION_MODIFY_SN           0x07    //更改YK电气编码锁SN码（只对生产开放,不允许调试人员使用）

//AVC开关部分通信规约
#define FUNC_CODE_AVC_QUERY_STATE                   0x31    //上位机查询AVC开关状态
#define FUNC_CODE_AVC_CONTROL_UPPER                 0x35    //上位机对AVC执行投退命令（上位机主动发起）
#define FUNC_CODE_AVC_CONTROL_LOWER                 0x32    //上位机对AVC执行投退命令（下位机主动发起）
#define FUNC_CODE_AVC_MODIFY_ADDR_TYPE              0x33    //变更地址及设备类型
#define FUNC_CODE_AVC_MODIFY_SN                     0x37    //变更SN指令
#define FUNC_CODE_AVC_PRODUCTION_QUERY              0x3F    //超级查询指令（点对点）

//冷闭锁套件部分通信规约
#define FUNC_CODE_COLD_LOCK_QUERY_ALL               0x21    //查询冷闭锁套件的地址、SN、状态
#define FUNC_CODE_COLD_LOCK_CONTROL                 0x25    //对冷闭锁套件进行操作，进入冷闭锁状态或解除冷闭锁状态
#define FUNC_CODE_COLD_LOCK_MODIFY_TIMEOUT          0x2A    //上位机设置指定的冷闭锁套件的超时时间
#define FUNC_CODE_COLD_LOCK_MODIFY_ADDR             0x23    //上位机设置指定的冷闭锁套件的地址
#define FUNC_CODE_COLD_LOCK_MODIFY_SN               0x27    //上位机设置指定的冷闭锁套件的SN码
#define FUNC_CODE_COLD_LOCK_PRODUCTION_QUERY        0x2F    //上位机对冷闭锁套件执行超级查询命令
/* ----------------------- FUNCTION CODE END --------------------------------------------*/

#define PREFIX_LENTH                    	strlen("FUNC_CODE_")


typedef struct {
    uint8_t function_code;
    data    *data;
} info;

typedef struct {
    int (*init)    (void);
    info *(*parser) (data *frame);
} protocol;

extern protocol *protocol_select(device_type dev_type);

#endif /* __PROTOCOL_H__ */
