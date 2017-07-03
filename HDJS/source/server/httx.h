//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			httx.h
// Description:		master station communication
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-16  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SERVERHTTX1_H__
#define __SERVERHTTX1_H__

#include "server.h"
#include "htrace.h"
#include "channel.h"
#include "connect.h"

/** @brief 级联参数
 */
#define FORWARD_ADDR_MAX		3
struct S_FORWARD
{
	bool m_MasterForwarder; ///< true主集中器，false从集中器
	S_BYTE m_ForwardedAddr[FORWARD_ADDR_MAX][4]; ///< ={00000000, 00000000, 00000000} #主填从集中器地址，从填主集中器地址
	S_BYTE m_ResendTimes; ///< 主动站接收失败重发次数(0～3),如为0,表示不重发
	S_BYTE m_CheckPeriodM; ///< 级联巡测周期(1~60)
	char m_ChannlName[CHANNEL_NAME_MAX+1];

public:
	int SetForwardedAddr(int idx, const char *pStrAddr);
	char *GetForwardedAddrStr(int idx, char *pStrAddr);
};

struct S_MSCONNECTSTAT
{
	volatile bool m_HttxNeedPPP[HTTX_ID_MAX];
	volatile bool m_PppOnline;
	volatile bool m_Httx1Logon;

	void Init(void)
	{
		for (int i = 0; i < HTTX_ID_MAX; i++)
            m_HttxNeedPPP[i] = false;
		m_PppOnline  = false;
		m_Httx1Logon = false;
	}
};

struct S_HTTXINFO
{
	S_BOOL m_ConversationEnable;
	S_BOOL m_ReportEnable;
	S_BOOL m_Class1ReportConfirm;
	S_BOOL m_Class2ReportConfirm;
	S_BOOL m_Class3ReportConfirm;
	S_WORD m_RTS_MS;//=20 #终端数传机延时时间
	S_BYTE m_TpDelayM;//=6 #终端作为启动站允许发送传输延时时间
	S_BYTE m_ResendTimes;//=3 #重发次数
	S_WORD m_WaitResponseTimeoutS;//=6 #终端等待从动站响应的超时时间
	S_BYTE m_HeartBeatPeriodM;//=30 #0表示不要心跳
	S_BYTE m_MsgConfirmCode;// = 0 #0表示不进行消息认证 1-255消息认证方案号
	S_WORD m_MsgConfirmParam;// = 0 #消息认证参数
	S_DWORD m_MonTrafficLimit;//=0 #0表示不须要流量控制
	S_CHAR  m_ChannelName[HTTX_CHANNEL_MAX][CHANNEL_NAME_MAX];
	S_SERVINFO *m_pServInfo;

public:
	S_HTTXINFO(S_SERVINFO *pInfo=NULL);
	~S_HTTXINFO();
};

enum
{
	CMDTYPE_ADDR_UNKNOW,
    CMDTYPE_ADDR_LOCAL,
	CMDTYPE_ADDR_SLAVE,
	CMDTYPE_ADDR_RELAY,
	CMDTYPE_ADDR_BROADCAST,

	CMDTYPE_ADDR_MAX
};


struct S_REPORTCMD
{
	S_BOOL m_confirm;
	S_BYTE m_resend;
	time_t m_SendTime;
	string m_CmdInfo;
	string m_frame;
};

struct S_RESPONSECMD
{
	string m_frame;
};

typedef list<S_REPORTCMD> S_REPORTCMDS;
typedef list<S_RESPONSECMD> S_RESPONSECMDS;

class C_HTTX: public C_SERVER
{
public:
	virtual int BeforeService(void);
	virtual int Servicing(void);
	virtual int AfterService(void);

protected:
	int ServiceProc(void);
	int ChannelService(void);

protected:
	int InitMsruleInformation(void);
	int MakeLogin(void);
	int MakeHeartBeat(void);
	int MakeResponse(const strings &strResponse);

	int ReportSend(void);
	int ResponseSend(void);

	int CmdPut(const string &strCmd);
	int CmdGet(string &strCmd);
	int CmdCheck(const string &strCmd);

	int CmdLocalProc(const string &strCmd);
	int CmdSlaveProc(const string &strCmd);
	int CmdRelayProc(const string &strCmd);

	int CmdParser(const string &strCmd, strings &strResponse);

	int ReportFailedProc(const string &strReportCmd);
	int CmdConfirmProc(int CmdResult, const strings &strF3);

	S_BOOL NeedLogin(void){return m_pHttxInfo->m_HeartBeatPeriodM > 0;}
	S_BOOL NeedHBeat(void){return m_pHttxInfo->m_HeartBeatPeriodM > 0;}
	S_BOOL HeartBeatTime(void);

protected:
	S_BYTE m_HttxID;
	S_BOOL m_SendEnable; ///< 
	S_CHANNEL  *m_pChannel;
	S_HTTXINFO *m_pHttxInfo;
	C_CONNECT  *m_pMConnect;
	C_CONNECT  *m_pFConnect;

	S_REPORTCMDS m_ReportCmds;
	S_RESPONSECMDS m_ResponseCmds;
};

//
//master station communication
//
class C_HTTXI: public C_HTTX
{
public:
	C_HTTXI(void);
	~C_HTTXI();

public:
	int BeforeService(void);
	int Servicing(void);
	int AfterService(void);

};

//
//Local ethernet communication
//
class C_HTTXII: public C_HTTX
{
public:
	C_HTTXII(void);
	~C_HTTXII();

public:
	int BeforeService(void);
	int Servicing(void);
	int AfterService(void);

};

//
//Local serial communication
//
class C_HTTXIII: public C_HTTX
{
public:
	C_HTTXIII(void);
	~C_HTTXIII();

public:
	int BeforeService(void);
	int Servicing(void);
	int AfterService(void);

};


#endif //__SERVERHTTX1_H__

