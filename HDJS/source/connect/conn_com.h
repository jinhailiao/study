//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			conn_com.h
// Description:		serial connection
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-04-27  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __CONN_COM_H__
#define __CONN_COM_H__

#include "haidef.h"
#include "connect.h"

class C_COMCONN:public C_CONNECT
{
public:
	C_COMCONN(S_CHANNEL &Channel);
	~C_COMCONN();

	virtual S_BOOL ClearInBuffer(void);
	virtual S_BOOL ClearOutBuffer(void);

protected:
	S_BOOL OnConnect(void);
	S_BOOL OnDisconnect(void);
	S_BOOL ConnectSuccessful(void);
	int ReadData(S_BYTE *pData, int size);
	int WriteData(S_BYTE *pData, int size);

protected:
	int InitCom(void);
	S_BOOL SetCom(void);
	int GetBaudAttr(int Baud);

protected:
	int m_Comfd;
};


#endif //__CONN_COM_H__


