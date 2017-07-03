//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			cfgfile.cpp
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-14  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "cfgfile.h"
#include "confuse/confuse.h"
#include "server.h"
#include "haiver.h"
#include <fstream>

#include "haicfg.h"
using std::ofstream;
using std::ios;

//
//trace.ini
//
cfg_opt_t OPT_TRACE_SETTINGS[] =
{
	CFG_BOOL("Enable", cfg_false,CFGF_NOCASE),
	CFG_STR ("Level", "Warn", CFGF_NOCASE),
	CFG_STR ("Way", "None", CFGF_NOCASE),
	CFG_INT ("FileSize", 50000, CFGF_NOCASE),//默认50K
	CFG_INT ("FileTotal", 2, CFGF_NOCASE),
	CFG_END()
};
cfg_opt_t OPT_TRACES_SETTINGS[] =
{
	CFG_SEC("trace", OPT_TRACE_SETTINGS, CFGF_MULTI|CFGF_TITLE|CFGF_NOCASE),
	CFG_END()
};

const char* TraceLevelKeys[6]={"Disabled","Error","Warn","Info","Verbose","Debug"};
const char* TraceWayKeys[10]={"None","Stdout","Logfile","ttyp0","ttyp1","ttyp2","ttyp3","ttyp4","ttyp5","ttyp6"};

S_BYTE C_CFGTRACE::MakeLevelVal(const char* pkey)
{
	return (S_BYTE)C_CFGFILE::MakeValue(pkey, TraceLevelKeys, sizeof(TraceLevelKeys)/sizeof(TraceLevelKeys[0]));
}

const char* C_CFGTRACE::MakeLevelKey(S_BYTE val)
{
	return C_CFGFILE::MakeKey(val, TraceLevelKeys, sizeof(TraceLevelKeys)/sizeof(TraceLevelKeys[0]));
}

S_BYTE C_CFGTRACE::MakeWayVal(const char* pkey)
{
	return (S_BYTE)C_CFGFILE::MakeValue(pkey, TraceWayKeys, sizeof(TraceWayKeys)/sizeof(TraceWayKeys[0]));
}
const char* C_CFGTRACE::MakeWayKey(S_BYTE val)
{
	return C_CFGFILE::MakeKey(val, TraceWayKeys, sizeof(TraceWayKeys)/sizeof(TraceWayKeys[0]));
}

int C_CFGTRACE::Load(S_TRACESET *pServerTrace, const char *pFileName)
{
	int err = 0;
	S_CHAR cfgFile[260];
	C_CFGFILE::GetSysFile(cfgFile, pFileName);

	cfg_t* cfg = cfg_init(OPT_TRACES_SETTINGS, CFGF_NONE);
	if (cfg_parse(cfg,cfgFile) == CFG_SUCCESS)
	{
		for(unsigned int i = 0; i < cfg_size(cfg, "trace"); i++)
		{
			cfg_t* cfgsec = cfg_getnsec(cfg, "trace", i);
			if (cfg_getbool(cfgsec, "Enable") != cfg_true)
				continue;
			int id = C_SERVER::GetServerID(cfg_title(cfgsec));
			if (id != HDJS_SERID_MAX)
			{
				pServerTrace[id].SetTitle((const unsigned char *)C_SERVER::GetServerName(id));
				pServerTrace[id].SetLevel((char)MakeLevelVal(cfg_getstr(cfgsec, "Level")));
				pServerTrace[id].SetWay((char)MakeWayVal(cfg_getstr(cfgsec, "Way")));
				pServerTrace[id].SetFileSize(cfg_getint(cfgsec, "FileSize"));
				pServerTrace[id].SetFileTotal((unsigned char)cfg_getint(cfgsec, "FileTotal"));
			}
		}	
		logit<<cfgFile<<" loads OK!"<<endl;
	}
	else
	{
		logwt<<cfgFile<<" loads failed!"<<endl;
		err = -1;
	}
	cfg_free(cfg);

	return err;
}

int C_CFGTRACE::Save(S_TRACESET *pServerTrace, const char *pFileName)
{
	char buffer[260];
	string FileBuf;
	FileBuf.reserve(2400);
	FileBuf.append("#file ").append(pFileName).append("\n""#path /mnt/system\n""#function 服务进程日志配置\n");
	FileBuf.append("#help:\n""#Level=Disabled,Error,Warn,Info,Verbose,Debug\n""#Way=logfile,stdout,ttyp0,ttyp1,ttyp2,ttyp3,ttyp4,ttyp5,ttyp6\n\n");
	for (int i = HDJS_SERID_DAEMON; i < HDJS_SERID_MAX; i++)
	{
		FileBuf.append("trace ").append((char *)pServerTrace[i].GetTitle()).append("\n""{\n");
		FileBuf.append("\t""Enable=").append("true\n");
		FileBuf.append("\t""Level=").append(MakeLevelKey(pServerTrace[i].GetLevel())).append("\n");
		FileBuf.append("\t""Way=").append(MakeWayKey(pServerTrace[i].GetWay())).append("\n");
		sprintf(buffer, "%d", pServerTrace[i].GetFileSize());
		FileBuf.append("\t""FileSize=").append(buffer).append("\n");
		sprintf(buffer, "%d", pServerTrace[i].GetFileTotal());
		FileBuf.append("\t""FileTotal=").append(buffer).append("\n");
		FileBuf.append("}\n\n");
	}

	C_CFGFILE::GetSysFile(buffer, pFileName);
	ofstream outfile(buffer, ios::out);
	if (!outfile)
		logwt<<buffer<<" saves failed!!!"<<endl;
	else
	{
		outfile<<FileBuf;
		logit<<buffer<<" saves OK!!!"<<endl;
	}
	return 0;
}

//
//upchannl.ini
//
cfg_opt_t OPT_UP_CHANNL[] =
{
	CFG_BOOL("Enable", cfg_false,CFGF_NOCASE),
	CFG_STR ("Name", "MS1", CFGF_NOCASE),
	CFG_STR ("Type", "Net", CFGF_NOCASE),
	CFG_STR ("Setting", "192.168.9.10:10000,TcpServer,NeedPPP", CFGF_NOCASE),
	CFG_INT ("ReadTimeoutMS", 3000, CFGF_NOCASE),
	CFG_INT ("WriteTimeoutMS", 1000, CFGF_NOCASE),
	CFG_INT ("SubReadTimeoutMS", 20, CFGF_NOCASE),
	CFG_INT ("CharReadTimeoutMS", 100, CFGF_NOCASE),
	CFG_INT ("ConnectTimeoutS", 15, CFGF_NOCASE),
	CFG_INT ("ConnectInterS", 60, CFGF_NOCASE),
	CFG_INT ("IdleTimeoutS", 0, CFGF_NOCASE),
	CFG_INT ("PackageMaxLen", 1400, CFGF_NOCASE),
	CFG_END()
};
cfg_opt_t OPT_UP_CHANNLS[] =
{
	CFG_SEC("Channel", OPT_UP_CHANNL, CFGF_MULTI|CFGF_NOCASE),
	CFG_END()
};

