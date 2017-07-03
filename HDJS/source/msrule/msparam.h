//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			msparam.h
// Description:		MS parameter config
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-07-20  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __MSPARAMETER_H__
#define __MSPARAMETER_H__

#include "haidef.h"
#include "mscmd.h"


/** 参数设置
 */
class C_PARASETCMD:public C_AFNOBJ
{
public:
	virtual S_BOOL NeedPassword(void){return true;}
	virtual int PnfnProc(strings &strReplys);

protected:
	C_FNOBJ *GetExtFnObj(S_WORD FN);
	C_FNOBJ *GetBaseFnObj(S_WORD FN);
};

/** 参数获取
 */
class C_PARAGETCMD:public C_AFNOBJ
{
public:
	virtual S_BOOL NeedPassword(void){return false;}
	virtual int PnfnProc(strings &strReplys);

protected:
	C_FNOBJ *GetExtFnObj(S_WORD FN);
	C_FNOBJ *GetBaseFnObj(S_WORD FN);
};


#endif //__MSPARAMETER_H__


