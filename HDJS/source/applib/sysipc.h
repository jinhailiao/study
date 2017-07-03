//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			sysipc.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-13  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SYSIPC_H__
#define __SYSIPC_H__
#include "message.h"

class C_SYSIPC
{
public:
	enum{MTYPE_BASE_VAL=100, SYSIPC_TIMEOUT_S=60};
	enum{SYSIPC_TYPE_MSG_SEND = 10};

	static int Send2Server(int SerID, S_BYTE MsgT, const string &msg);
	static int Recv4Server(S_BYTE MsgT, string &msg);
	static int Wait4Server(S_BYTE MsgT, string &msg);

	static int Query4Server(int &SerID, S_BYTE &MsgT, string &msg);
	static int Reply2Server(int SerID, S_BYTE MsgT, const string &msg);

private:
	static C_MESSAGE m_MsgObj;
	static S_BYTE ReqSeq;
	static S_BYTE ResSeq;
};

#endif //__SYSIPC_H__
