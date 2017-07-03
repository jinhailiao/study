//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			daemon.cpp
// Description:		daemon process
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-14  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "daemon.h"
#include "htrace.h"
#include "context.h"
#include "haicfg.h"
#include "command.h"
#ifndef WIN32
#include <sys/wait.h>
#include <errno.h>
#endif

S_DAEMONINFO::S_DAEMONINFO(S_SERVINFO *pInfo)
{
	m_pServInfo = pInfo;
}
S_DAEMONINFO::~S_DAEMONINFO()
{
}

//
//
//
C_DAEMON::C_DAEMON(void)
{
	m_pServInfo = &(CONTEXT.m_ServInfo[HDJS_SERID_DAEMON]);
	m_pDaemonInfo = &(CONTEXT.m_DaemonInfo);
}

C_DAEMON::~C_DAEMON()
{
}

int C_DAEMON::BeforeService(void)
{
	if (C_ContextOP::IsContextNewest() == false)
	{//reload config file
		logit<<"Daemon Check CONTEXT is oldest, will reload...."<<endl;
		C_ContextOP::GetContext()->init();
		if (C_ContextOP::IsStartAllServer() == true)
			C_ContextOP::SetAllServerActive();
		if (C_ContextOP::LoadConfig() == false)
		{
			loget<<"Important config loads failed!!!"<<endl;
			return -1;
		}
		//OK, Flush it
		C_ContextOP::ParamParse();
		C_ContextOP::SaveContext();
		C_ContextOP::SetContextNewest();
		logit<<"CONTEXT load OK."<<endl;
	}
	C_SERVER::SetCurrentServId(HDJS_SERID_DAEMON);
	C_HTRACE::SetSystemTraceSetting(&CONTEXT.m_TraceSet[HDJS_SERID_DAEMON]);
	logwt<<"Daemon is going to work......"<<endl;
	m_pServInfo->SetStartedStatus(getpid());
	return 0;
}

int C_DAEMON::Servicing(void)
{
	while (1)
	{
		WatchDog();
		HeartBeat();
		ScanServers();
		WaitServer();
		CheckStatus();
		SleepSecond(1);
	}
	return 0;
}

int C_DAEMON::AfterService(void)
{
	return 0;
}

bool C_DAEMON::ScanServers(void)
{
	for (int i = HDJS_SERID_DAEMON; i < HDJS_SERID_MAX; i++)
	{
		if (i == HDJS_SERID_DAEMON)
			continue;
		S_SERVINFO &ServInfo = C_ContextOP::GetContext()->m_ServInfo[i];
		if(ServInfo.IsRunning())
		{//check if that is death
			if (ServInfo.IsHBTimeout(m_pServInfo->m_LHBTime) == true)
			{
				ServInfo.m_DeadCnt++;
				loget<<C_SERVER::GetServerName(ServInfo.m_ServID)<<"'s DeadCnt = "<<ServInfo.m_DeadCnt<<"."<<endl;
				loget<<C_SERVER::GetServerName(ServInfo.m_ServID)<<" heart-beat timeout, will be killed!!!"<<endl;
				KillServer(ServInfo);
			}
			else if(ServInfo.IsKillable() == true)
			{
				loget<<C_SERVER::GetServerName(ServInfo.m_ServID)<<" was suspended or stopped,will be killed!!!"<<endl; 
				KillServer(ServInfo);
			}
			if (ServInfo.m_DeadCnt > SERVER_RETRY_TIMES || ServInfo.m_ExeCnt > SERVER_RETRY_TIMES)
			{
				loget<<C_SERVER::GetServerName(ServInfo.m_ServID)<<"'s dead cnt is more than "<<SERVER_RETRY_TIMES<<", the system will reboot!!!"<<endl; 
				ServInfo.m_DeadCnt = 0, ServInfo.m_ExeCnt = 0;//avoid reentry
				m_pDaemonInfo->m_ResetInfo.m_HWReset = true;//reboot
			}
		}
		else if(ServInfo.IsStartable() == true)
		{//exec server
			if (StartServer(ServInfo) == true)
			{
				logwt<<"Start "<<C_SERVER::GetServerName(ServInfo.m_ServID)<<" succeed!!!"<<endl;
			}
			else
			{
				logwt<<"Start "<<C_SERVER::GetServerName(ServInfo.m_ServID)<<" failed!!!"<<endl;
			}
		}
	}
	return true;
}

bool C_DAEMON::SyspendServers(bool bSyspend)
{
	for (int i = HDJS_SERID_DAEMON; i < HDJS_SERID_MAX; i++)
	{
		if (i == HDJS_SERID_DAEMON)
			continue;
		S_SERVINFO &ServInfo = C_ContextOP::GetContext()->m_ServInfo[i];
		
		if (bSyspend == true) ServInfo.SetSuspend();
		else ServInfo.SetResume();
	}
	return true;
}

