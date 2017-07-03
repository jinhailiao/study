//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			appids.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-08	Create					Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#ifndef _APPIDS_H_
#define _APPIDS_H_

#include "sysappid.h"

enum//USER APP ID
{
	APPID_DEMO = APPID_USER_START,
	APPID_AAA,//APPID example
	APPID_BBB,

	APPID_END
};

enum//USER SCENE ID
{
	SID_PICALPHA = SID_USERID_START,
	SID_PICPARALL,
	SID_PICSTRETCH,
	SID_CTRLDEMO1,
	SID_CTRLDEMO2,
	SID_TEST,

	SID_END
};


#endif //_APPIDS_H_
