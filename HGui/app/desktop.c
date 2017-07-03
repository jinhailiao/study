//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			desktop.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#include "schedule.h"
#include "haievent.h"
#include "stdio.h"
#include "haigdi.h"
#include "haitime.h"
#include "haihandle.h"
#include "bmpicon.h"
#include "virkey.h"
#include "haicaret.h"
#include "haimem.h"
#include "AppIDs.h"
#include "haictrl.h"
#include "haigui.h"
#include "haiscene.h"
#include "msgbox.h"
#include "dialog.h"
#include "hailib.h"

S_BYTE buffer[40];

S_VOID GetNameDlgProc(SP_EVT pEvt)
{
	S_BYTE ID, evt;
	SH_DC hDC;
	SP_RECT pRect = hai_GetSceneRect(S_NULL, S_NULL, S_NULL, S_NULL);
	S_PAINTSTRUCT ps;
	
	HG_DialogBoxPreproc(pEvt);
	switch (pEvt->EventType)
	{
	case EVT_CREAT:
		hai_SetSceneIME(INPUTMETHOD_ENABLE);
		hai_CreateButton(BS_PUSHBTN, 1, 60, 64, 60, 20, "确定", S_NULL, S_NULL, S_NULL);
		hai_CreateEditBox(BS_EDITBOX_BORDER, 2, 40, 32, 130, 20, S_NULL, 20);
		hai_ShowAllCtrl();
		hai_SetFocusCtrl(hai_GetCurSceneCtrl(2));
		break;
	case EVT_PAINT:
		hDC = hai_BeginPaint(&ps);
		hai_DrawWndFrame(hDC, 0, 0, pRect->w, pRect->h, "请输入姓名");
		hai_SetBkMode(hDC, BKM_TRANSPARENT);
		hai_TextOut(hDC, 8, 38, "姓名:", 0);
		hai_EndPaint(&ps);
		break;
	case EVT_KEYUP:
		if (pEvt->wParam == VK_RETURN)
			hai_SendCtrlEvt(hai_GetCurSceneCtrl(1), EVT_KEYUP, pEvt->wParam, pEvt->lParam);
		else if (pEvt->wParam == VK_ESCAPE)
			hai_SendEvent(EVT_DESTROY, 0, 0);
		break;
	case EVT_COMMAND:
		evt = HAI_GETHIBYTE(pEvt->wParam);
		ID = HAI_GETLOBYTE(pEvt->wParam);
		switch (ID)
		{
		case 1:
			if (evt == BS_PUSHBTN_UP)
			{
				S_BYTE *pDat = hai_GetEditBoxData(hai_GetCurSceneCtrl(2));
				if (*pDat)
				{
					hai_strcpy(buffer, pDat);
					hai_SendEvent(EVT_DESTROY, 0, (LPARAM)buffer);
				}
				else
					hai_SendEvent(EVT_DESTROY, 0, 0);
			}
			break;
		default:
			break;
		}
		break;
	case EVT_DESTROY:
		HG_EndDialogBox(pEvt->lParam);
		break;
	default:
		break;
	}
}

DEFINESCENE(SID_DESKTOP)
MSGMAPBEGIN
	ONCREAT(DTOnCreat)
	ONMSG(EVT_RESTORE, DTOnRestore)
	ONMSG(EVT_PAINT, DTOnPaint)
	ONUSERMSG(EVT_TIMER, DTOnTimer)
	ONDESTROY(DTOnDestroy)
	ONKEYDN(DTOnKey)
	ONMSG(EVT_EXIT, DTOnExit)
	ONMSG(EVT_COMMAND, DTOnCommand)
MSGMAPEND

#define DT_TIME_W		76
#define DT_TIME_H		16
#define DT_TIME_X		(LCD_WIDTH-DT_TIME_W-2)
#define DT_TIME_Y		(LCD_HEIGHT-DT_TIME_H-2)

#ifdef DEBUG_MEMHEAP
S_DWORD TotalSize, BlkNum, MaxBlkSize;
#endif
static const S_BYTE * const ShortCut[8] = 
{
	"控件1", "控件2", "图拉伸", "图隐现", "图层", "测试", "黄花菜", "分蘖节"
};
static const S_DWORD IconSel[] =
{
	IDI_WHITEBOOK, IDI_WHITEWORD
};
static const S_DWORD IconNor[] =
{
	IDI_GRAYBOOK, IDI_GRAYWORD	
};


S_VOID hai_DisplayTime(SH_DC hDC, S_WORD x, S_WORD y, S_WORD w, S_WORD h)
{
	S_BYTE time[14] = {0x20, 0x20, 0x20, 0x20};

	hai_GetTime(&time[4]);
	hai_DrawButton(hDC, x, y, w, h, S_FALSE, time);
}

static void  DTOnCreat(SP_EVT pEvent)
{
	S_BYTE i;

	for (i = 0; i < 8; i++)
		hai_CreateButton(BS_SELECTBTN|CS_TEXT_BUTTOM, (S_BYTE)(i+1), (S_SHORT)(120+(i%4)*60), (S_SHORT)(50+(i/4)*70), 32, 44, (S_BYTE*)ShortCut[i], MAKEINTRESOURCE(IconSel[i/4]), MAKEINTRESOURCE(IconNor[i/4]), MAKEINTRESOURCE(IconSel[i/4]));
	hai_CreateButton(BS_PUSHBTN, 10, 2, (LCD_HEIGHT-16-2), 40, 16, "开始", 0, 0, 0);
	hai_ShowAllCtrl();

#ifdef DEBUG_MEMHEAP
	TotalSize = hai_MemStatus(&BlkNum, &MaxBlkSize);
#endif
}

