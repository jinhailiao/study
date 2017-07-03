//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2015
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			command.cpp
// Description:		command manager
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-11  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "command.h"
#include "haidef.h"
#include "server.h"
#include "context.h"
#include "servfact.h"
#include "haicfg.h"
#include "haiver.h"
#include "cfgfile.h"

//first entry must help obj
BEGIN_CMD_OBJ_MAP(HelpObj)
	CMD_OBJ_ENTRY(HDJS_HELP_CMD, C_OBJPROC::Help, "help info")
END_CMD_OBJ_MAP()

//first entry must help obj
BEGIN_CMD_OBJ_MAP(ExecObj)
	CMD_OBJ_ENTRY(HDJS_HELP_CMD,  C_OBJPROC::Help, "help info")
	CMD_OBJ_ENTRY(HDJS_SER_ALLSER,C_OBJPROC::StartAll, "start all server")
	CMD_OBJ_ENTRY(HDJS_SER_DAEMON,C_OBJPROC::Exec, "daemon server")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX1, C_OBJPROC::Exec, "master station server")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX2, C_OBJPROC::Exec, "local ethernet server")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX3, C_OBJPROC::Exec, "local serial server")
	CMD_OBJ_ENTRY(HDJS_SER_DBCJ,  C_OBJPROC::Exec, "meter gather server")
	CMD_OBJ_ENTRY(HDJS_SER_PPP,   C_OBJPROC::Exec, "ppp server")
	CMD_OBJ_ENTRY(HDJS_SER_DMAN,  C_OBJPROC::Exec, "data manage server")
	CMD_OBJ_ENTRY(HDJS_SER_UIMAN, C_OBJPROC::Exec, "UI manage server")
END_CMD_OBJ_MAP()

//first entry must help obj
BEGIN_CMD_OBJ_MAP(ServObj)
	CMD_OBJ_ENTRY(HDJS_HELP_CMD,  C_OBJPROC::Help,     "help info")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX1, C_OBJPROC::ServCtrl, "master station server")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX2, C_OBJPROC::ServCtrl, "local ethernet server")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX3, C_OBJPROC::ServCtrl, "local serial server")
	CMD_OBJ_ENTRY(HDJS_SER_DBCJ,  C_OBJPROC::ServCtrl, "meter gather server")
	CMD_OBJ_ENTRY(HDJS_SER_PPP,   C_OBJPROC::ServCtrl, "ppp server")
	CMD_OBJ_ENTRY(HDJS_SER_DMAN,  C_OBJPROC::ServCtrl, "data manage server")
	CMD_OBJ_ENTRY(HDJS_SER_UIMAN, C_OBJPROC::ServCtrl, "UI manage server")
END_CMD_OBJ_MAP()

//first entry must help obj
BEGIN_CMD_OBJ_MAP(SetObj)
	CMD_OBJ_ENTRY(HDJS_HELP_CMD,  C_OBJPROC::Help,     "help info")
	CMD_OBJ_ENTRY(HDJS_SER_DAEMON,C_OBJPROC::SetParam, "set dyjc parameter")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX1, C_OBJPROC::SetParam, "set httx1 parameter")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX2, C_OBJPROC::SetParam, "set httx2 parameter")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX3, C_OBJPROC::SetParam, "set httx3 parameter")
	CMD_OBJ_ENTRY(HDJS_SER_DBCJ,  C_OBJPROC::SetParam, "set dbcj parameter")
	CMD_OBJ_ENTRY(HDJS_SER_PPP,   C_OBJPROC::SetParam, "set ppp parameter")
	CMD_OBJ_ENTRY(HDJS_SER_DMAN,  C_OBJPROC::SetParam, "set dman parameter")
	CMD_OBJ_ENTRY(HDJS_SER_UIMAN, C_OBJPROC::SetParam, "UI manage server")
END_CMD_OBJ_MAP()

//first entry must help obj
BEGIN_CMD_OBJ_MAP(ListObj)
	CMD_OBJ_ENTRY(HDJS_HELP_CMD,  C_OBJPROC::Help,     "help info")
	CMD_OBJ_ENTRY(HDJS_SER_DAEMON,C_OBJPROC::ListParam, "list dyjc parameter")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX1, C_OBJPROC::ListParam, "list httx1 parameter")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX2, C_OBJPROC::ListParam, "list httx2 parameter")
	CMD_OBJ_ENTRY(HDJS_SER_HTTX3, C_OBJPROC::ListParam, "list httx3 parameter")
	CMD_OBJ_ENTRY(HDJS_SER_DBCJ,  C_OBJPROC::ListParam, "list dbcj parameter")
	CMD_OBJ_ENTRY(HDJS_SER_PPP,   C_OBJPROC::ListParam, "list ppp parameter")
	CMD_OBJ_ENTRY(HDJS_SER_DMAN,  C_OBJPROC::ListParam, "list dman parameter")
	CMD_OBJ_ENTRY(HDJS_SER_UIMAN, C_OBJPROC::ListParam, "UI manage server")
