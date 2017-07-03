//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			dman.h
// Description:		data manager
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-16  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SERVERDMAN_H__
#define __SERVERDMAN_H__

#include "server.h"

class C_DMAN: public C_SERVER
{
public:
	C_DMAN(void);
	~C_DMAN();

public:
	int BeforeService(void);
	int Servicing(void);
	int AfterService(void);

};


#endif //__SERVERDMAN_H__

