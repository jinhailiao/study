//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haictrl.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-12-24  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#ifndef _HAICTRL_H_
#define _HAICTRL_H_

#include "haitype.h"
#include "haihandle.h"


#define SCENE_FOCUS		0
#define HG_GETFOCUS		0
#define HG_KILLFOCUS		1


//ctrl proc prototype
//
typedef  S_DWORD Hai_CtrlProc(SH_CTRL, S_BYTE, WPARAM, LPARAM);
typedef  S_DWORD (*pHai_CtrlProc)(SH_CTRL, S_BYTE, WPARAM, LPARAM);


//ctrl type
//
enum
{
	CT_NULL,
	CT_START,

	CT_BUTTON,
	CT_EVTAREA,
	CT_LISTAREA,
	CT_SCRLBAR,
	CT_STATIC,
	CT_PROGRESS,
	CT_EDITBOX,
	CT_INPUTM,
	CT_SCRLTEXT,

	CT_END
};

enum
{
	INPUTMETHOD_DISABLE,
	INPUTMETHOD_ENABLE,
	INPUTMETHOD_GET = 0xFF
};

//ctrl comm style
//
#define CS_VISIBLE			0x80000000
#define CS_GRAYED			0x40000000
#define CS_GROUP_FLAG		0x20000000

#define CS_TEXT_CENTER		0x00000000
#define CS_TEXT_LEFT		0x00000010
#define CS_TEXT_RIGHT		0x00000020
#define CS_TEXT_MIDDLE		0x00000000
#define CS_TEXT_TOP			0x00000040
#define CS_TEXT_BUTTOM		0x00000080


//button style      note: only 10 bit
//
#define BS_PUSHBTN			0x00000000
#define BS_CHECKBTN			0x00000001
#define BS_RADIOBTN			0x00000002
#define BS_SELECTBTN		0x00000003

//button state
//
#define BS_PUSHBTN_UP		0x00000000
#define BS_PUSHBTN_DN		0x00000001

#define BS_CHECKBTN_NOCHECK		0x00000000
#define BS_CHECKBTN_CHECKED		0x00000001

//scroll bar
//
#define BS_VSCRLBAR		0x00000000
#define BS_HSCRLBAR		0x00000001

#define SB_BOTTOM			1
#define SB_ENDSCROLL		2/*not used*/
#define SB_LINEDOWN			3
#define SB_LINEUP			4
#define SB_PAGEDOWN			5
#define SB_PAGEUP			6
#define SB_THUMBPOSITION	7/*lParam is new position*/
#define SB_THUMBTRACK		8/*lParam is new position*/
#define SB_TOP				9

//progress bar
//
#define PBS_RULE		0x00000000
#define PBS_SLOT		0x00000001
#define PBS_BLOCK		0x00000002

#define PBS_HPROGRESS	0x00000000
#define PBS_VPROGRESS	0x00000008

//edit box
//
#define BS_EDITBOX_NULL			0x00000000
#define BS_EDITBOX_UNDERLINE	0x00000001
#define BS_EDITBOX_BORDER		0x00000002
#define BS_EDITBOX_3DBOX		0x00000003

#define BS_EDITBOX_NORMAL		0x00000000
#define BS_EDITBOX_PASSWORD		0x00000010

//input methed
//
#define  INPUTM_TYPE_NONE				0
#define  INPUTM_TYPE_WUBI				1
#define  INPUTM_TYPE_PY			2


SH_CTRL hai_SetCapture(SH_CTRL hCtrl);
S_BOOL  hai_ReleaseCapture(S_VOID);
SH_CTRL hai_CreateCtrl(S_BYTE CtrlType, S_BYTE CtrlID, S_DWORD style, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h);
S_BOOL  hai_DestroyCtrl(SH_CTRL hCtrl);
S_BOOL  hai_DestroyAllCtrl(S_VOID);
S_BOOL  hai_ShowCtrl(SH_CTRL hCtrl, S_BOOL bShow);
S_BOOL  hai_ShowAllCtrl(S_VOID);
S_BOOL  hai_EnableCtrl(SH_CTRL hCtrl, S_BOOL bEnable);
SH_CTRL hai_GetCurSceneCtrl(S_BYTE ID);
S_BYTE  hai_GetCurSceneCtrlID(SH_CTRL hCtrl);
SH_CTRL hai_GetCurSceneFocusCtrl(S_VOID);
S_BOOL hai_SetFocusCtrl(SH_CTRL hCtrl);
S_DWORD  hai_SendCtrlEvt(SH_CTRL hCtrl, S_BYTE EventType, WPARAM wParam, LPARAM lParam);
S_BOOL hai_CreateInputMethod(S_BYTE InputMethodType);
S_BOOL hai_DestroyInputMethod(S_VOID);
SH_CTRL hai_GetSpecifiedCtrl(S_SHORT x, S_SHORT y);

