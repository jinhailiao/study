//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			connfact.h
// Description:		connection factory
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-23  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __CONNFACT_H__
#define __CONNFACT_H__

#include "haidef.h"
#include "connect.h"

class C_CONNFACT
{
public:
	static C_CONNECT *CreateConnection(S_CHANNEL &Channel);
	static void DestroyConnection(C_CONNECT *pConnect);
};

#endif //__CONNFACT_H__