END_CMD_OBJ_MAP()

//first entry must help obj
BEGIN_CMD_OBJ_MAP(CfgObj)
	CMD_OBJ_ENTRY(HDJS_HELP_CMD,         C_OBJPROC::Help,     "help info")
	CMD_OBJ_ENTRY(CFG_TRACE_FILE_NAME,   C_OBJPROC::ConfigOP, "cfgfile "CFG_TRACE_FILE)
	CMD_OBJ_ENTRY(CFG_UPCHNNL_FILE_NAME, C_OBJPROC::ConfigOP, "cfgfile "CFG_UPCHNNL_FILE)
	CMD_OBJ_ENTRY(CFG_DNCHNNL_FILE_NAME, C_OBJPROC::ConfigOP, "cfgfile "CFG_DNCHNNL_FILE)
	CMD_OBJ_ENTRY(CFG_HTTX_FILE_NAME,    C_OBJPROC::ConfigOP, "cfgfile "CFG_HTTX_FILE)
END_CMD_OBJ_MAP()

//first entry must help command
BEGIN_CMD_MAP(DjsCmd)
	CMD_ENTRY(DJS_CMD_HELP,  HelpObj, NULL,               NULL,              "print help infomation")
	CMD_ENTRY(DJS_CMD_START, ServObj, Command::BeforeCmd, Command::AfterCmd, "start all processes")
	CMD_ENTRY(DJS_CMD_EXEC,  ExecObj, Command::BeforeCmd, Command::AfterCmd, "exec one process")
	CMD_ENTRY(DJS_CMD_STOP,  ServObj, Command::BeforeCmd, Command::AfterCmd, "stop one process")
	CMD_ENTRY(DJS_CMD_SET,   SetObj,  Command::BeforeCmd, Command::AfterCmd, "set paramteres")
	CMD_ENTRY(DJS_CMD_LIST,  ListObj, Command::BeforeCmd, Command::AfterCmd, "list paramteres")
	CMD_ENTRY(DJS_CMD_LOAD,  CfgObj,  Command::BeforeCmd, Command::AfterCmd, "load config file")
	CMD_ENTRY(DJS_CMD_SAVE,  CfgObj,  Command::BeforeCmd, Command::AfterCmd, "save config file")
END_CMD_MAP()

static char *gDefCmd = DJS_CMD_EXEC;
static char *gDefObj = HDJS_SER_ALLSER;
Command::Command(int argc,char* argv[])
{
	int i;
	for (i = 0; i<argc && i<MAIN_ARGV_MAX; i++)
		m_argv[i] = argv[i];
	if (i == 1) //only command
	{
		m_argv[i++] = gDefCmd;
		m_argv[i++] = gDefObj;
	}
	m_argc = i;
	m_pCmd = DjsCmd;//first help command
	m_pCmdObj = NULL;

	if (m_argc > 1)
	{
		for(CMD* pCmd = DjsCmd; pCmd->m_pCmdName; pCmd++)
		{
			if (strcasecmp(m_argv[1],pCmd->m_pCmdName)==0)
			{
				m_pCmd = pCmd;
				break;
			}
		}
	}
}

CMD_OBJ* Command::GetCmdObj(const CMD* pCmd, const char *pCmdArg)
{
	for(CMD_OBJ* pCmdObj=pCmd->m_pObjects;pCmdObj->m_pName;pCmdObj++)//根据名称完全匹配查找命令对象
	{
		if (strcasecmp(pCmdArg, pCmdObj->m_pName) == 0)
		{
			return pCmdObj;
		}
	}
	for(CMD_OBJ* pCmdObj=pCmd->m_pObjects;pCmdObj->m_pName;pCmdObj++)//根据名称前部分匹配查找命令对象
	{
		if (strncasecmp(pCmdArg, pCmdObj->m_pName, strlen(pCmdObj->m_pName)) == 0)
		{
			return pCmdObj;
		}
	}

	return pCmd->m_pObjects;
}

int Command::BeforeCmd(void)
{
	if (C_ContextOP::InitContext() == false)
		return -1;
	
	return 0;
}

