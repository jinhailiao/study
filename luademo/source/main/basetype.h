/** @file
 *  @brief Ӧ�ó�������ṹ
 *  @author jinhailiao
 *  @date 2017/03/06
 *  @version 0.1
 *  @see Lua����ĵ�
 */
#ifndef __HAI_BASE_TYPE__H__
#define __HAI_BASE_TYPE__H__
#include "basedef.h"

//
//
//
struct S_COMSET
{
	S_DWORD  m_PortNO:8;///< �˿ں�
	S_DWORD  m_DataBit:8;///< [COMM_DATA_] 5,6,7,8
	S_DWORD  m_StopBit:8;///< [COMM_STOP_] 1->1λ,2->2λ,3->1.5
	S_DWORD  m_Parity:4;///< [COMM_PARITY_] 0->no parity;1->odd parity;2->even parity;
	S_DWORD  m_CDCheck:4; ///< [COMM_CDCHECK_]
	S_DWORD  m_Baud;///< 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 600,300
};

struct S_MeterInfo
{
	bool m_Enable; ///< �������״̬ 0����Ч��1����Ч
	S_WORD m_MeasureNo; ///< �����������[ע��������ṹ�������һ��]
	S_BYTE  m_RuleID; ///< ͨ��Э������ 00H��DL/T645-1997��Լ��01H��DL/T645-2007��Լ��02H��������Լ
	bool   m_VipFlag; ///< �ص��û����ԣ�0����ͨ�û���1���ص��û�
	S_COMSET m_PortSet; ///< �˿ںŲ�����˿ںţ�00H~1EH��ʾ485��˿ںţ�1FH��ʾ�ز�ͨ����20H��ʾ���ߣ�30H��ʾ������

	S_WORD m_RatedVoltage;///< ���ѹBCD
	S_DWORD m_RatedCurrent;///< �����BCD

	string m_strAddress; ///< ͨ�ŵ�ַ
	string m_strPassword;
	string m_strUserName;
};

#define METER_COUNT_MAX	10
struct S_MeterInfos:public vector<S_MeterInfo>
{
};

//
//
//
struct S_HttxInfo
{
	bool m_ReportEnable;
	S_BYTE m_MsgConfirmCode; ///<  = 0 #0��ʾ��������Ϣ��֤,1-255:��Ϣ��֤������
	S_BYTE m_HeartBeatPeriodM; ///< =30 #0��ʾ��Ҫ����
	S_BYTE m_ResendTimes; ///< =3 #�ط�����
	S_WORD m_WaitResponseTimeoutS; ///< =6 #�ն˵ȴ��Ӷ�վ��Ӧ�ĳ�ʱʱ��
	S_WORD m_LoginPeriodS; ///< =60 #0��ʾ��Ҫ��½
	S_DWORD m_MonTrafficLimit; ///< =0 #0��ʾ����Ҫ��������
	string m_strChannlName;
};

struct S_HttxInfos:public vector<S_HttxInfo>
{
};

#endif//__HAI_LUA_PARAM__H__

