//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			dbcj.cpp
// Description:		meter gather
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-16  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "dbcj.h"
#include "htrace.h"
#include "context.h"
#include "haicfg.h"
#include "sysipc.h"


C_DBCJ::C_DBCJ(void)
{
	m_pServInfo = &(CONTEXT.m_ServInfo[HDJS_SERID_DBCJ]);
}

C_DBCJ::~C_DBCJ()
{
}

int C_DBCJ::BeforeService(void)
{
	C_SERVER::SetCurrentServId(HDJS_SERID_DBCJ);
	C_HTRACE::SetSystemTraceSetting(&CONTEXT.m_TraceSet[HDJS_SERID_DBCJ]);
	return 0;
}

int C_DBCJ::Servicing(void)
{
	string reply("ok, I am dbcj.");
	logvt<<"C_DBCJ is running!"<<endl;
	while (1)
	{
		S_BYTE MsgT;
		int SerID;
		string msg;

		HeartBeat();
		SleepSecond(1);
		
		if (C_SYSIPC::Query4Server(SerID, MsgT, msg) >= 0)
			C_SYSIPC::Reply2Server(SerID, 2, reply);
		
		SleepSecond(1);
	}
	return 0;
}

int C_DBCJ::AfterService(void)
{
	return 0;
}

