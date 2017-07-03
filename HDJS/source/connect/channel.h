//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			channel.h
// Description:		communication channel setting
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-23  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "haidef.h"

enum
{
	NET_CONNECT_TCP,
	NET_CONNECT_UDP,
	NET_CONNECT_TCP_SERVER,
	NET_CONNECT_TCP_CLIENT,
	NET_CONNECT_UDP_SERVER,
	NET_CONNECT_UDP_CLIENT,

	NET_CONNECT_TYPE_MAX
};

enum
{
	NET_CONNECT_PPP_NO,
	NET_CONNECT_PPP_YES
};

struct S_NETSET
{
	S_BYTE m_IP[4]; ///<192.168.1.1 IP[0]=192
	S_WORD m_PortNO;
	S_BYTE m_Type;///<NET_CONNECT_
	S_BYTE m_NeedPPP;///<NET_CONNECT_NEEDPPP_

public:
	S_CHAR *GetStrIP(S_CHAR *pStrIP)
	{
		sprintf(pStrIP,"%d.%d.%d.%d",m_IP[0],m_IP[1],m_IP[2],m_IP[3]);
		return pStrIP;
	}
};

enum
{
	COMM_DATA_BIT5 = 5,
	COMM_DATA_BIT6,
	COMM_DATA_BIT7,
	COMM_DATA_BIT8
};

enum
{
	COMM_STOP_BIT1 = 1,
	COMM_STOP_BIT2, 
	COMM_STOP_BIT3
};

enum
{
	COMM_PARITY_NO = 0,
	COMM_PARITY_ODD,
	COMM_PARITY_EVEN
};

enum
{
	COMM_CDCHECK_NO = 0,
	COMM_CDCHECK_YES
};

struct S_COMMSET
{
	S_DWORD  m_PortNO:8;
	S_DWORD  m_DataBit:8;///<5,6,7,8
	S_DWORD  m_StopBit:8;///<1->1λ,2->2λ,3->1.5
	S_DWORD  m_Parity:4;///<0->no parity;1->odd parity;2->even parity;
	S_DWORD  m_CDCheck:4;
	S_DWORD m_Baud;///<115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 600,300
};

union S_PORTSET
{
	S_NETSET  m_NetSet;
	S_COMMSET m_CommSet;
};

enum
{
	CHANNEL_TYPE_UNKNOW,
	CHANNEL_TYPE_NET,
	CHANNEL_TYPE_COMM
};

#define CHANNEL_NAME_MAX	12
struct S_CHANNEL
{
	S_BOOL m_Enable;
	S_BYTE m_Name[CHANNEL_NAME_MAX];
	S_BYTE m_Type; ///<CHANNEL_TYPE_
	S_BYTE m_ConnectTimeoutS;
	S_WORD m_ConnectInterS;
	S_WORD m_IdleTimeoutS;
	S_WORD m_ReadTimeoutMS;
	S_WORD m_WriteTimeoutMS;
	S_WORD m_SubReadTimeoutMS;
	S_WORD m_CharReadTimeoutMS;
	S_WORD m_PackageMaxLen;
	S_PORTSET m_PortSet;

public:
	S_CHANNEL(void);
	S_CHANNEL(S_CHANNEL &r);
	~S_CHANNEL();

	S_CHANNEL & operator=(S_CHANNEL &r);

public:
	S_BOOL NeedPPP(void);
};


#endif //__CHANNEL_H__


