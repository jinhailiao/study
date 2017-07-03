//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			serfact.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-14  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SERVERFACT_H__
#define __SERVERFACT_H__

#include "server.h"

class C_SERVFACT
{
public:
	static C_SERVER* GetServer(int ServID);
	static C_SERVER* GetServer(const char *pServName){return GetServer(C_SERVER::GetServerID(pServName));} 
	static C_SERVER* GetCurServer(void){return GetServer(C_SERVER::GetCurrentServID());}
};

#endif //__SERVERFACT_H__