const char* ChannelTypeKeys[3] = {"","Net","Com"};
const char* ChannelConnKeys[6] = {"Tcp","Udp","TcpServer","TcpClient","UdpServer","UdpClient"};
const char* ChannelPPPKeys[2]  = {"NoPPP","NeedPPP"};
const char* ChannelCDKeys[2]   = {"NoCD","NeedCD"};

S_BYTE C_CFGUCHNNL::MakeChannelTypeVal(const char *pkey)
{
	return (S_BYTE)C_CFGFILE::MakeValue(pkey, ChannelTypeKeys, sizeof(ChannelTypeKeys)/sizeof(ChannelTypeKeys[0]));
}
S_BYTE C_CFGUCHNNL::MakeChannelNetConnectVal(const char *pkey)
{
	return (S_BYTE)C_CFGFILE::MakeValue(pkey, ChannelConnKeys, sizeof(ChannelConnKeys)/sizeof(ChannelConnKeys[0]));
}
S_BYTE C_CFGUCHNNL::MakeChannelNetPppVal(const char *pkey)
{
	return (S_BYTE)C_CFGFILE::MakeValue(pkey, ChannelPPPKeys, sizeof(ChannelPPPKeys)/sizeof(ChannelPPPKeys[0]));
}
const char* C_CFGUCHNNL::MakeChannelTypeKey(S_BYTE val)
{
	return C_CFGFILE::MakeKey(val, ChannelTypeKeys, sizeof(ChannelTypeKeys)/sizeof(ChannelTypeKeys[0]));
}
const char* C_CFGUCHNNL::MakeChannelNetConnectKey(S_BYTE val)
{
	return C_CFGFILE::MakeKey(val, ChannelConnKeys, sizeof(ChannelConnKeys)/sizeof(ChannelConnKeys[0]));
}
const char* C_CFGUCHNNL::MakeChannelNetPppKey(S_BYTE val)
{
	return C_CFGFILE::MakeKey(val, ChannelPPPKeys, sizeof(ChannelPPPKeys)/sizeof(ChannelPPPKeys[0]));
}
S_BYTE C_CFGUCHNNL::MakeChannelComCDval(const char *pkey)
{
	return (S_BYTE)C_CFGFILE::MakeValue(pkey, ChannelCDKeys, sizeof(ChannelCDKeys)/sizeof(ChannelCDKeys[0]));
}
const char* C_CFGUCHNNL::MakeChannelComCDkey(S_BYTE val)
{
	return C_CFGFILE::MakeKey(val, ChannelCDKeys, sizeof(ChannelCDKeys)/sizeof(ChannelCDKeys[0]));
}

int C_CFGUCHNNL::ParseNetParam(S_NETSET &NetSet, const char *pSetStr)
{
	int ip1, ip2, ip3, ip4, port;
	strings SetStrs;
	C_CFGFILE::ParseString(SetStrs, (string)pSetStr, ',');
	
	if (SetStrs.size() < 3)
		return -1;

	sscanf(SetStrs[0].c_str(), "%d.%d.%d.%d:%d", &ip1,&ip2,&ip3,&ip4,&port);
	NetSet.m_IP[0]  = (S_BYTE)ip1,NetSet.m_IP[1] = (S_BYTE)ip2,NetSet.m_IP[2] = (S_BYTE)ip3,NetSet.m_IP[3] = (S_BYTE)ip4;
	NetSet.m_PortNO = (S_WORD)port;
	NetSet.m_Type   = MakeChannelNetConnectVal(SetStrs[1].c_str());
	NetSet.m_NeedPPP = MakeChannelNetPppVal(SetStrs[2].c_str());

	return 0;
}
int C_CFGUCHNNL::ParseComParam(S_COMMSET &ComSet, const char *pSetStr)
{
	int baud, parity, stop, databit, port;
	strings SetStrs;
	C_CFGFILE::ParseString(SetStrs, (string)pSetStr, ',');
	
	if (SetStrs.size() < 4)
		return -1;

	sscanf(SetStrs[0].c_str(), "%d", &port);
	sscanf(SetStrs[1].c_str(), "%d.%d.%d.%d", &baud,&databit,&stop,&parity);
	ComSet.m_PortNO  = port;
	ComSet.m_Baud    = baud;
	ComSet.m_DataBit = databit;
	ComSet.m_StopBit = stop;
	ComSet.m_Parity  = parity;
	//SetStrs[2] = "serial"
	ComSet.m_CDCheck = MakeChannelComCDval(SetStrs[3].c_str());
	return 0;
}

