#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "type.h"

/* ----------------------- FUNCTION CODE BEGIN ------------------------------------------*/
//YK��������������ͨ�Ź�Լ
#define FUNC_CODE_YK_QUERY_STATE                    0x01    //��λ����ѯYK����������״̬
#define FUNC_CODE_YK_CONTROL                        0x02    //�����ض���ַ��YK����������ִ�з�/�ϲ���
#define FUNC_CODE_YK_MODIFY_SN                      0x03    //��ָ���ǰ�SN��ΪXx Xx Xx��Xx��ַ��ΪYY��ַ��16����
#define FUNC_CODE_YK_REPORT_STATE                   0x04    //YK���������������Ϸ���λ��Ϣ
#define FUNC_CODE_YK_PRODUCTION_MODIFY_SN           0x07    //����YK����������SN�루ֻ����������,�����������Աʹ�ã�

//AVC���ز���ͨ�Ź�Լ
#define FUNC_CODE_AVC_QUERY_STATE                   0x31    //��λ����ѯAVC����״̬
#define FUNC_CODE_AVC_CONTROL_UPPER                 0x35    //��λ����AVCִ��Ͷ�������λ����������
#define FUNC_CODE_AVC_CONTROL_LOWER                 0x32    //��λ����AVCִ��Ͷ�������λ����������
#define FUNC_CODE_AVC_MODIFY_ADDR_TYPE              0x33    //�����ַ���豸����
#define FUNC_CODE_AVC_MODIFY_SN                     0x37    //���SNָ��
#define FUNC_CODE_AVC_PRODUCTION_QUERY              0x3F    //������ѯָ���Ե㣩

//������׼�����ͨ�Ź�Լ
#define FUNC_CODE_COLD_LOCK_QUERY_ALL               0x21    //��ѯ������׼��ĵ�ַ��SN��״̬
#define FUNC_CODE_COLD_LOCK_CONTROL                 0x25    //��������׼����в��������������״̬���������״̬
#define FUNC_CODE_COLD_LOCK_MODIFY_TIMEOUT          0x2A    //��λ������ָ����������׼��ĳ�ʱʱ��
#define FUNC_CODE_COLD_LOCK_MODIFY_ADDR             0x23    //��λ������ָ����������׼��ĵ�ַ
#define FUNC_CODE_COLD_LOCK_MODIFY_SN               0x27    //��λ������ָ����������׼���SN��
#define FUNC_CODE_COLD_LOCK_PRODUCTION_QUERY        0x2F    //��λ����������׼�ִ�г�����ѯ����
/* ----------------------- FUNCTION CODE END --------------------------------------------*/

typedef int (*p_function_handler)(data *frame);

typedef struct {
	uint8_t function_code;
	p_function_handler handler;
} protocol_handler;


extern protocol_handler *protocol_select(device_type type);
#endif