int Command::AfterCmd(void)
{
	return 0;
}

int Command::Exec()
{
	char *pArg;
	if (m_argc > 2)
	{
		m_pCmdObj = GetCmdObj(m_pCmd, m_argv[2]);
		if ((pArg = strchr(m_argv[2], '.')) != NULL)
			pArg += 1;
	}
	else
	{
		m_pCmdObj = m_pCmd->m_pObjects;
		pArg = NULL;
	}

	if (m_pCmdObj == NULL 
		|| strcasecmp(m_pCmd->m_pCmdName, DJS_CMD_EXEC) != 0
		|| ((strcasecmp(m_pCmd->m_pCmdName,DJS_CMD_EXEC)==0)&&(strcasecmp(m_pCmdObj->m_pName,HDJS_SER_DAEMON)==0)))
	{
		logn<<endl;
		logn<<"******************************************"<<endl;
		logn<<"* Power user electric energy acquire system *"<<endl;
		logn<<"* Copyright (c) Haisoft 2012-09-01       *"<<endl;
		logn<<"* Email: jinhailiao@163.com              *"<<endl;
		logn<<"* Ver: "<<GetSoftVersion()<<"               *"<<endl;
		logn<<"******************************************"<<endl;
		logn<<endl;
	}
	if (m_pCmdObj == NULL)
	{
		loge<<"CmdObj is mismatch!"<<endl;
		return -1;
	}

	if (m_pCmd->m_pfnBeforeCmd && m_pCmd->m_pfnBeforeCmd() != 0)
		return -1;

	if (m_pCmdObj->m_pfnCmdProc && m_pCmdObj->m_pfnCmdProc(m_pCmd, m_pCmdObj, pArg) != 0)
		return -1;

	if (m_pCmd->m_pfnAfterCmd && m_pCmd->m_pfnAfterCmd() != 0)
		return -1;

	return 0;
}
#ifndef WIN32
#include <sys/file.h>
#endif
bool Command::MutuxCheck(char *pMutuxName)
{
#ifndef WIN32
	int fd;
	char FileBuf[64];

	sprintf(FileBuf, "/tmp/.%s", pMutuxName);
	if ((fd = open(FileBuf, O_WRONLY|O_CREAT)) == -1)
		return false;
	if (flock(fd, LOCK_EX|LOCK_NB) != 0)
	{
		close(fd);
		return false;
	}
//	close(fd); //if close fd, the file lock will invalid.
#endif
	return true;
}

int C_OBJPROC::Exec(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg)
{
	C_SERVER *pServer = C_SERVFACT::GetServer(pCmdObj->m_pName);
	if (pServer == NULL)
		return -1;

	if (Command::MutuxCheck(pCmdObj->m_pName) == false)
	{//mutux check
		loget<<"The other instance of "<<pCmdObj->m_pName<<" is already running! I will exit!"<<endl;
		return -1;
	}

//	if (pServer->GetCurrentServID() != HDJS_SERID_DAEMON && pServer->IsStartable() == false)
//	{//check if that is inactive myself
//		logwt<<pCmdObj->m_pName<<" is inactived or disabled, will exit!!!"<<endl;
//		return -1;
//	}
	
	logit<<pCmdObj->m_pName<<" start......"<<endl;

	if (pServer->BeforeService() != 0)
		return -1;

	if (pServer->Servicing() != 0)
		return -1;

	if (pServer->AfterService() != 0)
		return -1;

	return 0;
}

int C_OBJPROC::StartAll(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg)
{
	CMD_OBJ *m_pDaemonObj = Command::GetCmdObj(pCmd,HDJS_SER_DAEMON);

	C_ContextOP::SetAllServerActive();
	C_ContextOP::SetStartAllServer();
	return Exec(pCmd, m_pDaemonObj, pArg);
}

