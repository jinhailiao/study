//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			_haictrl.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-12-24  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#ifndef __HAICTRL_H__
#define __HAICTRL_H__

#include "haitype.h"
#include "haictrl.h"


#define  CTRL_TEXT_LENGTH_MAX			128

//input method
//
#define  INPUTMETHOD_TEXT_LENGTH_MAX	10
#define  INPUTM_TEXT_LINE_MAX			3
#define  INPUTM_INPUT_TEXT_LENGTH_MAX	8
#define  INPUTM_DISP_TEXT_LENGTH_MAX	20


typedef struct tagPushBtn
{
	S_BYTE  *UpPicID;
	S_BYTE  *DownPicID;
	S_BYTE  *GrayPicID;
	S_BYTE  *pText;//[CTRL_TEXT_LENGTH_MAX];
}
S_BUTTON, *SP_BUTTON;

typedef struct tagEvtArea
{
	S_DWORD EvtCode;
}
S_EVTAREA, *SP_EVTAREA;

typedef struct tagListArea
{
	S_SHORT  LineH;
	S_SHORT  TotalLine;
}
S_LISTAREA, *SP_LISTAREA;

typedef struct tagScrollBar
{
	S_BYTE ClickArea;
	S_BYTE reserved;
	S_SHORT PriorMouseY;
	S_DWORD TotalLine;
	S_DWORD CurStart;
	S_WORD  HighLight;
	S_WORD  LinePerPage;
	S_WORD  ThumbPos;
	S_WORD  ThumbW;
}
S_SCRLBAR, *SP_SCRLBAR;

typedef struct tagStatic
{
	S_BYTE  *PicID;
	S_BYTE  *pText;//[CTRL_TEXT_LENGTH_MAX];
}
S_STATIC, *SP_STATIC;

typedef struct tagProgress
{
	S_BYTE  *BGPicID;
	S_BYTE  *FGPicID;
	S_SHORT total;
	S_SHORT MinPos;
	S_SHORT CurPos;
	S_SHORT MaxPos;
}
S_PROGESS, *SP_PROGRESS;

typedef struct tagEditBox
{
	S_BYTE *pText;
	S_WORD TextLength; //in byte
//	S_WORD CurTextLeng;
	S_SHORT CursorX;
}
S_EDITBOX, *SP_EDITBOX;

typedef struct tagInputMethod
{
	S_BYTE *pInputText;
	S_BYTE *DisplayText[INPUTM_TEXT_LINE_MAX];
	S_SHORT InputIndicate;
	S_WORD  CurrDataIndex;
}
S_INPUTMETHOD, *SP_INPUTMETHOD;

typedef struct tagControl
{
	S_WORD  HdlType;

	S_UINT CtrlType : 4;
	S_UINT group    : 4;
	S_UINT CtrlID   : 8;
	S_UINT visible  : 1;
	S_UINT grayed   : 1;
	S_UINT focus    : 1;
	S_UINT state    : 2;
	S_UINT reserved : 1;
	S_UINT style    : 10;
	S_SHORT x;
	S_SHORT y;
	S_SHORT w;
	S_SHORT h;
	struct tagControl *prev;
	struct tagControl *next;
	Hai_CtrlProc *CtrlProc;
	union
	{
		S_BUTTON   ButtonData;
		S_EVTAREA  EvtAreaData;
		S_LISTAREA ListAreaData;
		S_SCRLBAR  ScrlBarData;
		S_STATIC   StaticData;
		S_PROGESS  ProgressData;
		S_EDITBOX  EditBoxData;
		S_INPUTMETHOD InputMethodData;
	};
}
S_CTRL, *SP_CTRL;

#define CTRL_CHECKBTN_FLAG_W	12

#define CS_STYLE_MASK		0x00003FFF
#define CS_TEXT_LF_MASK		0x00000030
#define CS_TEXT_TB_MASK		0x000000C0

#define BS_BTN_TYPE_MASK	0x00000007

#define BS_SCRLBAR_VH_MASK	0x00000001
#define BS_SCRLBAR_WIDTH	16

#define BS_EDITBOX_STYLE_MASK	0x0000000F
#define BS_EDITBOX_PW_MASK		0x00000010

#define BS_PROGRESS_VHMASK	0x00000004
#define BS_PROGRESS_STYLE_MASK	0x0000000F


S_BOOL _CantCtrlGetFocus(SP_CTRL pCtrl);
S_BOOL _AlwaysGrayCtrl(SP_CTRL pCtrl);
S_VOID _hai_InitCtrlSystem(S_VOID);
S_DWORD hai_ButtonCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam);
S_DWORD hai_EvtAreaCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam);
S_DWORD hai_ListAreaCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam);
S_DWORD hai_ScrlBarCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam);
S_DWORD hai_StaticCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam);
S_DWORD hai_ProgressCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam);
Hai_CtrlProc *hai_GetCtrlTypeProc(S_BYTE CtrlType);
S_BOOL _hai_SetFocusCtrl(SH_CTRL hCtrl, S_BOOL draw);
S_BOOL _hai_CheckRadioButton(SH_CTRL hCtrl, S_BOOL bshow);
S_BOOL _hai_SetNextFocusCtrl(S_VOID);
SP_CTRL _hai_CreateInputMethod(S_BYTE InputMethodType);
S_BOOL _hai_DestroyInputMethod(SP_CTRL pInputCtrl);
SP_CTRL hai_GetCurrentInputMethod(S_VOID);
S_BOOL _hai_DestroyAllCtrl(S_BYTE bShow);
S_BOOL _hai_DestroyCtrl(SH_CTRL hCtrl, S_BYTE bShow);



#endif //_HAILIB_H_
