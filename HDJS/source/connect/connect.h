//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			connect.h
// Description:		connectino father
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-23  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __CONNECT_H__
#define __CONNECT_H__

#include "haidef.h"
#include "channel.h"

class C_CONNECT
{
public:
	C_CONNECT(S_CHANNEL &channel);
	virtual ~C_CONNECT();

	S_BOOL Connect(void);
	S_BOOL Disconnect(void);
	S_BOOL IsConnected(void);
	int SendData(const string &strData);
	int RecvData(string &strData);

	virtual S_BOOL ClearInBuffer(void){return true;}
	virtual S_BOOL ClearOutBuffer(void){return true;}

protected:
	virtual S_BOOL OnConnect(void) = 0;
	virtual S_BOOL OnDisconnect(void) = 0;
	virtual S_BOOL ConnectSuccessful(void) = 0;
	virtual int ReadData(S_BYTE *pData, int size) = 0;
	virtual int WriteData(S_BYTE *pData, int size) = 0;

	virtual int WriteDataDelay(int SendByte){return 0;}

protected:
	time_t m_LastConnectTime;
	S_CHANNEL m_Channel;
};

#endif //__CONNECT_H__


