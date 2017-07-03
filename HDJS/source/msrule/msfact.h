//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			msfact.h
// Description:		afn object and fn object factory
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-07-20  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __MSFACTORY_H__
#define __MSFACTORY_H__

#include "haidef.h"
#include "mscmd.h"

//
//AFN factory
//
class C_AFNFACT
{
public:
	static C_AFNOBJ *GetAfnObj(S_BYTE AFN);
};


#endif //__MSFACTORY_H__