static void  DTOnRestore(SP_EVT pEvent)
{
#ifdef DEBUG_MEMHEAP
	S_DWORD a,b,c;

	a = hai_MemStatus(&b, &c);
	if (a != TotalSize || b != BlkNum || c != MaxBlkSize)
		hai_MsgBox("警告!", "内存泻漏!", MB_BTNOK);
#endif
}

static void  DTOnPaint(SP_EVT pEvent)
{
	S_PAINTSTRUCT Paint;
	SH_DC hDC;

	hDC = hai_BeginPaint(&Paint);
	hai_BitBlt(hDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, S_NULL, 0, 0, WHITENESS);
	hai_Draw3DBlock(hDC, 0, LCD_HEIGHT-20, LCD_WIDTH, 20, S_TRUE, GRAY_BRUSH);
	hai_DisplayTime(hDC, DT_TIME_X, DT_TIME_Y, DT_TIME_W, DT_TIME_H);
	hai_SetTextColor(hDC, (S_COLOR)COLOR_GRAY);
	hai_TextOut(hDC,  70, 180, "温馨提示:F1为输入法切换按键,F2,F3,F4,F5为消息框测试按键,", 0);
	hai_TextOut(hDC, 124, 200, "请在各个版面试按,谢了!", 0);
	hai_EndPaint(&Paint);
}

static void  DTOnTimer(SP_EVT pEvent)
{
	SH_DC hDC;

	hDC = hai_GetSceneDC();
	hai_DisplayTime(hDC, DT_TIME_X, DT_TIME_Y, DT_TIME_W, DT_TIME_H);
	hai_ReleaseDC(hDC);
	hai_UpdateWindow(DT_TIME_X, DT_TIME_Y, DT_TIME_W, DT_TIME_H);
}

static void  DTOnDestroy(SP_EVT pEvent)
{
}

static void  DTOnCommand(SP_EVT pEvent)
{
	S_BYTE CtrlID = HAI_GETLOBYTE(pEvent->wParam);
	S_BYTE Event  = HAI_GETHIBYTE(pEvent->wParam);
	switch (CtrlID)
	{
	case 1:
		if (Event == EVT_MOUSEUP)
		{
			S_BYTE *pName = (S_BYTE*)HG_DialogBox(140, 90, 180, 90, GetNameDlgProc);
			if (pName)
				GOTOSCENE(SID_CTRLDEMO1, 23, (LPARAM)pName);
		}
		break;
	case 2:
		if (Event == EVT_MOUSEUP)
			GOTOSCENE(SID_CTRLDEMO2, 50, 60);
		break;
	case 3:
		if (Event == EVT_MOUSEUP)
			GOTOSCENE(SID_PICSTRETCH, 50, 60);
		break;
	case 4:
		if (Event == EVT_MOUSEUP)
			GOTOSCENE(SID_PICALPHA, 50, 60);
		break;
	case 5:
		if (Event == EVT_MOUSEUP)
			GOTOSCENE(SID_PICPARALL, 50, 60);
		break;
	case 6:
		if (Event == EVT_MOUSEUP)
			GOTOSCENE(SID_TEST, 50, 60);
		break;
	case 7:
		break;
	case 8:
		break;
	case 10:
		if (Event == BS_PUSHBTN_UP)
			hai_MsgBox("Welcome", "欢迎体验HAISOFT的HGui系统!", MB_BTNOK|MB_RESTORE_BG);
		break;
	default:
		break;
	}
}

static void  DTOnKey(SP_EVT pEvent)
{
	SH_CTRL hFocus = hai_GetCurSceneFocusCtrl();
	S_BYTE CtrlID  = hai_GetCurSceneCtrlID(hFocus);

	switch (pEvent->wParam)
	{
	case VK_LEFT:
		if (CtrlID > 1 && CtrlID < 9)
		{
			CtrlID -= 1;
			hFocus = hai_GetCurSceneCtrl(CtrlID);
			hai_SetFocusCtrl(hFocus);
		}
		else if (CtrlID == 1)
		{
			CtrlID = 8;
			hFocus = hai_GetCurSceneCtrl(CtrlID);
			hai_SetFocusCtrl(hFocus);
		}
		break;
	case VK_RIGHT:
		if (CtrlID > 0 && CtrlID < 8)
		{
			CtrlID += 1;
			hFocus = hai_GetCurSceneCtrl(CtrlID);
			hai_SetFocusCtrl(hFocus);
		}
		else if (CtrlID == 8)
		{
			CtrlID = 1;
			hFocus = hai_GetCurSceneCtrl(CtrlID);
			hai_SetFocusCtrl(hFocus);
		}
		break;
	case VK_UP:
	case VK_DOWN:
		if (CtrlID > 4 && CtrlID < 9)
		{
			CtrlID -= 4;
			hFocus = hai_GetCurSceneCtrl(CtrlID);
			hai_SetFocusCtrl(hFocus);
		}
		else if (CtrlID > 0 && CtrlID < 5)
		{
			CtrlID += 4;
			hFocus = hai_GetCurSceneCtrl(CtrlID);
			hai_SetFocusCtrl(hFocus);
		}
		break;
	default:
		break;
	}
}

static void  DTOnExit(SP_EVT pEvent)
{//desktop scene:在初始场景要做特殊化处理
	CALLPREGOTOSCENE();
	pEvent->EventType = EVT_NULL;
}