#include "haiver.h"
int C_OBJPROC::Help(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg)
{
/*	logn<<"******************************************"<<endl;
	logn<<"* Power user elect energy acquire system *"<<endl;
	logn<<"* Copyright (c) Haisoft 2012-09-01       *"<<endl;
	logn<<"* Email: jinhailiao@163.com              *"<<endl;
	logn<<"* Ver: "<<GetSoftVersion()<<"               *"<<endl;
	logn<<"******************************************"<<endl;
	logn<<endl;
*/
	int i = 0;
	if (strcasecmp(pCmd->m_pCmdName, DJS_CMD_HELP) == 0)
	{
		logn<<"usage: hdjs -command argument"<<endl;
		logn<<endl;

		logn<<"HDJS command list:"<<endl;
		for(CMD* pCmd = DjsCmd; pCmd->m_pCmdName; pCmd++)
		{
			logn<<++i<<": "<<pCmd->m_pCmdName<<"   "<<pCmd->m_pHelp<<endl;
		}
	}
	else
	{
		logn<<"usage: hdjs "<<pCmd->m_pCmdName<<" argument"<<endl;
		logn<<endl;

		logn<<"HDJS argument list:"<<endl;
		for(; pCmdObj->m_pName; pCmdObj++)
		{
			logn<<++i<<": "<<pCmdObj->m_pName<<"   "<<pCmdObj->m_pHelp<<endl;
		}
	}
	return 0;
}

int C_OBJPROC::ServCtrl(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg)
{
	int ServID = C_SERVER::GetServerID(pCmdObj->m_pName);

	if (ServID == HDJS_SERID_MAX)
	{
		logit<<"Unknow Obj: "<<pCmdObj->m_pName<<endl;
	}
	else if (strcasecmp(pCmd->m_pCmdName, DJS_CMD_START) == 0)//-start
	{
		CONTEXT.m_ServInfo[ServID].SetResume();
		logit<<pCmdObj->m_pName<<" server resume OK!!!"<<endl;
	}
	else if (strcasecmp(pCmd->m_pCmdName, DJS_CMD_STOP) == 0)//-stop
	{
		CONTEXT.m_ServInfo[ServID].SetSuspend();
		logit<<pCmdObj->m_pName<<" server suspends OK!!!"<<endl;
	}
	return 0;
}

int C_OBJPROC::ConfigOP(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg)
{
	if (strcasecmp(pCmd->m_pCmdName, DJS_CMD_LOAD) == 0)
		return ConfigLoad(pCmd, pCmdObj, pArg);
	else if (strcasecmp(pCmd->m_pCmdName, DJS_CMD_SAVE) == 0)
		return ConfigSave(pCmd, pCmdObj, pArg);
	
	logwt<<"Unknow command: "<<pCmd->m_pCmdName<<endl;
	return 0;
}

int C_OBJPROC::SetParam(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg)
{
	return 0;
}

int C_OBJPROC::ListParam(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg)
{
	return 0;
}

int C_OBJPROC::ConfigLoad(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg)
{
	S_CONTEXT *pContext = C_ContextOP::GetContext();

	if (strcasecmp(pCmdObj->m_pName, CFG_TRACE_FILE_NAME) == 0)
	{
		C_CFGTRACE::Load(pContext->m_TraceSet);
	}
	else if (strcasecmp(pCmdObj->m_pName, CFG_UPCHNNL_FILE_NAME) == 0)
	{
		C_CFGUCHNNL::Load(pContext->m_UpChannel, CHANNEL_UP_TOTAL_MAX);
	}
	else if (strcasecmp(pCmdObj->m_pName, CFG_DNCHNNL_FILE_NAME) == 0)
	{
		C_CFGDCHNNL::Load(pContext->m_DnChannel, CHANNEL_DN_TOTAL_MAX);
	}
	else if (strcasecmp(pCmdObj->m_pName, CFG_HTTX_FILE_NAME) == 0)
	{
		C_CFGHTTX::Load(pContext->m_HttxInfo, HTTX_ID_MAX);
	}

	return 0;
}

int C_OBJPROC::ConfigSave(CMD *pCmd, CMD_OBJ *pCmdObj, char *pArg)
{
	S_CONTEXT *pContext = C_ContextOP::GetContext();

	if (strcasecmp(pCmdObj->m_pName, CFG_TRACE_FILE_NAME) == 0)
	{
		C_CFGTRACE::Save(pContext->m_TraceSet);
	}
	else if (strcasecmp(pCmdObj->m_pName, CFG_UPCHNNL_FILE_NAME) == 0)
	{
		C_CFGUCHNNL::Save(pContext->m_UpChannel, CHANNEL_UP_TOTAL_MAX);
	}
	else if (strcasecmp(pCmdObj->m_pName, CFG_DNCHNNL_FILE_NAME) == 0)
	{
		C_CFGDCHNNL::Save(pContext->m_DnChannel, CHANNEL_DN_TOTAL_MAX);
	}
	else if (strcasecmp(pCmdObj->m_pName, CFG_HTTX_FILE_NAME) == 0)
	{
		C_CFGHTTX::Save(pContext->m_HttxInfo, HTTX_ID_MAX);
	}

	return 0;
}






