//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			msgbox.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-09-16  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef _MSGBOX_H_
#define _MSGBOX_H_

#include "haitype.h"
#include "haihandle.h"


#define MB_TITLE_CENTER			0x00000000
#define MB_TITLE_LEFT			0x00000008
#define MB_TEXT_CENTER			0x00000000
#define MB_TEXT_LEFT			0x00000004

#define MB_NO_DELAY			0x00000000
#define MB_DELAY				0x80000000

#define MB_RESTORE_BG			0x00000000
#define MB_NO_RESTORE_BG		0x40000000

#define MB_BTNNULL				0x00000000
#define MB_BTNOK				0x00000010
#define MB_BTNOKCANCEL			0x00000020
//#define MB_BTNYESNO			0x00000030
//#define MB_BTNYESNOCANCEL		0x00000040

#define MB_BTN_FOCUS_YESOK	0x00000000
#define MB_BTN_FOCUS_NOCAN	0x00000001
//#define MB_BTN_FOCUS_CANCEL	0x00000002

#define MB_ICONNO				0x00000000
#define MB_ICON_NEW			0x00000100
#define MB_ICON_WARNING		0x00000200
#define MB_ICON_QUESTION		0x00000300
#define MB_ICON_COPY			0x00000400
#define MB_ICON_DELETE			0x00000500

#define MB_BKICON_NO			0x00000000

#define MB_RTN_NULL				0x0000
#define MB_RTN_YESOK			0x0001
#define MB_RTN_NO				0x0002
#define MB_RTN_CANCEL			MB_RTN_NO
#define MB_RTN_ERROR			0xFFFF


S_WORD hai_MsgBox(S_BYTE *ptitle, S_BYTE *ptext, S_DWORD dwFlag);
SH_DLG hai_OpenMsgBox(S_BYTE *ptitle, S_BYTE *ptext, S_DWORD dwFlag);
S_WORD hai_CloseMsgBox(SH_DLG hMsgbox, S_DWORD dwFlag);


#endif //_MSGBOX_H_


