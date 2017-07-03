//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			msfact.cpp
// Description:		afn object and fn object factory
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-07-20  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "msrule.h"
#include "msfact.h"
#include "msparam.h"

//
//AFN factory
//
C_AFNOBJ *C_AFNFACT::GetAfnObj(S_BYTE AFN)
{
	static C_RESETCMD ResetCmd;
	static C_CONF_CMD ConfirmCmd;
	static C_PARASETCMD ParaSetCmd;
	static C_PARAGETCMD ParaGetCmd;

	switch (AFN)
	{
	case AFN_CONFIRM_DENY:return &ConfirmCmd;
	case AFN_RESET: return &ResetCmd;
	case AFN_SET_PARAM: return &ParaSetCmd;
	case AFN_GET_PARAM: return &ParaGetCmd;
	}
	return NULL;
}