int C_CFGUCHNNL::Load(S_CHANNEL *pChannel, int ChannlSize, const char *pFileName)
{
	int err = 0;
	S_CHAR cfgFile[260];

	cfg_t* cfg = cfg_init(OPT_UP_CHANNLS, CFGF_NONE);
	C_CFGFILE::GetUserSetFile(cfgFile, pFileName);

	if ((err = cfg_parse(cfg,cfgFile)) != CFG_SUCCESS)
	{
		C_CFGFILE::GetDefSetFile(cfgFile, pFileName);
		err = cfg_parse(cfg,cfgFile);
	}

	if (err == CFG_SUCCESS)
	{
		int NO = 0;
		for(unsigned int i = 0; i < cfg_size(cfg, "Channel"); i++)
		{
			cfg_t* cfgsec = cfg_getnsec(cfg, "Channel", i);
			if (cfg_getbool(cfgsec, "Enable") != cfg_true)
				continue;
			if (NO < ChannlSize)
			{
				pChannel[NO].m_Enable = true;
				strncpy((char *)pChannel[NO].m_Name, cfg_getstr(cfgsec, "Name"), sizeof(pChannel[0].m_Name)-2);
				pChannel[NO].m_Type = MakeChannelTypeVal(cfg_getstr(cfgsec, "Type"));
				if (pChannel[NO].m_Type == CHANNEL_TYPE_NET)
					ParseNetParam(pChannel[NO].m_PortSet.m_NetSet, cfg_getstr(cfgsec, "Setting"));
				else if (pChannel[NO].m_Type == CHANNEL_TYPE_COMM)
					ParseComParam(pChannel[NO].m_PortSet.m_CommSet, cfg_getstr(cfgsec, "Setting"));
				pChannel[NO].m_ReadTimeoutMS  = (S_WORD)cfg_getint(cfgsec, "ReadTimeoutMS");
				pChannel[NO].m_WriteTimeoutMS = (S_WORD)cfg_getint(cfgsec, "WriteTimeoutMS");
				pChannel[NO].m_SubReadTimeoutMS = (S_WORD)cfg_getint(cfgsec, "SubReadTimeoutMS");
				pChannel[NO].m_CharReadTimeoutMS= (S_WORD)cfg_getint(cfgsec, "CharReadTimeoutMS");
				pChannel[NO].m_ConnectTimeoutS = (S_BYTE)cfg_getint(cfgsec, "ConnectTimeoutS");
				pChannel[NO].m_ConnectInterS = (S_WORD)cfg_getint(cfgsec, "ConnectInterS");
				pChannel[NO].m_IdleTimeoutS  = (S_WORD)cfg_getint(cfgsec, "IdleTimeoutS");
				pChannel[NO].m_PackageMaxLen = (S_WORD)cfg_getint(cfgsec, "PackageMaxLen");
				++NO;
			}
		}	
		logit<<cfgFile<<" loads OK!"<<endl;
		err = 0;
	}
	else
	{
		logwt<<cfgFile<<" loads failed!"<<endl;
		err = -1;
	}
	cfg_free(cfg);

	return err;
}

int C_CFGUCHNNL::Save(S_CHANNEL *pChannel, int ChannlSize, const char *pFileName)
{
	char buffer[260];
	string FileBuf;
	FileBuf.reserve(2400);

	FileBuf.append("#file ").append(pFileName).append("\n""#path /mnt/dyjc/set/default /mnt/dyjc/set/user\n""#function 集中器上行或下行通道配置\n");
	FileBuf.append("#help:\n""#Type=Net,Com\n""#NetConnect=TcpClient,TcpServer,UdpClient,UdpServer,Tcp,Udp\n""#PPP=NeedPPP,NoPPP\n""#CD=NeedCP,NoCD\n\n");
	for (int i = 0; i < ChannlSize; i++)
	{
		if (pChannel[i].m_Enable != (S_BYTE)true)
			continue;
		FileBuf.append("Channel\n""{\n");
		FileBuf.append("\t""Enable=").append("true\n");
		FileBuf.append("\t""Name=").append((char *)pChannel[i].m_Name).append("\n");
		FileBuf.append("\t""Type=").append(MakeChannelTypeKey(pChannel[i].m_Type)).append("\n");
		if (pChannel[i].m_Type == CHANNEL_TYPE_NET)
			sprintf(buffer, "\"%d.%d.%d.%d:%d,%s,%s\"", pChannel[i].m_PortSet.m_NetSet.m_IP[0],pChannel[i].m_PortSet.m_NetSet.m_IP[1],pChannel[i].m_PortSet.m_NetSet.m_IP[2],pChannel[i].m_PortSet.m_NetSet.m_IP[3],pChannel[i].m_PortSet.m_NetSet.m_PortNO,MakeChannelNetConnectKey(pChannel[i].m_PortSet.m_NetSet.m_Type),MakeChannelNetPppKey(pChannel[i].m_PortSet.m_NetSet.m_NeedPPP));
		else if (pChannel[i].m_Type == CHANNEL_TYPE_COMM)
			sprintf(buffer, "\"%d,%d.%d.%d.%d,Serial,%s\"", pChannel[i].m_PortSet.m_CommSet.m_PortNO,pChannel[i].m_PortSet.m_CommSet.m_Baud,pChannel[i].m_PortSet.m_CommSet.m_DataBit,pChannel[i].m_PortSet.m_CommSet.m_StopBit,pChannel[i].m_PortSet.m_CommSet.m_Parity,MakeChannelComCDkey(pChannel[i].m_PortSet.m_CommSet.m_CDCheck));
		FileBuf.append("\t""Setting=").append(buffer).append("\n");
		sprintf(buffer, "%d", pChannel[i].m_ReadTimeoutMS);
		FileBuf.append("\t""ReadTimeoutMS=").append(buffer).append("\n");
		sprintf(buffer, "%d", pChannel[i].m_WriteTimeoutMS);
		FileBuf.append("\t""WriteTimeoutMS=").append(buffer).append("\n");
		sprintf(buffer, "%d", pChannel[i].m_SubReadTimeoutMS);
		FileBuf.append("\t""SubReadTimeoutMS=").append(buffer).append("\n");
		sprintf(buffer, "%d", pChannel[i].m_CharReadTimeoutMS);
		FileBuf.append("\t""CharReadTimeoutMS=").append(buffer).append("\n");
		sprintf(buffer, "%d", pChannel[i].m_ConnectTimeoutS);
		FileBuf.append("\t""ConnectTimeoutS=").append(buffer).append("\n");
		sprintf(buffer, "%d", pChannel[i].m_ConnectInterS);
		FileBuf.append("\t""ConnectInterS=").append(buffer).append("\n");
		sprintf(buffer, "%d", pChannel[i].m_IdleTimeoutS);
		FileBuf.append("\t""IdleTimeoutS=").append(buffer).append("\n");
		sprintf(buffer, "%d", pChannel[i].m_PackageMaxLen);
		FileBuf.append("\t""PackageMaxLen=").append(buffer).append("\n");
		FileBuf.append("}\n\n");
	}

	C_CFGFILE::GetUserSetFile(buffer, pFileName);
	ofstream outfile(buffer, ios::out);
	if (!outfile)
		logwt<<buffer<<" saves failed!!!"<<endl;
	else
	{
		outfile<<FileBuf;
		logit<<buffer<<" saves OK!!!"<<endl;
	}
	return 0;
}

