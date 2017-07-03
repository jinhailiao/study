//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			dbcj.h
// Description:		meter gather
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-16  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SERVERDBCJ_H__
#define __SERVERDBCJ_H__

#include "server.h"

class C_DBCJ: public C_SERVER
{
public:
	C_DBCJ(void);
	~C_DBCJ();

public:
	int BeforeService(void);
	int Servicing(void);
	int AfterService(void);

};


#endif //__SERVERDBCJ_H__

