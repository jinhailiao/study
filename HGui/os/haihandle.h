//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haihandle.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-09-06  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#ifndef  _HAIHANDLE_H_
#define  _HAIHANDLE_H_

#define  S_HDLTYPE    S_WORD

enum
{
	HT_NONE,

	HT_WND,
	HT_DC,
	HT_PEN,
	HT_BITMAP,
	HT_BRUSH,
	HT_PALLETTE,
	HT_FONT,
	HT_RGN,
	HT_BUTTON,
	HT_MENU,
	HT_ICON,
	HT_CARET,
	HT_CTRL,
	HT_FILE,

	HT_END
};

HAI_DECLARE_HANDLE(SH_WND);
HAI_DECLARE_HANDLE(SH_DC);
HAI_DECLARE_HANDLE(SH_PEN);
HAI_DECLARE_HANDLE(SH_BRUSH);
HAI_DECLARE_HANDLE(SH_FONT);
HAI_DECLARE_HANDLE(SH_BITMAP);
HAI_DECLARE_HANDLE(SH_BUTTON);
HAI_DECLARE_HANDLE(SH_RGN);
HAI_DECLARE_HANDLE(SH_PALLETTE);
HAI_DECLARE_HANDLE(SH_MENU);
HAI_DECLARE_HANDLE(SH_GDIOBJ);
HAI_DECLARE_HANDLE(SH_ICON);
HAI_DECLARE_HANDLE(SH_CTRL);
HAI_DECLARE_HANDLE(SH_FILE);
HAI_DECLARE_HANDLE(SH_DLG);

#define  HAI_GETHDLTYPE(hdl)	( (hdl) ? (*((S_WORD*)(hdl))) : ((S_WORD)HT_NONE) )
#define  HAI_SETHDLTYPE(hdl,tp)	if ((hdl)!=S_NULL) (*((S_WORD*)(hdl)) = (S_WORD)(tp))



#endif //_HAIHANDLE_H_
