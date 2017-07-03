//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			ppp.h
// Description:		ppp process
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-16  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SERVERPPP_H__
#define __SERVERPPP_H__

#include "server.h"
#include "channel.h"
#include "appbase.h"
#include "modem.h"

#define MODEM_TYPE_CHECK_SECONDS 120

struct C_DIALSET
{
	S_CHAR m_Apn[16];
	S_CHAR m_TeleNo[32];
	S_CHAR m_User[32];
	S_CHAR m_Pwd[32];
	S_CHAR m_ModemInitCmd[32];
};

struct C_PPPSET
{
	S_WORD m_CheckInterM;//检测间隔（分钟）
	S_WORD m_CheckFailTimes;//检测失败阀值，如果连续此数值次检测失败，将断线
	S_CHAR m_ChannlName[CHANNEL_NAME_MAX+1];
	C_DIALSET m_GprsSet;
};

struct S_PPPINFO
{
	S_CHAR m_GprsSignal;
	S_BYTE m_OnlineMode;//
	S_WORD m_RedialIntervalS;//
	S_WORD m_RedialTimes;//
	S_WORD m_IdleTimeoutM;//
	C_BITSET<3> m_OnlineSpace;

	S_CHAR m_MasterPhone[20];//主站号码
	S_CHAR m_SmsCenterNo[20];//短信中心号码

	C_PPPSET m_PppSet;
	S_SERVINFO *m_pServInfo;

public:
	S_PPPINFO(S_SERVINFO *pInfo=NULL);
	~S_PPPINFO();
};


class C_PPP: public C_SERVER
{
public:
	C_PPP(void);
	~C_PPP();

public:
	int BeforeService(void);
	int Servicing(void);
	int AfterService(void);

private:
	S_BOOL CheckStatus(void);
	S_BOOL PppOnline(void);
	int ModemServer(void);
	int OnlineServer(void);

	int DetectModemType(void);
	S_CHANNEL* GetModemChannel(void);
	S_BOOL SetModemConnect(void);
	S_BOOL ClrModemConnect(void);

	int ModemReset(void);
	int ModemCheck(void);
	int ModemRunInfo(void);

	int InitSmsMode(void);
	int SmsSubmit(const string &strSCA, const string &strDA, const string &strUD, S_BOOL fTextMode=false);
	int SmsDeliver(string &strOA, string &strUD, string &strSCTS);
	int SmsDelete(S_BOOL fRead, S_BOOL fAll);

private:
	S_PPPINFO *m_pPppInfo;
	C_MODEM *m_pModem;
};


#endif //__SERVERPPP_H__

