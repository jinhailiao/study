//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			cfgfile.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-13  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __CFGFILE_H__
#define __CFGFILE_H__
#include "haidef.h"
#include "haicfg.h"
#include "channel.h"
#include "httx.h"
#include "ppp.h"
#include "context.h"

#define CFG_FILE_SUFFIX		".ini"
#define CFG_FILE_SYS_PATH	CFG_FILE_BASE_PATH"system/"
#define CFG_DEF_SET_PATH	CFG_FILE_BASE_PATH"dyjc/set/default/"
#define CFG_USR_SET_PATH	CFG_FILE_BASE_PATH"dyjc/set/user/"

#define CFG_TRACE_FILE_NAME	"trace"
#define CFG_TRACE_FILE		CFG_TRACE_FILE_NAME CFG_FILE_SUFFIX
#define CFG_UPCHNNL_FILE_NAME	"upchannl"
#define CFG_UPCHNNL_FILE	CFG_UPCHNNL_FILE_NAME CFG_FILE_SUFFIX
#define CFG_DNCHNNL_FILE_NAME	"dnchannl"
#define CFG_DNCHNNL_FILE	CFG_DNCHNNL_FILE_NAME CFG_FILE_SUFFIX
#define CFG_HTTX_FILE_NAME	"httx"
#define CFG_HTTX_FILE	CFG_HTTX_FILE_NAME CFG_FILE_SUFFIX
#define CFG_PPP_FILE_NAME	"ppp"
#define CFG_PPP_FILE	CFG_PPP_FILE_NAME CFG_FILE_SUFFIX
#define CFG_FORWARD_FILE_NAME	"forward"
#define CFG_FORWARD_FILE	CFG_FORWARD_FILE_NAME CFG_FILE_SUFFIX
#define CFG_SYSCFG_FILE_NAME	"syscfg"
#define CFG_SYSCFG_FILE	CFG_SYSCFG_FILE_NAME CFG_FILE_SUFFIX

class C_CFGTRACE
{
public:
	static int Load(S_TRACESET *pServerTrace, const char *pFileName = CFG_TRACE_FILE);
	static int Save(S_TRACESET *pServerTrace, const char *pFileName = CFG_TRACE_FILE);

private:
	static S_BYTE MakeLevelVal(const char *pkey);
	static const char *MakeLevelKey(S_BYTE val);
	static S_BYTE MakeWayVal(const char *pkey);
	static const char *MakeWayKey(S_BYTE val);
};

class C_CFGUCHNNL
{
public:
	static int Load(S_CHANNEL *pChannel, int ChannlSize, const char *pFileName = CFG_UPCHNNL_FILE);
	static int Save(S_CHANNEL *pChannel, int ChannlSize, const char *pFileName = CFG_UPCHNNL_FILE);

private:
	static int ParseNetParam(S_NETSET &NetSet, const char *pSetStr);
	static int ParseComParam(S_COMMSET &ComSet, const char *pSetStr);

	static S_BYTE MakeChannelTypeVal(const char *pkey);
	static S_BYTE MakeChannelNetConnectVal(const char *pkey);
	static S_BYTE MakeChannelNetPppVal(const char *pkey);
	static const char* MakeChannelTypeKey(S_BYTE val);
	static const char* MakeChannelNetConnectKey(S_BYTE val);
	static const char* MakeChannelNetPppKey(S_BYTE val);
	static S_BYTE MakeChannelComCDval(const char *pkey);
	static const char* MakeChannelComCDkey(S_BYTE val);
};

//
//dnchannl.ini
//
class C_CFGDCHNNL
{
public:
	static int Load(S_CHANNEL *pChannel, int ChannlSize, const char *pFileName = CFG_DNCHNNL_FILE)
	{
		return C_CFGUCHNNL::Load(pChannel, ChannlSize, pFileName);
	}
	static int Save(S_CHANNEL *pChannel, int ChannlSize, const char *pFileName = CFG_DNCHNNL_FILE)
	{
		return C_CFGUCHNNL::Save(pChannel, ChannlSize, pFileName);
	}
};

//
//httx.ini
//
class C_CFGHTTX
{
public:
	static int Load(S_HTTXINFO *pHttxInfo, int InfoSize, const char *pFileName = CFG_HTTX_FILE);
	static int Save(S_HTTXINFO *pHttxInfo, int InfoSize, const char *pFileName = CFG_HTTX_FILE);
};

//
//ppp.ini
//
class C_CFGPPP
{
public:
	static int Load(S_PPPINFO &PppInfo, const char *pFileName = CFG_PPP_FILE);
	static int Save(S_PPPINFO &PppInfo, const char *pFileName = CFG_PPP_FILE);
};

//
//forward.ini
//
class C_CFGFORWARD
{
public:
	static int Load(S_FORWARD &ForwardInfo, const char *pFileName = CFG_FORWARD_FILE);
	static int Save(S_FORWARD &ForwardInfo, const char *pFileName = CFG_FORWARD_FILE);

private:
	static bool GetMasterForward(const char *pStrMask);
	static char *GetMasterForwardStr(bool Master);
};

//
//syscfg.ini
//
class C_CFGSYSTEM
{
public:
	static int Load(S_SYSCFG &SysConfig, const char *pFileName = CFG_SYSCFG_FILE);
	static int Save(S_SYSCFG &SysConfig, const char *pFileName = CFG_SYSCFG_FILE);
};



//
//public cfg function
//
class C_CFGFILE
{
public:
	static int GetSysFile(char *pFileBuf, const char *pFileName);
	static int GetDefSetFile(char *pFileBuf, const char *pFileName);
	static int GetUserSetFile(char *pFileBuf, const char *pFileName);

	static int MakeValue(const char *pkey, const char **pkeys, int size);
	static const char *MakeKey(int val, const char **pkeys, int size);

	static const char *GetBoolKey(S_BOOL val);
	static int ParseString(strings &strs, const string &str, char ch);
};

#endif //__CFGFILE_H__
