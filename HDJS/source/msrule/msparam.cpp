//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			msparam.cpp
// Description:		MS parameter config
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-07-20  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "msrule.h"
#include "msfact.h"
#include "msparam.h"

int C_PARASETCMD::PnfnProc(strings &strReplys)
{
	int err = 0;
	C_FNOBJ *pFnObj;
	S_WORD PN = 0, FN = 0;
	string DataUnit;
	//parase PNFN
	
	//while (FN... && err==0){
	if ((pFnObj = GetExtFnObj(FN)) != NULL)
		err = pFnObj->FnProc(PN, DataUnit, strReplys);
	else if ((pFnObj = GetBaseFnObj(FN)) != NULL)
		err = pFnObj->FnProc(PN, DataUnit, strReplys);
	//}
	return CMDPARSE_ERROR;
}

C_FNOBJ *C_PARASETCMD::GetExtFnObj(S_WORD FN)
{
	return NULL;
}

C_FNOBJ *C_PARASETCMD::GetBaseFnObj(S_WORD FN)
{
	return NULL;
}




int C_PARAGETCMD::PnfnProc(strings &strReplys)
{
	int err = 0;
	C_FNOBJ *pFnObj;
	S_WORD PN = 0, FN = 0;
	string DataUnit;
	//parase PNFN
	
	//while (FN... && err==0){
	if ((pFnObj = GetExtFnObj(FN)) != NULL)
		err = pFnObj->FnProc(PN, DataUnit, strReplys);
	else if ((pFnObj = GetBaseFnObj(FN)) != NULL)
		err = pFnObj->FnProc(PN, DataUnit, strReplys);
	//}
	return CMDPARSE_ERROR;
}

C_FNOBJ *C_PARAGETCMD::GetExtFnObj(S_WORD FN)
{
	return NULL;
}

C_FNOBJ *C_PARAGETCMD::GetBaseFnObj(S_WORD FN)
{
	return NULL;
}