//
//httx.ini
//
cfg_opt_t OPT_HTTX[] =
{
	CFG_BOOL("Enable", cfg_false,CFGF_NOCASE),
	CFG_BOOL("ConversationEnable", cfg_true,CFGF_NOCASE),
	CFG_BOOL("ReportEnable", cfg_false,CFGF_NOCASE),
	CFG_BOOL("Class1ReportConfirm", cfg_false,CFGF_NOCASE),
	CFG_BOOL("Class2ReportConfirm", cfg_false,CFGF_NOCASE),
	CFG_BOOL("Class3ReportConfirm", cfg_false,CFGF_NOCASE),
	CFG_INT ("RTS_MS", 20, CFGF_NOCASE),
	CFG_INT ("TpDelayM", 6, CFGF_NOCASE),
	CFG_INT ("ResendTimes", 3, CFGF_NOCASE),
	CFG_INT ("WaitResponseTimeoutS", 6, CFGF_NOCASE),
	CFG_INT ("HeartBeatPeriodM", 5, CFGF_NOCASE),
	CFG_INT ("MonTrafficLimit", 0, CFGF_NOCASE),
	CFG_INT ("MsgConfirmCode", 0, CFGF_NOCASE),
	CFG_INT ("MsgConfirmParam", 0, CFGF_NOCASE),
	CFG_STR_LIST("Channel", "{}", CFGF_NONE),
	CFG_END()
};
cfg_opt_t OPT_HTTXS[] =
{
	CFG_SEC("Httx", OPT_HTTX, CFGF_MULTI|CFGF_TITLE|CFGF_NOCASE),
	CFG_END()
};

int C_CFGHTTX::Load(S_HTTXINFO *pHttxInfo, int InfoSize, const char *pFileName)
{
	int err = 0;
	S_CHAR cfgFile[260];

	cfg_t* cfg = cfg_init(OPT_HTTXS, CFGF_NONE);
	C_CFGFILE::GetUserSetFile(cfgFile, pFileName);

	if ((err = cfg_parse(cfg,cfgFile)) != CFG_SUCCESS)
	{
		C_CFGFILE::GetDefSetFile(cfgFile, pFileName);
		err = cfg_parse(cfg,cfgFile);
	}

	if (err == CFG_SUCCESS)
	{
		for(unsigned int i = 0; i < cfg_size(cfg, "Httx"); i++)
		{
			cfg_t* cfgsec = cfg_getnsec(cfg, "Httx", i);
			int id = C_SERVER::GetServerID(cfg_title(cfgsec))-HDJS_SERID_HTTX1;
			if (id >= HTTX_ID_MAX)
				continue;
			pHttxInfo[id].m_pServInfo->m_Enabled = ((cfg_getbool(cfgsec, "Enable")==cfg_true)?true:false);
			pHttxInfo[id].m_ConversationEnable = ((cfg_getbool(cfgsec, "ConversationEnable")==cfg_true)?true:false);
			pHttxInfo[id].m_ReportEnable = ((cfg_getbool(cfgsec, "ReportEnable")==cfg_true)?true:false);
			pHttxInfo[id].m_Class1ReportConfirm = ((cfg_getbool(cfgsec, "Class1ReportConfirm")==cfg_true)?true:false);
			pHttxInfo[id].m_Class2ReportConfirm = ((cfg_getbool(cfgsec, "Class2ReportConfirm")==cfg_true)?true:false);
			pHttxInfo[id].m_Class3ReportConfirm = ((cfg_getbool(cfgsec, "Class3ReportConfirm")==cfg_true)?true:false);
			pHttxInfo[id].m_RTS_MS = (S_WORD)cfg_getint(cfgsec, "RTS_MS");
			pHttxInfo[id].m_TpDelayM = (S_BYTE)cfg_getint(cfgsec, "TpDelayM");
			pHttxInfo[id].m_ResendTimes = (S_BYTE)cfg_getint(cfgsec, "ResendTimes");
			pHttxInfo[id].m_WaitResponseTimeoutS = (S_WORD)cfg_getint(cfgsec, "WaitResponseTimeoutS");
			pHttxInfo[id].m_HeartBeatPeriodM = (S_BYTE)cfg_getint(cfgsec, "HeartBeatPeriodM");
			pHttxInfo[id].m_MonTrafficLimit = cfg_getint(cfgsec, "MonTrafficLimit");
			pHttxInfo[id].m_MsgConfirmCode = (S_BYTE)cfg_getint(cfgsec, "MsgConfirmCode");
			pHttxInfo[id].m_MsgConfirmParam = (S_WORD)cfg_getint(cfgsec, "MsgConfirmParam");
			for (int NO = 0; NO < (int)cfg_size(cfgsec, "Channel") && NO<HTTX_CHANNEL_MAX; ++NO)
			{
				memset(pHttxInfo[id].m_ChannelName[NO], 0x00, sizeof(pHttxInfo[id].m_ChannelName[NO]));
				strncpy(pHttxInfo[id].m_ChannelName[NO], cfg_getnstr(cfgsec, "Channel", NO), sizeof(pHttxInfo[id].m_ChannelName[NO])-1);
			}
		}
		logit<<cfgFile<<" loads OK!"<<endl;
		err = 0;
	}
	else
	{
		logwt<<cfgFile<<" loads failed!"<<endl;
		err = -1;
	}
	cfg_free(cfg);

	return err;
}

