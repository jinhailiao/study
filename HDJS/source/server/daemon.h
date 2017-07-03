//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			daemon.h
// Description:		daemon process
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-14  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SERVERDAEMON_H__
#define __SERVERDAEMON_H__

#include "server.h"
#include "htrace.h"

#define HAREWARE_RESET_DELAY_MAX	30
#define SOFTWARE_RESET_DELAY_MAX		10


struct S_ResetInfo
{
	bool m_HWReset;
	bool m_SWReset;
	S_WORD m_HWResetDelayS;
	S_WORD m_SWResetDelayS;

public:
	S_ResetInfo(void)
	{
		m_HWReset = false;
		m_SWReset = false;
		m_HWResetDelayS = 0;
		m_SWResetDelayS = 0;
	}
	~S_ResetInfo(){}
};

struct S_DAEMONINFO
{
	S_SERVINFO *m_pServInfo;
	S_ResetInfo  m_ResetInfo;

public:
	S_DAEMONINFO(S_SERVINFO *pInfo=NULL);
	~S_DAEMONINFO();
};


class C_DAEMON: public C_SERVER
{
public:
	C_DAEMON(void);
	~C_DAEMON();

public:
	int BeforeService(void);
	int Servicing(void);
	int AfterService(void);

	bool ScanServers(void);
	bool SyspendServers(bool bSyspend);
	bool CheckStatus(void);
	bool WaitServer(PID_T pid=-1);
	bool WatchDog(void);

private:
	bool doWatchDog(void);
	bool SystemReboot(void);

	bool KillServer(S_SERVINFO &ServInfo);
	bool StartServer(S_SERVINFO &ServInfo);
	
private:
	S_DAEMONINFO *m_pDaemonInfo;
};


#endif //__SERVERDAEMON_H__

