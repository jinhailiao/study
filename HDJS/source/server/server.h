//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			server.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-13  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SERVER_H__
#define __SERVER_H__
#include "haidef.h"
#include "haicfg.h"
#include "osrelate.h"
#include <time.h>

#define HDJS_SER_DYJC "dyjc"
#define HDJS_SER_HTTX1	"httx1"
#define HDJS_SER_HTTX2	"httx2"
#define HDJS_SER_HTTX3	"httx3"
#define HDJS_SER_DBCJ	"dbcj"
#define HDJS_SER_PPP	"ppp"
#define HDJS_SER_DMAN	"dman"
#define HDJS_SER_UIMAN	"uiman"

#define HDJS_SER_DAEMON	"daemon"
#define HDJS_SER_ALLSER	"All"

#define SERVER_RETRY_TIMES	10

//must keep order with gSerName
enum
{
	HDJS_SERID_DAEMON, //must first
	HDJS_SERID_DYJC = HDJS_SERID_DAEMON,
	HDJS_SERID_HTTX1,
	HDJS_SERID_HTTX2,
	HDJS_SERID_HTTX3,
	HDJS_SERID_DBCJ,
	HDJS_SERID_PPP,
	HDJS_SERID_DMAN,
	HDJS_SERID_UIMAN,

	HDJS_SERID_MAX
};


//
//server information
//
enum
{
	SERVER_MODE_ACTIVED,
	SERVER_MODE_INACTIVED,
	SERVER_MODE_SUSPENDED
};
enum
{
	SERVER_STATUS_STOPPED,//未运行
	SERVER_STATUS_RUNNING,//正在运行
	SERVER_STATUS_STOPPING//正在停止
};
struct S_SERVINFO
{
	bool   m_Enabled;
	S_BYTE m_ServID;
	S_BYTE m_Mode;///< 模式,参见SERVER_MODE
	S_BYTE m_Status;///< 参见SERVER_STATUS
	S_BYTE m_DeadCnt;///< 心跳停止的次数，当心跳连续停止次数超过CONS_MAX_DEADCOUNT时，将视为此进程已死
	S_WORD m_ExeCnt;///< 被执行的次数,每被fork一次就加1
	S_WORD m_HBInterS;///< 心跳间隔
	time_t m_LHBTime;///< 最后心跳时间
	PID_T  m_PID;///< 进程ID

public:
	S_SERVINFO(void);
	S_SERVINFO(S_BYTE ServID);
	~S_SERVINFO();

public:
	bool IsStartable(void) const;
	S_BYTE GetServerID(void) const;
	bool IsKillable(void) const;
	void SetActive(void);
	void SetInactive(void);
	void SetSuspend(void);
	void SetResume(void);
	void SetStoppingStatus(void);
	void SetStopedStatus(void);
	void SetStartedStatus(PID_T pid);
	bool IsRunning() const;
	bool IsHBTimeout(time_t& tNow) const;
};
inline bool S_SERVINFO::IsStartable(void) const
{
	return (m_Enabled==true && m_Mode==SERVER_MODE_ACTIVED && m_Status==SERVER_STATUS_STOPPED);
}
inline S_BYTE S_SERVINFO::GetServerID(void) const
{
	return m_ServID;
}
inline bool S_SERVINFO::IsKillable(void) const
{
	return (m_Enabled==false || m_Mode!=SERVER_MODE_ACTIVED);
}
inline void S_SERVINFO::SetActive(void)
{
	m_Mode=SERVER_MODE_ACTIVED;
}
inline void S_SERVINFO::SetInactive(void)
{
	m_Mode=SERVER_MODE_INACTIVED;
}
inline void S_SERVINFO::SetSuspend(void)
{
	if (m_Mode==SERVER_MODE_ACTIVED)
	{
		m_Mode=SERVER_MODE_SUSPENDED;
	}
}
inline void S_SERVINFO::SetResume(void)
{
	if (m_Mode==SERVER_MODE_SUSPENDED)
	{
		m_Mode=SERVER_MODE_ACTIVED;
	}		
}
inline bool S_SERVINFO::IsRunning() const
{
	return (m_Status==SERVER_STATUS_RUNNING && m_PID>0);
}

//
//the father of all server
//
class C_SERVER
{
public:
	C_SERVER(void);
	virtual ~C_SERVER();

	static const char *GetServerName(int ServerID);
	static int GetServerID(const char *ServerName);
	static int GetCurrentServID(void){return CurServID;}
	static void SetCurrentServId(int ServerID){CurServID = ServerID;}

	virtual int BeforeService(void) = 0;
	virtual int Servicing(void) = 0;
	virtual int AfterService(void) = 0;

	bool HeartBeat(void);
	bool HeartBeatable(void){return m_pServInfo->m_HBInterS>0;}
	bool IsStartable(void){return m_pServInfo->IsStartable();}

protected:
	S_SERVINFO *m_pServInfo;

private:
	static int CurServID;
};

#endif //__SERVER_H__