S_BOOL  hai_SetButtonCtrlInfo(SH_CTRL hCtrl, S_BYTE *pText, S_BYTE  *UpPicID, S_BYTE  *DownPicID, S_BYTE  *GrayPicID);
SH_CTRL hai_CreateButton(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BYTE *pText, S_BYTE  *UpPicID, S_BYTE  *DownPicID, S_BYTE  *GrayPicID);
S_BOOL hai_IsButtonChecked(SH_CTRL hCtrl);
S_BOOL hai_CheckButton(SH_CTRL hCtrl, S_BOOL checked);
S_BOOL hai_CheckRadioButton(SH_CTRL hCtrl);

S_BOOL hai_SetStaticCtrlInfo(SH_CTRL hCtrl, S_BYTE *pText, S_BYTE *PicID);
SH_CTRL hai_CreateStatic(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BYTE *pText, S_BYTE *PicID);

S_BOOL hai_SetScrlBarCtrlInfo(SH_CTRL hCtrl, S_DWORD TotalLine, S_DWORD CurStart, S_WORD LinePerPage);
SH_CTRL hai_CreateScrlBar(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_DWORD TotalLine, S_DWORD CurStart, S_WORD LinePerPage);
S_BOOL hai_GetScrlBarInfo(SH_CTRL hCtrl, S_DWORD *CurStart, S_DWORD *TotalLines, S_WORD *LinesPerPage);
S_BOOL hai_SetScrlBarInfo(SH_CTRL hCtrl, S_DWORD CurStart, S_DWORD TotalLines, S_WORD LinesPerPage);

S_BOOL hai_SetEditBoxCtrlInfo(SH_CTRL hCtrl, S_BYTE *pText, S_WORD TextLength);
SH_CTRL hai_CreateEditBox(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BYTE *pText, S_WORD TextLength);
S_BYTE *hai_GetEditBoxData(SH_CTRL hCtrl);

S_BOOL hai_SetEvtAreaCtrlInfo(SH_CTRL hCtrl, S_DWORD EvtCode);
SH_CTRL hai_CreateEvtArea(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_DWORD EvtCode);
S_BOOL hai_SetListAreaCtrlInfo(SH_CTRL hCtrl, S_SHORT LineH);
SH_CTRL hai_CreateListArea(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_SHORT LineH);


//function   : SH_CTRL hai_CreateProgress(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_SHORT total, S_SHORT MinPos, S_SHORT CurPos, S_SHORT MaxPos, S_BYTE  *BGPicID, S_BYTE  *FGPicID);
//description: w和h 在内部会缩小6个像素,以便画焦点和标尺等等.
//             pos从0开始计算.total是指总的刻度(包括0),total必须大于MaxPos;
//
SH_CTRL hai_CreateProgress(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_SHORT total, S_SHORT MinPos, S_SHORT CurPos, S_SHORT MaxPos, S_BYTE  *BGPicID, S_BYTE  *FGPicID);
S_BOOL hai_SetProgressCtrlInfo(SH_CTRL hCtrl, S_SHORT total, S_SHORT MinPos, S_SHORT CurPos, S_SHORT MaxPos, S_BYTE  *BGPicID, S_BYTE  *FGPicID);

S_SHORT hai_GetProgressCurPos(SH_CTRL hCtrl);
S_BOOL hai_SetProgressCurPos(SH_CTRL hCtrl, S_SHORT CurPos);


#endif //_HAICTRL_H_