int C_CFGHTTX::Save(S_HTTXINFO *pHttxInfo, int InfoSize, const char *pFileName)
{
	char buffer[260];
	string FileBuf;
	FileBuf.reserve(2400);

	FileBuf.append("#file ").append(pFileName).append("\n""#path /mnt/dyjc/set/default /mnt/dyjc/set/user\n""#function 集中器后台通讯进程配置\n""#see upchannl.ini\n\n");
	for (int i = 0; i < InfoSize; i++)
	{
		FileBuf.append("Httx ").append(C_SERVER::GetServerName(i+HDJS_SERID_HTTX1)).append("\n""{\n");
		FileBuf.append("\t""Enable=").append(C_CFGFILE::GetBoolKey(pHttxInfo[i].m_pServInfo->m_Enabled)).append("\n");
		FileBuf.append("\t""ConversationEnable=").append(C_CFGFILE::GetBoolKey(pHttxInfo[i].m_ConversationEnable)).append("\n");
		FileBuf.append("\t""ReportEnable=").append(C_CFGFILE::GetBoolKey(pHttxInfo[i].m_ReportEnable)).append("\n");
		FileBuf.append("\t""Class1ReportConfirm=").append(C_CFGFILE::GetBoolKey(pHttxInfo[i].m_Class1ReportConfirm)).append("\n");
		FileBuf.append("\t""Class2ReportConfirm=").append(C_CFGFILE::GetBoolKey(pHttxInfo[i].m_Class2ReportConfirm)).append("\n");
		FileBuf.append("\t""Class3ReportConfirm=").append(C_CFGFILE::GetBoolKey(pHttxInfo[i].m_Class3ReportConfirm)).append("\n");
		sprintf(buffer, "%d", pHttxInfo[i].m_RTS_MS);
		FileBuf.append("\t""RTS_MS=").append(buffer).append(" #终端数传机延时时间\n");
		sprintf(buffer, "%d", pHttxInfo[i].m_TpDelayM);
		FileBuf.append("\t""TpDelayM=").append(buffer).append(" #终端作为启动站允许发送传输延时时间\n");
		sprintf(buffer, "%d", pHttxInfo[i].m_ResendTimes);
		FileBuf.append("\t""ResendTimes=").append(buffer).append(" #重发次数\n");
		sprintf(buffer, "%d", pHttxInfo[i].m_WaitResponseTimeoutS);
		FileBuf.append("\t""WaitResponseTimeoutS=").append(buffer).append(" #终端等待从动站响应的超时时间\n");
		sprintf(buffer, "%d", pHttxInfo[i].m_HeartBeatPeriodM);
		FileBuf.append("\t""HeartBeatPeriodM=").append(buffer).append(" #0表示不要心跳\n");
		sprintf(buffer, "%d", pHttxInfo[i].m_MonTrafficLimit);
		FileBuf.append("\t""MonTrafficLimit=").append(buffer).append(" #0表示不须要流量控制\n");
		sprintf(buffer, "%d", pHttxInfo[i].m_MsgConfirmCode);
		FileBuf.append("\t""MsgConfirmCode=").append(buffer).append(" #0表示不进行消息认证 1-255消息认证方案号\n");
		sprintf(buffer, "%d", pHttxInfo[i].m_MsgConfirmParam);
		FileBuf.append("\t""MsgConfirmParam=").append(buffer).append(" #消息认证参数\n");
		FileBuf.append("\t""Channel={");
		for (int n = 0; n < HTTX_CHANNEL_MAX; n++)//then put left
		{
			if (pHttxInfo[i].m_ChannelName[n][0])
				FileBuf.append(pHttxInfo[i].m_ChannelName[n]).append(",");
		}
		if (*(FileBuf.end()-1) == ',')
			FileBuf.erase(FileBuf.end()-1);
		FileBuf.append("} #主通道名(必需),备用通道名(httx1必需,其它可选),级联通道(可选)\n");
		FileBuf.append("}\n\n");
	}

	C_CFGFILE::GetUserSetFile(buffer, pFileName);
	ofstream outfile(buffer, ios::out);
	if (!outfile)
		logwt<<buffer<<" saves failed!!!"<<endl;
	else
	{
		outfile<<FileBuf;
		logit<<buffer<<" saves OK!!!"<<endl;
	}
	return 0;
}

/** @brief PPP配置结构体 */
cfg_opt_t OPT_PPP_DIAL_SETTINGS[] =
{
	CFG_STR("Apn", "APN",CFGF_NOCASE),
	CFG_STR("TeleNo", "123456",CFGF_NOCASE),
	CFG_STR("User", "",CFGF_NOCASE),
	CFG_STR("Pwd", "",CFGF_NOCASE),
	CFG_STR("ModemInitCmd","AT+CGDCONT=1,\"IP\",\"CMNET\"",CFGF_NOCASE),
	CFG_END()
};

cfg_opt_t OPT_PPP_SETTINGS[] =
{
	CFG_BOOL("Enable", cfg_true,CFGF_NOCASE),
	CFG_INT("OnlineMode", 1, CFGF_NOCASE),
	CFG_INT("RedialIntervalS", 6, CFGF_NOCASE),
	CFG_INT("RedialTimes", 3, CFGF_NOCASE),
	CFG_INT("IdleTimeoutM", 6, CFGF_NOCASE),
	CFG_STR_LIST("OnlineSpace","{}", CFGF_NOCASE),
	CFG_INT("CheckInterM",720, CFGF_NOCASE),
	CFG_INT("CheckFailTimes",2, CFGF_NOCASE),
	CFG_STR("Channel","ModemCom", CFGF_NOCASE),
	CFG_STR("MasterPhone","", CFGF_NOCASE),
	CFG_STR("SmsCenterNo","", CFGF_NOCASE),

	CFG_SEC("Gprs", OPT_PPP_DIAL_SETTINGS, CFGF_NOCASE),
	CFG_END()
};

/** @fn C_CFGPPP::Load
 *  @brief 加载PPP拨号参数配置
 *  @param[out] pPppInfo:配置结构体
 *  @param[in] pFileName:文件名
 *  @return 0:successful, -1:failed
 */
