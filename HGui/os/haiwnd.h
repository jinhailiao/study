//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haiwnd.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-09-06  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#ifndef  _HAIWND_H_
#define  _HAIWND_H_

#include "haitype.h"
#include "haihandle.h"


typedef struct tagWnd
{
	S_HDLTYPE	hdltype;

	const S_BYTE*	ptitle;
	S_WORD		style;
	S_SHORT		x;
	S_SHORT		y;
	S_SHORT		width;
	S_SHORT		height;
	SH_BRUSH	hBrush;
	S_WORD		ChildID;
	S_VOID*		pWinData;
	SH_WND		ChildWndList;
}
S_WND, *SP_WND;






#endif //_HAIWND_H_