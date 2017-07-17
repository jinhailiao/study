/** @file
 *  @brief 应用程序基础结构
 *  @author jinhailiao
 *  @date 2017/03/06
 *  @version 0.1
 *  @see Lua相关文档
 */
#ifndef __HAI_BASE_TYPE__H__
#define __HAI_BASE_TYPE__H__
#include "basedef.h"

//
//
//
struct S_COMSET
{
	S_DWORD  m_PortNO:8;///< 端口号
	S_DWORD  m_DataBit:8;///< [COMM_DATA_] 5,6,7,8
	S_DWORD  m_StopBit:8;///< [COMM_STOP_] 1->1位,2->2位,3->1.5
	S_DWORD  m_Parity:4;///< [COMM_PARITY_] 0->no parity;1->odd parity;2->even parity;
	S_DWORD  m_CDCheck:4; ///< [COMM_CDCHECK_]
	S_DWORD  m_Baud;///< 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 600,300
};

struct S_MeterInfo
{
	bool m_Enable; ///< 测量点的状态 0：无效；1：有效
	S_WORD m_MeasureNo; ///< 所属测量点号[注意在数组结构里与序号一致]
	S_BYTE  m_RuleID; ///< 通信协议类型 00H：DL/T645-1997规约；01H：DL/T645-2007规约；02H：其他规约
	bool   m_VipFlag; ///< 重点用户属性：0，普通用户；1，重点用户
	S_COMSET m_PortSet; ///< 端口号测量点端口号：00H~1EH表示485表端口号；1FH表示载波通道；20H表示无线；30H表示其他。

	S_WORD m_RatedVoltage;///< 额定电压BCD
	S_DWORD m_RatedCurrent;///< 额定电流BCD

	string m_strAddress; ///< 通信地址
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
	S_BYTE m_MsgConfirmCode; ///<  = 0 #0表示不进行消息认证,1-255:消息认证方案号
	S_BYTE m_HeartBeatPeriodM; ///< =30 #0表示不要心跳
	S_BYTE m_ResendTimes; ///< =3 #重发次数
	S_WORD m_WaitResponseTimeoutS; ///< =6 #终端等待从动站响应的超时时间
	S_WORD m_LoginPeriodS; ///< =60 #0表示不要登陆
	S_DWORD m_MonTrafficLimit; ///< =0 #0表示不须要流量控制
	string m_strChannlName;
};

struct S_HttxInfos:public vector<S_HttxInfo>
{
};

#endif//__HAI_LUA_PARAM__H__