int C_CFGPPP::Load(S_PPPINFO &PppInfo, const char *pFileName)
{
	int err = 0;
	S_CHAR cfgFile[260];

	cfg_t* cfg = cfg_init(OPT_PPP_SETTINGS, CFGF_NONE);
	C_CFGFILE::GetUserSetFile(cfgFile, pFileName);

	if ((err = cfg_parse(cfg,cfgFile)) != CFG_SUCCESS)
	{
		C_CFGFILE::GetDefSetFile(cfgFile, pFileName);
		err = cfg_parse(cfg,cfgFile);
	}

	if (err == CFG_SUCCESS)
	{
		PppInfo.m_pServInfo->m_Enabled = cfg_getbool(cfg, "Enable")==cfg_true?true:false;
		PppInfo.m_OnlineMode = (INT8U)cfg_getint(cfg, "OnlineMode");
		PppInfo.m_RedialIntervalS = (INT16U)cfg_getint(cfg, "RedialIntervalS");
		PppInfo.m_RedialTimes  = (INT16U)cfg_getint(cfg, "RedialTimes");
		PppInfo.m_IdleTimeoutM = (INT16U)cfg_getint(cfg, "IdleTimeoutM");

		PppInfo.m_OnlineSpace.ClearBit();
		for (unsigned int i = 0; i < cfg_size(cfg, "OnlineSpace"); i++)
		{
			strings strs;
			C_CFGFILE::ParseString(strs, cfg_getnstr(cfg, "OnlineSpace", i), '-');
			if (strs.empty())
				continue;
			int from = atoi(strs[0].c_str()), to = from+1;
			if (strs.size() >= 2)
				to = atoi(strs[1].c_str());
			for (; from < to; from++)
			{
				if (from >= 0 && from <= 23)
					PppInfo.m_OnlineSpace.SetBit(from);
			}
		}

		PppInfo.m_PppSet.m_CheckInterM=(INT16U)cfg_getint(cfg,"CheckInterM");
		PppInfo.m_PppSet.m_CheckFailTimes=(INT16U)cfg_getint(cfg,"CheckFailTimes");

		memset(PppInfo.m_MasterPhone, 0x00, sizeof(PppInfo.m_MasterPhone));
		strncpy(PppInfo.m_MasterPhone, cfg_getstr(cfg,"MasterPhone"), sizeof(PppInfo.m_MasterPhone)-1);
		memset(PppInfo.m_SmsCenterNo,0x00,sizeof(PppInfo.m_SmsCenterNo));
		strncpy(PppInfo.m_SmsCenterNo, cfg_getstr(cfg,"SmsCenterNo"), sizeof(PppInfo.m_SmsCenterNo)-1);
		memset(PppInfo.m_PppSet.m_ChannlName, 0x00, sizeof(PppInfo.m_PppSet.m_ChannlName));
		strncpy(PppInfo.m_PppSet.m_ChannlName, cfg_getstr(cfg,"Channel"), sizeof(PppInfo.m_PppSet.m_ChannlName)-1);

		cfg_t* cfgDial = cfg_getsec(cfg, "Gprs");
		C_DIALSET* pDialSetting = &PppInfo.m_PppSet.m_GprsSet;
		memset(pDialSetting->m_Apn, 0x00,sizeof(pDialSetting->m_Apn));
		strncpy(pDialSetting->m_Apn,cfg_getstr(cfgDial,"Apn"),sizeof(pDialSetting->m_Apn)-1);
		memset(pDialSetting->m_TeleNo,0x00,sizeof(pDialSetting->m_TeleNo));
		strncpy(pDialSetting->m_TeleNo,cfg_getstr(cfgDial,"TeleNo"),sizeof(pDialSetting->m_TeleNo)-1);
		memset(pDialSetting->m_User,0x00,sizeof(pDialSetting->m_User));
		strncpy(pDialSetting->m_User,cfg_getstr(cfgDial,"User"),sizeof(pDialSetting->m_User)-1);
		memset(pDialSetting->m_Pwd,0x00,sizeof(pDialSetting->m_Pwd));
		strncpy(pDialSetting->m_Pwd,cfg_getstr(cfgDial,"Pwd"),sizeof(pDialSetting->m_Pwd)-1);
		memset(pDialSetting->m_ModemInitCmd,0x00,sizeof(pDialSetting->m_ModemInitCmd));
		strncpy(pDialSetting->m_ModemInitCmd,cfg_getstr(cfgDial,"ModemInitCmd"),sizeof(pDialSetting->m_ModemInitCmd)-1);

		logit<<cfgFile<<" loads OK!"<<endl;
		err = 0;
	}
	else
	{
		logwt<<cfgFile<<" loads failed!"<<endl;
		err = -1;
	}

	cfg_free(cfg);
	return err;
}

/** @fn C_CFGPPP::Save
 *  @brief 保存PPP拨号参数配置
 *  @param[in] pPppInfo:配置结构体
 *  @param[in] pFileName:文件名
 *  @return 0:successful, -1:failed
 */
int C_CFGPPP::Save(S_PPPINFO &PppInfo, const char *pFileName)
{
	char strBuf[260];
	string FileBuf;
	FileBuf.reserve(3000);

	FileBuf.append("#file ").append(pFileName).append("\n""#path /mnt/dyjc/set/default /mnt/dyjc/set/user\n""#function PPP拨号配置\n""#see upchannl.ini\n");

	FileBuf.append("\nEnable=").append(C_CFGFILE::GetBoolKey(PppInfo.m_pServInfo->m_Enabled));
	sprintf(strBuf, "%d", PppInfo.m_OnlineMode);
	FileBuf.append("\nOnlineMode=").append(strBuf).append(" #1永久在线模式、2被动激活模式、3时段在线模式");
	sprintf(strBuf,"%d",PppInfo.m_RedialIntervalS);
	FileBuf.append("\nRedialIntervalS=").append(strBuf).append(" #永久在线、时段在线模式重拨间隔");
	sprintf(strBuf, "%d", PppInfo.m_RedialTimes);
	FileBuf.append("\nRedialTimes=").append(strBuf).append(" #被动激活模式重拨次数");
	sprintf(strBuf,"%d",PppInfo.m_IdleTimeoutM);
	FileBuf.append("\nIdleTimeoutM=").append(strBuf).append(" #被动激活模式连续无通信自动断线时间");

	FileBuf.append("\nOnlineSpace={");
	for (int i = 0; i < 24; i++)
	{
		if (PppInfo.m_OnlineSpace.TestBit(i))
		{
			sprintf(strBuf, "%d", i);
			FileBuf.append(strBuf).append(",");
		}
	}
	if (*(FileBuf.end() -1) == ',')
		FileBuf.erase(FileBuf.end() -1);
	FileBuf.append("}");

	sprintf(strBuf,"%d",PppInfo.m_PppSet.m_CheckInterM);
	FileBuf.append("\nCheckInterM=").append(strBuf);
	sprintf(strBuf,"%d",PppInfo.m_PppSet.m_CheckFailTimes);
	FileBuf.append("\nCheckFailTimes=").append(strBuf);
	
	FileBuf.append("\nMasterPhone=\"").append(PppInfo.m_MasterPhone).append("\"");
	FileBuf.append("\nSmsCenterNo=\"").append(PppInfo.m_SmsCenterNo).append("\"");
	FileBuf.append("\nChannel=").append(PppInfo.m_PppSet.m_ChannlName);

	C_DIALSET* pDialSetting = &PppInfo.m_PppSet.m_GprsSet;
	FileBuf.append("\n\n" "Gprs").append("\n{\n\t");
	FileBuf.append("Apn=\"").append(pDialSetting->m_Apn).append("\"\n\t");
	FileBuf.append("TeleNo=\"").append(pDialSetting->m_TeleNo).append("\"\n\t");
	FileBuf.append("User=\"").append(pDialSetting->m_User).append("\"\n\t");
	FileBuf.append("Pwd=\"").append(pDialSetting->m_Pwd).append("\"\n\t");
	sprintf(strBuf,"%s,\\\"%s\\\"","AT+CGDCONT=1,\\\"IP\\\"",pDialSetting->m_Apn);
	FileBuf.append("ModemInitCmd=\"").append(strBuf).append("\"\n}\n\n");

	C_CFGFILE::GetUserSetFile(strBuf, pFileName);
	ofstream outfile(strBuf, ios::out);
	if (!outfile)
		logwt<<strBuf<<" saves failed!!!"<<endl;
	else
	{
		outfile<<FileBuf;
		logit<<strBuf<<" saves OK!!!"<<endl;
	}
	return 0;
}

