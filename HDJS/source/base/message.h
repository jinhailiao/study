//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			message.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-01-13  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include "haidef.h"

class C_MESSAGE
{
public:
	enum{TEXT_BUF_MAX=2048};

	C_MESSAGE(char QFlag = 'M');
	~C_MESSAGE();

public:
	int SendMsg(int MsgT, string &msg);
	int RecvMsg(int MsgT, string &msg);
	int ClearMsg(int MsgT);

private:
	int m_MsgQID;
};

#endif //__MESSAGE_H__


