//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			servfact.cpp
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-14  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "haidef.h"
#include "servfact.h"
#include "daemon.h"
#include "httx.h"
#include "dbcj.h"
#include "ppp.h"
#include "dman.h"
#include "uiman.h"

C_SERVER* C_SERVFACT::GetServer(int ServID)
{
	static C_SERVER *pDaemonServ = NULL;
	static C_SERVER *pHttx1Serv = NULL;
	static C_SERVER *pHttx2Serv = NULL;
	static C_SERVER *pHttx3Serv = NULL;
	static C_SERVER *pDbcjServ  = NULL;
	static C_SERVER *pPPPServ   = NULL;
	static C_SERVER *pDmanServ  = NULL;
	static C_SERVER *pUImanServ = NULL;

	switch (ServID)
	{
	case HDJS_SERID_DAEMON:
		if (pDaemonServ == NULL)
			pDaemonServ = new C_DAEMON;
		return pDaemonServ;
	case HDJS_SERID_HTTX1:
		if (pHttx1Serv == NULL)
			pHttx1Serv = new C_HTTXI;
		return pHttx1Serv;
	case HDJS_SERID_HTTX2:
		if (pHttx2Serv == NULL)
			pHttx2Serv = new C_HTTXII;
		return pHttx2Serv;
	case HDJS_SERID_HTTX3:
		if (pHttx3Serv == NULL)
			pHttx3Serv = new C_HTTXIII;
		return pHttx3Serv;
	case HDJS_SERID_DBCJ:
		if (pDbcjServ == NULL)
			pDbcjServ = new C_DBCJ;
		return pDbcjServ;
	case HDJS_SERID_PPP:
		if (pPPPServ == NULL)
			pPPPServ = new C_PPP;
		return pPPPServ;
	case HDJS_SERID_DMAN:
		if (pDmanServ == NULL)
			pDmanServ = new C_DMAN;
		return pDmanServ;
	case HDJS_SERID_UIMAN:
		if (pUImanServ == NULL)
			pUImanServ = new C_UIMAN;
		return pUImanServ;
	default:
		break;
	}
	return NULL;
}

/*
C_SERVER* C_SERVFACT::GetServer(const char *pServName)
{
	return GetServer(C_SERVER::GetServerID(pServName));
}

C_SERVER* C_SERVFACT::GetCurServer(void)
{
	return GetServer(C_SERVER::GetCurrentServID());
}
*/

