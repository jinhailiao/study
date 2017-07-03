//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			server.cpp
// Description:		the father of the all server
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-13  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "server.h"
#include "haicfg.h"
#include "haidef.h"

int C_SERVER::CurServID = 0;

const char * const gSerName[] =
{
	HDJS_SER_DYJC,
	HDJS_SER_HTTX1,
	HDJS_SER_HTTX2,
	HDJS_SER_HTTX3,
	HDJS_SER_DBCJ,
	HDJS_SER_PPP,
	HDJS_SER_DMAN,
	HDJS_SER_UIMAN
};

const char *C_SERVER::GetServerName(int ServerID)
{
	return gSerName[ServerID];
}

int C_SERVER::GetServerID(const char *ServerName)
{
	for (int i = 0; i < HDJS_SERID_MAX; i++)
	{
		if (strcasecmp(gSerName[i], ServerName) == 0)
			return i;
	}

	if (strcasecmp(HDJS_SER_DAEMON, ServerName) == 0)
		return HDJS_SERID_DAEMON;

	return HDJS_SERID_MAX;
}

C_SERVER::C_SERVER(void):m_pServInfo(NULL)
{
}

C_SERVER::~C_SERVER()
{
}

bool C_SERVER::HeartBeat(void)
{
	if (HeartBeatable() == true)
	{
		m_pServInfo->m_LHBTime = time(NULL);
		return true;
	}
	return false;
}


//
//server information
//
S_SERVINFO::S_SERVINFO(void)
{
	memset(this, 0x00, sizeof(*this));
}

S_SERVINFO::S_SERVINFO(S_BYTE ServID)
{
	m_Enabled = true;
	m_ServID  = ServID;
	m_Mode    = SERVER_MODE_INACTIVED;///< 模式,参见SERVER_MODE,缺省非活动
	m_Status  = SERVER_STATUS_STOPPED;///< 参见SERVER_STATUS
	m_DeadCnt = 0;///< 心跳停止的次数
	m_ExeCnt = 0;///< 被执行的次数,每被fork一次就加1
	m_HBInterS  = SERVER_HEART_BEAT_INTERS;///< 心跳间隔
	m_LHBTime   = time(NULL);///< 最后心跳时间
	m_PID       = 0;///< 进程ID
}

S_SERVINFO::~S_SERVINFO()
{
}

bool S_SERVINFO::IsHBTimeout(time_t &tNow) const
{
	if (m_LHBTime>0 && m_HBInterS>0)
	{
		if ((tNow-m_LHBTime) > m_HBInterS*3)//three times
		{
			loget<<C_SERVER::GetServerName(m_ServID)<<"'s heart beat is not normal(LHBTime="<<(unsigned long)m_LHBTime<<",NowTime="<<(unsigned long)tNow<<"), like death!!!"<<endl;
			return true;
		}
	}

	return false;
}

void S_SERVINFO::SetStoppingStatus(void)
{
	m_Status = SERVER_STATUS_STOPPING;
}

void S_SERVINFO::SetStopedStatus(void)
{
	m_PID = -1;
	m_Status = SERVER_STATUS_STOPPED;
}

void S_SERVINFO::SetStartedStatus(PID_T pid)
{
	m_PID = pid;
	m_ExeCnt++;
	m_LHBTime = time(NULL);
	m_Status = SERVER_STATUS_RUNNING;
}




