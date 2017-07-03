//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			context.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-13  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __CONTEXT_H__
#define __CONTEXT_H__
#include "htrace.h"
#include "server.h"
#include "sharemem.h"
#include "daemon.h"
#include "channel.h"
#include "httx.h"
#include "ppp.h"

/** @class S_SYSCFG
 *  @brief 系统配置结构体
 */
struct S_SYSCFG
{
	/** 终端地址
	 */
	char m_RTUA[4]; ///< 终端地址=4521000A BCD+HEX

	/** 登陆信息
	 */
	char m_SysUser[20];
	char m_SysPassword[20];

	/** 版本信息
	 */
	char m_ReleaseDate[32];
	char m_HWVersion[32];
	char m_SWVersion[32];

	/** 配置信息
	 */
	bool m_GatherMeterEnable; ///< 全局采表使能标志
	INT16U m_VipPointPerDay; ///< 重点户每天须要采的点数,24的整数倍
	INT16U m_HeartBeatFailedTimes;//心跳次数  次

	void Init(void)
	{
		memset(this, 0x00, sizeof(*this));
		m_GatherMeterEnable = true;
		m_VipPointPerDay = 24;
		m_HeartBeatFailedTimes = 3;
	}
	/** @brief 设置终端地址 */
	void SetRTUA(const char *pRTUA);
	/** @brief 获取终端地址 */
	char *GetRTUAstr(char *pRTUA);
};

//
//
//
struct S_MP
{
	enum MP_ATTR{MP_ATTR_NONE,MP_ATTR_METER,MP_ATTR_PULSE,MP_ATTR_ACPORT};
	enum{MP_ATTR_BITS=4,MP_DEV_SN=12,MP_ATTR_M=0xF000,MP_DEV_M=0x0FFF};

	S_WORD m_attrb;

public:
	S_MP(void){m_attrb = 0;}
	S_MP(S_WORD DevT, S_WORD DevSN){m_attrb = (DevT<<MP_DEV_SN)|(DevSN&MP_DEV_M);}
	S_MP(S_MP &r){if (this != &r) m_attrb = r.m_attrb;}
	~S_MP(){}

	S_WORD GetMpAttr(void){return m_attrb >> MP_DEV_SN;}
	S_WORD GetMpDevSN(void){return m_attrb & MP_DEV_M;}

	bool operator==(S_MP &r){return m_attrb == r.m_attrb;}
	S_MP &operator=(S_MP &r){if (this != &r) m_attrb = r.m_attrb; return *this;}
};

struct S_MPS
{
	S_MP m_MP[GW_MP_MAX];

public:
	S_MPS(void){}
	~S_MPS(){}

	S_MP& operator[](int Index){return m_MP[Index];}
	S_MP  GetMP(int Index){return m_MP[Index];}

	bool RegisterMP(int MP, S_WORD DevT, S_WORD DevSN);
	bool UnregisterMP(int MP, S_WORD DevT=0xFFFF, S_WORD DevSN=0xFFFF);
};


#define CFG_FILE_CONTEXT_FILE	CFG_FILE_BASE_PATH"ramdisk/context.dat"
//
//
//
typedef struct _tagContext
{
	C_MEMSIGN  m_ContextSign;
	S_SYSCFG   m_SysConfig;
	S_TRACESET m_TraceSet[HDJS_SERID_MAX];
	S_SERVINFO m_ServInfo[HDJS_SERID_MAX];

	S_CHANNEL  m_UpChannel[CHANNEL_UP_TOTAL_MAX];
	S_CHANNEL  m_DnChannel[CHANNEL_DN_TOTAL_MAX];

	S_DAEMONINFO m_DaemonInfo;
	S_HTTXINFO   m_HttxInfo[HTTX_ID_MAX];
	S_PPPINFO    m_PppInfo;

	S_FORWARD m_forward;
	S_MSCONNECTSTAT m_ConnectStat;

	bool init(void);
}S_CONTEXT, *SP_CONTEXT;


//
//
//
class C_ContextOP
{
public:
	static bool InitContext(void);
	static S_CONTEXT& instance(void);
	static S_CONTEXT* GetContext(void);
	static bool SetAllServerActive(void);
	static bool SaveContext(void);
	static bool SaveContext(void *pstart, int size);
	static bool LoadConfig(void);
	static bool ParamParse(void);
	static bool SetContextNewest(void){m_ContextNew=true; return true;}
	static bool IsContextNewest(void){return m_ContextNew==true;}
	static bool SetStartAllServer(void){m_StartAllServer=true; return true;}
	static bool IsStartAllServer(void){return m_StartAllServer==true;}
	static S_CHANNEL* FindUpChannel(const char *pChannelName)
	{
		return FindChannel(m_pContext->m_UpChannel, CHANNEL_UP_TOTAL_MAX, pChannelName);
	}
	static S_CHANNEL* FindDnChannel(const char *pChannelName)
	{
		return FindChannel(m_pContext->m_DnChannel, CHANNEL_DN_TOTAL_MAX, pChannelName);
	}

private:
	static S_CHANNEL* FindChannel(S_CHANNEL *pChannel, int size, const char *pChannelName);

private:
    static bool m_ContextNew;///< only use for daemon
	static bool m_StartAllServer;///< only use for daemon
	static SP_CONTEXT m_pContext;
	static C_SHAREMEM m_ShareMem;
};

#define CONTEXT C_ContextOP::instance()


#endif //__CONTEXT_H__