//
//forward.ini
//
/** @brief 级联控制参数结构体 */
cfg_opt_t OPT_FORWARD_SETTINGS[] =
{
	CFG_BOOL("Enable", cfg_false, CFGF_NOCASE),
	CFG_STR("Forwarder","master", CFGF_NOCASE),
	CFG_STR_LIST("ForwardedAddr","{}", CFGF_NOCASE),
	CFG_INT("ResendTimes", 3, CFGF_NOCASE),
	CFG_INT("CheckPeriodM", 5, CFGF_NOCASE),
	CFG_STR("Channel","forward", CFGF_NOCASE),
	CFG_END()
};

cfg_opt_t OPT_FORWARDS_SETTINGS[] =
{
	CFG_SEC("forward", OPT_FORWARD_SETTINGS, CFGF_MULTI |  CFGF_NOCASE),
	CFG_END()
};

/** @fn C_CFGFORWARD::Load
 *  @brief 加载级联控制参数配置
 *  @param[out] pForwardInfo:配置结构体
 *  @param[in] pFileName:文件名
 *  @return 0:successful, -1:failed
 */
int C_CFGFORWARD::Load(S_FORWARD &ForwardInfo, const char *pFileName)
{
	int err = 0;
	S_CHAR cfgFile[260];

	cfg_t* cfg = cfg_init(OPT_FORWARDS_SETTINGS, CFGF_NONE);
	C_CFGFILE::GetUserSetFile(cfgFile, pFileName);

	if ((err = cfg_parse(cfg,cfgFile)) != CFG_SUCCESS)
	{
		C_CFGFILE::GetDefSetFile(cfgFile, pFileName);
		err = cfg_parse(cfg,cfgFile);
	}

	if (err == CFG_SUCCESS)
	{
		for(unsigned int i = 0; i < cfg_size(cfg, "forward"); i++)
		{
			cfg_t* cfgForward = cfg_getnsec(cfg, "forward", i);
			if (cfg_getbool(cfgForward, "Enable") == cfg_false)
				continue;

			ForwardInfo.m_MasterForwarder = GetMasterForward(cfg_getstr(cfgForward, "Forwarder"));
			for (int i = 0; i < FORWARD_ADDR_MAX; i++)
				ForwardInfo.SetForwardedAddr(i, cfg_getnstr(cfgForward, "ForwardedAddr", i));

			ForwardInfo.m_ResendTimes  = (INT8U)cfg_getint(cfgForward, "ResendTimes");
			ForwardInfo.m_CheckPeriodM = (INT8U)cfg_getint(cfgForward, "CheckPeriodM");
			strncpy(ForwardInfo.m_ChannlName, cfg_getstr(cfgForward, "Channel"), sizeof(ForwardInfo.m_ChannlName)-1);
		}
		logit<<cfgFile<<" loads OK!"<<endl;
		err = 0;
	}
	else
	{
		logwt<<cfgFile<<" loads failed!"<<endl;
		err = -1;
	}

	cfg_free(cfg);
	return err;
}

/** @fn C_CFGFORWARD::Save
 *  @brief 保存级联控制参数配置
 *  @param[out] pForwardInfo:配置结构体
 *  @param[in] pFileName:文件名
 *  @return 0:successful, -1:failed
 */
int C_CFGFORWARD::Save(S_FORWARD &ForwardInfo, const char *pFileName)
{
	char strBuf[260];
	string FileBuf;
	FileBuf.reserve(3000);

	FileBuf.append("#file ").append(pFileName).append("\n""#path /mnt/dyjc/set/default /mnt/dyjc/set/user\n""#function 级联配置参数文件\n""#see upchannl.ini\n\n");

	FileBuf.append("forward\n{\n");
	FileBuf.append("\tEnable = true\n");
	FileBuf.append("\tForwarder = ").append(GetMasterForwardStr(ForwardInfo.m_MasterForwarder)).append(" #master主集中器，slave从集中器\n");
	FileBuf.append("\tForwardedAddr = {");
	for (int i = 0; i < FORWARD_ADDR_MAX; i++)
	{
		memset(strBuf, 0x00, sizeof(strBuf));
		FileBuf.append(ForwardInfo.GetForwardedAddrStr(i, strBuf));
		if (i == FORWARD_ADDR_MAX-1) FileBuf.append("} #主填从集中器地址，从填主集中器地址\n");
		else FileBuf.append(",");
	}
	sprintf(strBuf, "%d", ForwardInfo.m_ResendTimes);
	FileBuf.append("\tResendTimes = ").append(strBuf).append(" #主动站接收失败重发次数(0～3),如为0,表示不重发\n");

	sprintf (strBuf, "%d", ForwardInfo.m_CheckPeriodM);
	FileBuf.append("\tCheckPeriodM = ").append(strBuf).append(" #级联巡测周期(1~60)\n");
	FileBuf.append("\tChannel = ").append(ForwardInfo.m_ChannlName).append(" #级联通道(必须)\n");
	FileBuf.append("}\n\n");

	C_CFGFILE::GetUserSetFile(strBuf, pFileName);
	ofstream outfile(strBuf, ios::out);
	if (!outfile)
		logwt<<strBuf<<" saves failed!!!"<<endl;
	else
	{
		outfile<<FileBuf;
		logit<<strBuf<<" saves OK!!!"<<endl;
	}
	return 0;
}

/** @fn C_CFGFORWARD::GetMasterForward
 *  @brief 根据名称识别主从集中器
 *  @param[in] pStrMask:名称 "master" "slave"
 *  @return true:主集中器, false:从集中器
 */
bool C_CFGFORWARD::GetMasterForward(const char *pStrMask)
{
	if (strcasecmp(pStrMask, "master") == 0)
		return true;
	else if (strcasecmp(pStrMask, "slave") == 0)
		return false;

	return true;
}

/** @fn C_FORWARDCFG::GetMasterForwardStr
 *  @brief 获取主从集中器名称
 *  @param[in] Master:主从
 *  @return "master":主集中器, "false":从集中器
 */
char *C_CFGFORWARD::GetMasterForwardStr(bool Master)
{
	if (Master == true)
        return "master";
	return "slave";
}

//
//syscfg.ini
//
/** @brief 系统配置参数结构体 */
cfg_opt_t OPT_SYSCFG_SETTINGS[] =
{	//参数名 ,参数默认值  ,标志	configuration file is case insensitive
	CFG_STR("RTUA", "00000000", CFGF_NOCASE),
	CFG_STR("SysUser", "root", CFGF_NOCASE),
	CFG_STR("SysPassword", "123456", CFGF_NOCASE),
	CFG_STR("ReleaseDate", "2009-09-09", CFGF_NOCASE),
	CFG_STR("HWVersion", "v0.0.0", CFGF_NOCASE),
	CFG_STR("SWVersion", "v0.0.0", CFGF_NOCASE),
	CFG_BOOL("GatherMeterEnable", cfg_true, CFGF_NOCASE),
	CFG_INT("VipPointPerDay", 24, CFGF_NOCASE),
	CFG_INT("HeartBeatFailedTimes", 3, CFGF_NOCASE),
	CFG_END()
};