bool C_DAEMON::CheckStatus(void)
{
	static time_t PrevTick = 0;
	S_ResetInfo &RI = m_pDaemonInfo->m_ResetInfo;
	if (RI.m_HWReset == true && RI.m_HWResetDelayS == 0)
	{
		RI.m_HWResetDelayS = 1;
		PrevTick = time(NULL);
		SyspendServers(true);
		//sync_system
	}
	if (RI.m_SWReset == true && RI.m_SWResetDelayS == 0)
	{
		RI.m_SWResetDelayS = 1;
		PrevTick = time(NULL);
		SyspendServers(true);
		//sync_system
	}
	if (RI.m_SWResetDelayS || RI.m_HWResetDelayS)
	{
		time_t now = time(NULL);
		if (PrevTick > now) PrevTick = now;
		if (RI.m_SWResetDelayS) RI.m_SWResetDelayS += (S_WORD)(now - PrevTick);
		if (RI.m_HWResetDelayS) RI.m_HWResetDelayS += (S_WORD)(now - PrevTick);
	}

	if (RI.m_HWResetDelayS > HAREWARE_RESET_DELAY_MAX)
	{
		loget<<"Hardware Dog is not work normally.force to reboot system!!!"<<endl;
		SystemReboot();
	}
	if (RI.m_SWResetDelayS > SOFTWARE_RESET_DELAY_MAX)
	{
		exit(0);
	}

	return true;
}

bool C_DAEMON::WaitServer(PID_T pid)
{
#ifndef WIN32
	int status;
	S_SERVINFO *pServInfo;
	PID_T child=waitpid(pid, &status, WNOHANG); 

	if(child <= 0)
		return false;

	for (int i = HDJS_SERID_DAEMON; i < HDJS_SERID_MAX; i++)
	{
		pServInfo = &(C_ContextOP::GetContext()->m_ServInfo[i]);
		if (pServInfo->m_PID == child)
		{
			logwt<<C_SERVER::GetServerName(pServInfo->GetServerID())<<"(pid:"<<child<<") return ";
#ifndef WIN32
			if(WIFEXITED(status))
				logw<<WEXITSTATUS(status)<<",no normal";
			else if(WIFSIGNALED(status))
				logw<<WTERMSIG(status)<<",no normal";
			else//status == 0
				logw<<status;
#endif
			logw<<"!!!."<<endl;
			
			pServInfo->SetStopedStatus();
			break;
		}
	}
#endif
	return true;
}

bool C_DAEMON::WatchDog(void)
{
	if (m_pDaemonInfo->m_ResetInfo.m_HWReset == true)
		return false;
	return doWatchDog();
}

bool C_DAEMON::KillServer(S_SERVINFO &ServInfo)
{
#ifndef WIN32
	if (ServInfo.m_PID > 0)
	{
		if (kill(ServInfo.m_PID,SIGKILL) == -1)
		{
			loget<<"Forcely kill "<<C_SERVER::GetServerName(ServInfo.m_ServID)<<"(pid="<<ServInfo.m_PID<<") error: "<<strerror(errno)<<endl;
		}
		else
		{
			loget<<"Forcely kill "<<C_SERVER::GetServerName(ServInfo.m_ServID)<<"(pid="<<ServInfo.m_PID<<") succeed."<<endl;
			ServInfo.SetStoppingStatus();
		}
	}
#endif
	return true;
}

bool C_DAEMON::StartServer(S_SERVINFO &ServInfo)
{
	PID_T child = 0;
#ifndef WIN32
	if((child=fork()) == -1) 
	{ 
		loget<<"Fork error in server "<<C_SERVER::GetServerName(ServInfo.m_ServID)<<": "<<strerror(errno)<<endl;
		return false;
	} 
	else if(child == 0) 
	{
		char linkExe[260];
		char Program[260];

		sprintf(Program, "%s%s", CFG_FILE_PROG_PATH, CFG_FILE_PROG_NAME);
		sprintf(linkExe, "/tmp/%s_%s", CFG_FILE_PROG_NAME, C_SERVER::GetServerName(ServInfo.m_ServID));
		unlink(linkExe);
		if (symlink(Program, linkExe) != 0)
		{
			loget<<"SymLink failed from "<<Program<<" to "<<linkExe<<"."<<endl;
			exit(1);
		}
		execl(linkExe, linkExe, DJS_CMD_EXEC, C_SERVER::GetServerName(ServInfo.m_ServID), NULL);
		loget<<"execl error: "<<strerror(errno)<<endl;
		exit(0);
	}
	else if (child > 0)
	{
		SleepMS(1);
	}
#endif

	ServInfo.SetStartedStatus(child);
	logit<<C_SERVER::GetServerName(ServInfo.m_ServID)<<"(pid="<<child<<") is starting...,ExecTimes="<<ServInfo.m_ExeCnt<<endl;
	return true;
}

bool C_DAEMON::doWatchDog(void)
{
	extern int io_Watchdog(void);
	static time_t lastWatchTime = 0;
	time_t TickSecond = time(NULL);

	if (lastWatchTime == 0)//应用程序一启动就喂狗
		;
	else if (TickSecond < lastWatchTime)//向下对时
		;
	else if (TickSecond < lastWatchTime+5)//5秒钟喂一次
		return false;
	else if (TickSecond > lastWatchTime+10)
		loget<<"More than "<<(long)(TickSecond-lastWatchTime)<<" seconds not watch dog\n"<<endl;

	lastWatchTime = TickSecond;
	if (io_Watchdog() != 0)
	{
		loget<<"Watchdog exec failed!"<<endl;
		return false;
	}
	return true;
}

bool C_DAEMON::SystemReboot(void)
{
	system("reboot");
	return true;
}


