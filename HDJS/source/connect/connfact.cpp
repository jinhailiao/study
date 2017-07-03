//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			connfact.cpp
// Description:		connection factory
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-23  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "connfact.h"
#include "conn_net.h"
#include "conn_com.h"


C_CONNECT *C_CONNFACT::CreateConnection(S_CHANNEL &Channel)
{
	switch (Channel.m_Type)
	{
	case CHANNEL_TYPE_NET:
		return new C_NETCONN(Channel);
	case CHANNEL_TYPE_COMM:
		return new C_COMCONN(Channel);
	default:
		break;
	}
	return NULL;
}

void C_CONNFACT::DestroyConnection(C_CONNECT *pConnect)
{
	delete pConnect;
}