/** @fn C_CFGSYSTEM::Load
 *  @brief 加载系统配置参数配置
 *  @param[out] SysConfig:配置结构体
 *  @param[in] pFileName:文件名
 *  @return 0:successful, -1:failed
 */
int C_CFGSYSTEM::Load(S_SYSCFG &SysConfig, const char *pFileName)
{
	int err = 0;
	S_CHAR cfgFile[260];
	C_CFGFILE::GetSysFile(cfgFile, pFileName);

	cfg_t* cfg = cfg_init(OPT_SYSCFG_SETTINGS, CFGF_NONE);
	if (cfg_parse(cfg,cfgFile) == CFG_SUCCESS)
	{
		SysConfig.SetRTUA(cfg_getstr(cfg, "RTUA"));

		strncpy(SysConfig.m_SysUser, cfg_getstr(cfg, "SysUser"), sizeof(SysConfig.m_SysUser)-4);
		strncpy(SysConfig.m_SysPassword, cfg_getstr(cfg, "SysPassword"), sizeof(SysConfig.m_SysPassword)-4);

		strncpy(SysConfig.m_ReleaseDate, cfg_getstr(cfg, "ReleaseDate"), sizeof(SysConfig.m_ReleaseDate)-4);
		strncpy(SysConfig.m_HWVersion, cfg_getstr(cfg, "HWVersion"), sizeof(SysConfig.m_HWVersion)-4);
		strncpy(SysConfig.m_SWVersion, cfg_getstr(cfg, "SWVersion"), sizeof(SysConfig.m_SWVersion)-4);

		SysConfig.m_GatherMeterEnable = cfg_getbool(cfg, "GatherMeterEnable")==cfg_true? true:false;
		SysConfig.m_VipPointPerDay = (INT16U)cfg_getint(cfg, "VipPointPerDay");
		SysConfig.m_HeartBeatFailedTimes = (INT16U)cfg_getint(cfg, "HeartBeatFailedTimes");
	
		logit<<cfgFile<<" loads OK!"<<endl;
	}
	else
	{
		logwt<<cfgFile<<" loads failed!"<<endl;
		err = -1;
	}
	cfg_free(cfg);

	return err;
}

/** @fn C_CFGSYSTEM::Save
 *  @brief 保存系统配置参数配置
 *  @param[in] SysConfig:配置结构体
 *  @param[in] pFileName:文件名
 *  @return 0:successful, -1:failed
 */
int C_CFGSYSTEM::Save(S_SYSCFG &SysConfig, const char *pFileName)
{
	char strBuf[260];
	string FileBuf;
	FileBuf.reserve(2400);
	FileBuf.append("#file ").append(pFileName).append("\n""#path /mnt/system\n""#function 系统信息配置\n\n");

	FileBuf.append("#终端地址\n");
	memset(strBuf, 0x00, sizeof(strBuf));
	FileBuf.append("RTUA=").append(SysConfig.GetRTUAstr(strBuf)).append(" #格式 BBBBHHHH:B=BCD,H=HEX\n\n");
	FileBuf.append("#登陆信息\n");
	FileBuf.append("SysUser=\"").append(SysConfig.m_SysUser).append("\"\n");
	FileBuf.append("SysPassword=\"").append(SysConfig.m_SysPassword).append("\"\n\n");
	
	FileBuf.append("#版本信息\n");
	FileBuf.append("ReleaseDate=\"").append(HAI_RELEASE_DATE).append("\"\n");
	FileBuf.append("SWVersion=\"").append(HAI_SOFT_VERSION).append("\"\n");
	FileBuf.append("HWVersion=\"").append(HAI_HARD_VERSION).append("\"\n\n");

	FileBuf.append("#配置信息\n");
	FileBuf.append("GatherMeterEnable=").append(SysConfig.m_GatherMeterEnable==cfg_true?"true":"false").append("\n");
	sprintf(strBuf, "%d", SysConfig.m_VipPointPerDay);
	FileBuf.append("VipPointPerDay=").append(strBuf).append(" #须为24的整数倍\n");
	sprintf(strBuf, "%d", SysConfig.m_HeartBeatFailedTimes);
	FileBuf.append("HeartBeatFailedTimes=").append(strBuf);

	C_CFGFILE::GetSysFile(strBuf, pFileName);
	ofstream outfile(strBuf, ios::out);
	if (!outfile)
		logwt<<strBuf<<" saves failed!!!"<<endl;
	else
	{
		outfile<<FileBuf;
		logit<<strBuf<<" saves OK!!!"<<endl;
	}
	return 0;
}





//
//public cfg function
//
int C_CFGFILE::GetSysFile(char *pFileBuf, const char *pFileName)
{
	sprintf(pFileBuf, "%s%s", CFG_FILE_SYS_PATH, pFileName);
	return 0;
}

int C_CFGFILE::GetDefSetFile(char *pFileBuf, const char *pFileName)
{
	sprintf(pFileBuf, "%s%s", CFG_DEF_SET_PATH, pFileName);
	return 0;
}

int C_CFGFILE::GetUserSetFile(char *pFileBuf, const char *pFileName)
{
	sprintf(pFileBuf, "%s%s", CFG_USR_SET_PATH, pFileName);
	return 0;
}

int C_CFGFILE::MakeValue(const char *pkey, const char **pkeys, int size)
{
	for(int i = 0; i < size; ++i)
	{
		if (strcasecmp(pkey, pkeys[i]) == 0)
			return i;
	}
	return 0;
}

const char *C_CFGFILE::MakeKey(int val, const char **pkeys, int size)
{
	if (val < size)
		return pkeys[val];
	return "invalid";
}

const char *C_CFGFILE::GetBoolKey(S_BOOL val)
{
	if (val == _TRUE)
		return "true";
	return "false";
}

int C_CFGFILE::ParseString(strings &strs, const string &str, char ch)
{
	string::const_iterator it, first = str.begin();
	for (it = str.begin(); it < str.end(); ++it)
	{
		if (*it == ch)
		{
			string tmp(first, it);
			strs.push_back(tmp);
			first = it+1;
		}
	}
	if (first < it)//process lastest
	{
		string tmp(first, it);
		strs.push_back(tmp);
	}
	return 0;
}


