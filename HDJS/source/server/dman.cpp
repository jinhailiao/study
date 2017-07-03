//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			dman.cpp
// Description:		data manager
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-16  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "dman.h"
#include "htrace.h"
#include "context.h"
#include "haicfg.h"


C_DMAN::C_DMAN(void)
{
	m_pServInfo = &(CONTEXT.m_ServInfo[HDJS_SERID_DMAN]);
}

C_DMAN::~C_DMAN()
{
}

int C_DMAN::BeforeService(void)
{
	C_SERVER::SetCurrentServId(HDJS_SERID_DMAN);
	C_HTRACE::SetSystemTraceSetting(&CONTEXT.m_TraceSet[HDJS_SERID_DMAN]);
	return 0;
}

int C_DMAN::Servicing(void)
{
	while (1)
	{
		HeartBeat();
		loget<<"Dman server is running!"<<endl;
		SleepSecond(60);
	}
	return 0;
}

int C_DMAN::AfterService(void)
{
	return 0;
}

