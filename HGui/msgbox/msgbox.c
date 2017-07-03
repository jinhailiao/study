//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			msgbox.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2008-02-16  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#include "haitype.h"
#include "msgbox.h"
#include "haihandle.h"
#include "_haiscene.h"
#include "haiscene.h"
#include "appids.h"
#include "_haievent.h"
#include "haievent.h"
#include "_haigdi.h"
#include "haigdi.h"
#include "haigui.h"
#include "haimem.h"
#include "virkey.h"
#include "haicaret.h"
#include "haikey.h"


#define MB_TITLE_MASK			0x00000008L
#define MB_TEXT_MASK			0x00000004L
#define MB_BTN_MASK			0x000000F0L
#define MB_ICON_MASK			0x00000F00L
#define MB_BKICON_MASK		0x0000F000L
#define MB_BTN_FOCUS_MASK		0x00000003L
#define MB_DELAY_MASK			0x80000000L
#define MB_RESTOREBG_MASK		0x40000000L

#define MB_TITLEBAR_HEIGHT		20
#define MB_TOP_HEIGHT			6
#define MB_BTNBAR_HEIGHT		24
#define MB_BOTTOM_HEIGHT		6
#define MB_TEXTLINE_MAX		10
#define MB_ICON_W				30
#define MB_ICON_H				30
#define MB_BTN_W				50
#define MB_BTN_H				20
#define MB_BTN_SPACEBETWEEN		10
#define MB_SIDE_W				6
#define MB_ROW_SPACING		4

typedef struct tagMsgboxSceneDat
{
	S_LONG delay;
	S_DWORD dwflag;
	S_BYTE *ptitle;
	S_BYTE *ptext[MB_TEXTLINE_MAX+1];
}
S_MSGBOXDAT, *SP_MSGBOXDAT;


S_SCENE gMsgbox[APPID_SYS9+1];

S_VOID _InitMsgBoxSystem(S_VOID)
{
	memset(gMsgbox, 0x00, sizeof(gMsgbox));
}

S_BYTE _FindMsgBoxID(S_VOID)
{
	S_BYTE i;

	for (i = APPID_SYS1; i <= APPID_SYS9; i++)
		if (gMsgbox[i].AppID == APPID_NULL)
			return i;
		
	return APPID_NULL;
}

S_VOID _SaveCurrentScene4MsgBox(S_BYTE MsgBoxID)
{
	SP_SCENE pScene = hai_GetCurSceneAddr();

	memcpy(&gMsgbox[MsgBoxID], pScene, sizeof(*pScene));
}

S_VOID _RestoreCurrentScene4MsgBox(S_BYTE MsgBoxID)
{
	SP_SCENE pScene = hai_GetCurSceneAddr();

	memcpy(pScene, &gMsgbox[MsgBoxID], sizeof(*pScene));
}

S_VOID _ClearSceneInfo4MsgBox(S_BYTE MsgBoxID)
{
	memset(&gMsgbox[MsgBoxID], 0x00, sizeof(gMsgbox[MsgBoxID]));
}

static S_DWORD GetMsgBoxIcon(S_DWORD dwFlag)
{
	switch (dwFlag&MB_ICON_MASK)
	{
		case MB_ICONNO:
			return 0;
		case MB_ICON_NEW:
			return 0;
		case MB_ICON_WARNING:
			return 0;
		case MB_ICON_QUESTION:
			return 0;
		case MB_ICON_COPY:
			return 0;
		case MB_ICON_DELETE:
			return 0;
		default:
			return 0;
	}
}

static S_DWORD GetMsgBoxBKIcon(S_DWORD dwFlag)
{
	switch (dwFlag&MB_BKICON_MASK)
	{
		default:
			return 0;
	}
}

static const S_BYTE *MsgYesCan[] = {"确 定", "取 消"};
S_VOID _MsgBoxProc(SP_EVT pEvt)
{
	S_BYTE b, bk, ID, evt;
	S_WORD w, x = MB_SIDE_W, y = MB_ROW_SPACING;
	S_COLOR c;
	S_BYTE **pptext;
	SH_DC hDC;
	_SP_FONT pfont;
	S_PAINTSTRUCT ps;
//	S_RECT rect;
	S_SCENE scene;
	SH_CTRL hctrl;
	SP_RECT pInvalidRect = hai_GetInvalidRect();
	SP_RECT pSceneRect = hai_GetSceneRect(S_NULL, S_NULL, S_NULL, S_NULL);
	SP_SCENE pCurScene = hai_GetCurSceneAddr();
	SP_MSGBOXDAT pMsgDat = hai_GetSceneData(0);

	if (pCurScene->AppID != pEvt->AppID)
		return;
	
	switch (pEvt->EventType)
	{
	case EVT_CREAT:
		if (pMsgDat->dwflag & MB_BTN_MASK)
		{
			S_SHORT m, n = pSceneRect->h-MB_BOTTOM_HEIGHT-MB_BTN_H+2;
			SH_CTRL hCtrl;
			switch (pMsgDat->dwflag & MB_BTN_MASK)
			{
			case MB_BTNOK:
				m = (pSceneRect->w - MB_BTN_W) >> 1;
				hCtrl = hai_CreateButton(BS_PUSHBTN, 1, m, n, MB_BTN_W, MB_BTN_H, "确 定", S_NULL, S_NULL, S_NULL);
				hai_ShowCtrl(hCtrl, S_TRUE);
				hai_SetFocusCtrl(hCtrl);
				break;
			case MB_BTNOKCANCEL:
				m = (pSceneRect->w - (MB_BTN_W<<1)-MB_BTN_SPACEBETWEEN) >> 1;
				for (b = 0; b < 2; b++, m += MB_BTN_W+MB_BTN_SPACEBETWEEN)
				{
					hCtrl = hai_CreateButton(BS_PUSHBTN, (S_BYTE)(b+1), m, n, MB_BTN_W, MB_BTN_H, (S_BYTE*)MsgYesCan[b], S_NULL, S_NULL, S_NULL);
					hai_ShowCtrl(hCtrl, S_TRUE);
					if ((pMsgDat->dwflag&MB_BTN_FOCUS_MASK) == (S_DWORD)b)
						hai_SetFocusCtrl(hCtrl);
				}
				break;
			default:
				break;
			}
		}
		else if ((pMsgDat->dwflag & MB_DELAY_MASK) == MB_DELAY)
			pMsgDat->delay = 8;
		else
			hai_SendEvent(EVT_DESTROY, MB_RTN_NULL, 0);
		break;
	case EVT_PAINT:
		if (!hai_IsRectCovered(pSceneRect, pInvalidRect))
		{
			scene = *pCurScene;
			*pCurScene = gMsgbox[pEvt->AppID];
			b = hai_SetScreenFlush(SCREEN_FLUSH_OFF);
			hai_SendEvent(EVT_PAINT, 0, 0);
			hai_SetScreenFlush(b);
			*pCurScene = scene;
		}
		hDC = hai_BeginPaint(&ps);
		if (pMsgDat->dwflag & MB_BKICON_MASK)
		{
		}
		else
		{
			if (pMsgDat->ptitle)
				hai_DrawWndFrame(hDC, 0, 0, pSceneRect->w, pSceneRect->h, S_NULL);
			else
			{
				hai_Draw3DBlock(hDC, 0, 0, pSceneRect->w, pSceneRect->h, S_TRUE, GRAY_BRUSH);
				hai_Draw3DBlock(hDC, 2, 2, (S_SHORT)(pSceneRect->w-4), (S_SHORT)(pSceneRect->h-4), S_FALSE, LTGRAY_BRUSH);
			}
		}
		pfont = (_SP_FONT)((SP_DC)hDC)->hFont;
		c = hai_SetTextColor(hDC, COLOR_BLACK);
		bk = hai_SetBkMode(hDC, BKM_TRANSPARENT);
		if (pMsgDat->ptitle)
		{
			if ((pMsgDat->dwflag & MB_TITLE_MASK) == MB_TITLE_LEFT)
				hai_TextOut(hDC, 4, 2, pMsgDat->ptitle, 0);
			else
			{
				hai_GetStringExtent(hDC, pMsgDat->ptitle, &w, S_NULL);
				hai_TextOut(hDC, (S_SHORT)((pSceneRect->w-w)/2), 2, pMsgDat->ptitle, 0);
			}
			y += MB_TITLEBAR_HEIGHT;
		}
		else
			y += MB_TOP_HEIGHT;
		if (pMsgDat->dwflag&MB_ICON_MASK)
		{
			S_DWORD IconID = GetMsgBoxIcon(pMsgDat->dwflag);
			hai_DrawIcon(hDC, (S_SHORT)(x-2), (S_SHORT)(y-8), (SH_ICON)hai_LoadBitmap(MAKEINTRESOURCE(IconID)));
			x += MB_ICON_W+2;
		}
		pptext = pMsgDat->ptext; 
		while(*pptext)
		{
			if ((pMsgDat->dwflag&MB_TEXT_MASK) == MB_TEXT_LEFT)
			{
				hai_TextOut(hDC, x, y, *pptext, 0);
			}
			else
			{
				hai_GetStringExtent(hDC, *pptext, &w, S_NULL);
				w = ((pSceneRect->w-x-MB_SIDE_W+2)-w)/2;
				hai_TextOut(hDC, (S_SHORT)(x+w), y, *pptext, 0);
			}
			y += (pfont->height+MB_ROW_SPACING);
			pptext++;
		}
		hai_SetTextColor(hDC, c);
		hai_SetBkMode(hDC, bk);

		hai_EndPaint(&ps);
		break;
	case EVT_KEYUP:
		if (!(pMsgDat->dwflag & MB_BTN_MASK))
			break;
		if ((pMsgDat->dwflag & MB_BTN_MASK) == MB_BTNOK)
			break;
		if ((hctrl = hai_GetCurSceneFocusCtrl()) == S_NULL)
			break;
		ID = hai_GetCurSceneCtrlID(hctrl);
		switch (pEvt->wParam)
		{
		case VK_LEFT:
			if (ID == 1)
				break;
			hctrl = hai_GetCurSceneCtrl((S_BYTE)(ID-1));
			hai_SetFocusCtrl(hctrl);
			break;
		case VK_RIGHT:
			if (ID == 2)//TODO: Max buttons number
				break;
			hctrl = hai_GetCurSceneCtrl((S_BYTE)(ID+1));
			hai_SetFocusCtrl(hctrl);
			break;
		default:
			break;
		}
		break;
	case EVT_COMMAND:
		if (!(pMsgDat->dwflag & MB_BTN_MASK))
			break;
		evt = HAI_GETHIBYTE(pEvt->wParam);
		ID = HAI_GETLOBYTE(pEvt->wParam);
		switch (ID)
		{
		case 1:
			if (evt == BS_PUSHBTN_UP)
				hai_SendEvent(EVT_DESTROY, MB_RTN_YESOK, 0);
			break;
		case 2:
			if (evt == BS_PUSHBTN_UP)
				hai_SendEvent(EVT_DESTROY, MB_RTN_CANCEL, 0);
			break;
		default:
			break;
		}
		break;
	case EVT_TIMER:
		scene = *pCurScene;//time msg send to pre scene, 
		*pCurScene = gMsgbox[pEvt->AppID];
		b = hai_SetScreenFlush(SCREEN_FLUSH_OFF);
		hai_SendEvent(EVT_TIMER, 0, 0);
		hai_SetScreenFlush(b);
		*pCurScene = scene;
		if (hai_IsNeedRedraw())
		{
			if (hai_IsRectIntersect(pInvalidRect, pSceneRect))
				hai_SendEvent(EVT_PAINT, 0, 0);
			else
				_hai_UpdateRect();
		}
		if ((pMsgDat->dwflag & MB_DELAY_MASK) == MB_DELAY){
			if (pMsgDat->delay-- <= 0)
			{
				hai_SendEvent(EVT_DESTROY, MB_RTN_NULL, 0);
				break;
			}
		}
		break;
	case EVT_DESTROY:
		_hai_SendQuitEvent(pEvt->wParam, pEvt->lParam);
	case EVT_EXIT:
		b = hai_SetScreenFlush(SCREEN_FLUSH_OFF);
		_hai_DestroyAllCtrl(S_FALSE);
		hai_SetScreenFlush(b);
		break;
	case EVT_CTRLCOLOR:
		break;
		
	default:
		break;
	}
}

S_WORD hai_MsgBox(S_BYTE *ptitle, S_BYTE *ptext, S_DWORD dwFlag)
{
	S_BYTE LineCnt, MsgboxID, CaretShow;
	S_WORD MsgBoxX, MsgBoxY, MsgBoxW, MsgBoxH, StrLength;
	S_BYTE *pLineFeed, *CurText;
	SH_DC hDC;
	_SP_FONT pFont;
	SP_SCENE pScene;
	S_EVT event;
	S_MSGBOXDAT msgDat;
	S_CARET BackCaret;

	
	if ((MsgboxID = _FindMsgBoxID()) == APPID_NULL)
		return MB_RTN_ERROR;
	if (ptext == S_NULL)
		return MB_RTN_ERROR;
	
	CurText = hai_MemAlloc(strlen(ptext)+2);
	if (CurText == S_NULL)
		return MB_RTN_ERROR;
	strcpy(CurText, ptext);

	if (ptitle && !ptitle[0])
		ptitle = S_NULL;

	if ((hDC = hai_GetSceneDC()) == S_NULL)
	{
		hai_MemFree(CurText);
		return MB_RTN_ERROR;
	}
	pFont = (_SP_FONT)((SP_DC)hDC)->hFont;
	
	memset(&msgDat, 0x00, sizeof(msgDat));
	MsgBoxH = MsgBoxW = StrLength = 0;
	msgDat.ptext[0] = CurText;
	for (LineCnt = 0; LineCnt < MB_TEXTLINE_MAX; LineCnt++)
	{
		pLineFeed = strchr(msgDat.ptext[LineCnt], '\n');
		if (pLineFeed)
			*pLineFeed = 0;
		hai_GetStringExtent(hDC, msgDat.ptext[LineCnt], &StrLength, S_NULL);
		MsgBoxW = MsgBoxW > StrLength? MsgBoxW:StrLength;
		if (!pLineFeed)
			break;
		msgDat.ptext[LineCnt+1] = pLineFeed+1;
	}
	LineCnt += 1;
	if (dwFlag & MB_ICON_MASK)
		MsgBoxW += MB_ICON_W;
	MsgBoxH += LineCnt * (pFont->height+MB_ROW_SPACING) + MB_ROW_SPACING;
	if (dwFlag & MB_ICON_MASK)
		MsgBoxH = MsgBoxH>(MB_ICON_H+MB_ROW_SPACING)? MsgBoxH:(MB_ICON_H+MB_ROW_SPACING);
	if (ptitle)
	{
		MsgBoxH += MB_TITLEBAR_HEIGHT;
		hai_GetStringExtent(hDC, ptitle, &StrLength, S_NULL);
		MsgBoxW = MsgBoxW > StrLength? MsgBoxW:StrLength;	
	}
	else
		MsgBoxH += MB_TOP_HEIGHT;
	hai_ReleaseDC(hDC);
	if (dwFlag & MB_BTN_MASK)
	{
		MsgBoxH += MB_BTNBAR_HEIGHT;
		if ((dwFlag & MB_BTN_MASK) == MB_BTNOK)
			StrLength = MB_BTN_W + 5;
//		else if ((dwFlag & MB_BTN_MASK) == MB_BTNYESNOCANCEL)
//			StrLength = (MB_BTN_W + MB_BTN_SPACEBETWEEN)*3;
		else
			StrLength = (MB_BTN_W + MB_BTN_SPACEBETWEEN)*2;
		StrLength += 6;
		MsgBoxW = MsgBoxW > StrLength? MsgBoxW:StrLength;
	}
	else
		MsgBoxH += MB_BOTTOM_HEIGHT;
	MsgBoxW += MB_SIDE_W * 2;
	
	if (MsgBoxW+20 >= LCD_WIDTH || MsgBoxH+10 >= LCD_HEIGHT)
	{
		hai_MemFree(CurText);
		return MB_RTN_ERROR;
	}
	
	if (dwFlag & MB_BKICON_MASK)
	{
	}
	else
	{
		MsgBoxX = (LCD_WIDTH - MsgBoxW)/2;
		MsgBoxY = (LCD_HEIGHT - MsgBoxH)/2;
	}
	
	msgDat.ptitle = ptitle;
	msgDat.dwflag = dwFlag;

	CaretShow = hai_HideCaret();
	HG_SaveCurCaret(&BackCaret, sizeof(S_CARET));
	
	_SaveCurrentScene4MsgBox(MsgboxID);
	pScene = hai_GetCurSceneAddr();
	hai_SetScene(pScene, MsgboxID, SID_MSGBOX, &msgDat, _MsgBoxProc);
	pScene->SceneRect.x = MsgBoxX;
	pScene->SceneRect.y = MsgBoxY;
	pScene->SceneRect.w = MsgBoxW;
	pScene->SceneRect.h = MsgBoxH;
//	hai_InitEventQueue();
	_hai_SetInvalidRectEx(MsgBoxX, MsgBoxY, MsgBoxW, MsgBoxH);
	hai_SendEvent(EVT_CREAT, 0, 0);
	hai_SendEvent(EVT_PAINT, 0, 0);
			
	while (1)
	{
		event = hai_GetEvent();
		if (event.EventType == EVT_QUIT)
			break;
		hai_EventPreprocess(&event);
		pScene->pSceneProc(&event);
		if (event.EventType == EVT_EXIT)
		{
			event.wParam = MB_RTN_NULL;
			break;
		}
		else
			hai_EventEndprocess(&event);
	}

	hai_MemFree(CurText);
	hai_GetSceneRect(&MsgBoxX, &MsgBoxY, &MsgBoxW, &MsgBoxH);
	_RestoreCurrentScene4MsgBox(MsgboxID);
	_ClearSceneInfo4MsgBox(MsgboxID);
	HG_RestoreCurCaret(&BackCaret);
	if ((dwFlag & MB_RESTOREBG_MASK) == MB_RESTORE_BG)
	{
		hai_InvalidRectEx(MsgBoxX, MsgBoxY, MsgBoxW, MsgBoxH);
		if (CaretShow)
			hai_ShowCaret();
	}
	return (S_WORD)event.wParam;
}

S_VOID _OpenMsgBoxProc(SP_EVT pEvt)
{
	S_BYTE b, bk;
	S_WORD w, x = MB_SIDE_W, y = MB_ROW_SPACING;
	S_COLOR c;
	S_BYTE **pptext;
	SH_DC hDC;
	_SP_FONT pfont;
	S_PAINTSTRUCT ps;
	SP_RECT pInvalidRect = hai_GetInvalidRect();
	SP_RECT pSceneRect = hai_GetSceneRect(S_NULL, S_NULL, S_NULL, S_NULL);
	S_SCENE scene;
	SP_SCENE pCurScene = hai_GetCurSceneAddr();
	SP_MSGBOXDAT pMsgDat = hai_GetSceneData(0);

	switch (pEvt->EventType)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		if (!hai_IsRectCovered(pSceneRect, pInvalidRect))
		{
			scene = *pCurScene;
			*pCurScene = gMsgbox[pEvt->AppID];
			b = hai_SetScreenFlush(SCREEN_FLUSH_OFF);
			hai_SendEvent(EVT_PAINT, 0, 0);
			hai_SetScreenFlush(b);
			*pCurScene = scene;
		}
		hDC = hai_BeginPaint(&ps);
		if (pMsgDat->dwflag & MB_BKICON_MASK)
		{
		}
		else
		{
			if (pMsgDat->ptitle)
				hai_DrawWndFrame(hDC, 0, 0, pSceneRect->w, pSceneRect->h, S_NULL);
			else
			{
				hai_Draw3DBlock(hDC, 0, 0, pSceneRect->w, pSceneRect->h, S_TRUE, GRAY_BRUSH);
				hai_Draw3DBlock(hDC, 2, 2, (S_SHORT)(pSceneRect->w-4), (S_SHORT)(pSceneRect->h-4), S_FALSE, LTGRAY_BRUSH);
			}
		}
		pfont = (_SP_FONT)((SP_DC)hDC)->hFont;
		c = hai_SetTextColor(hDC, COLOR_BLACK);
		bk = hai_SetBkMode(hDC, BKM_TRANSPARENT);
		if (pMsgDat->ptitle)
		{
			if ((pMsgDat->dwflag & MB_TITLE_MASK) == MB_TITLE_LEFT)
				hai_TextOut(hDC, 4, 2, pMsgDat->ptitle, 0);
			else
			{
				hai_GetStringExtent(hDC, pMsgDat->ptitle, &w, S_NULL);
				hai_TextOut(hDC, (S_SHORT)((pSceneRect->w-w)/2), 2, pMsgDat->ptitle, 0);
			}
			y += MB_TITLEBAR_HEIGHT;
		}
		else
			y += MB_TOP_HEIGHT;
		if (pMsgDat->dwflag&MB_ICON_MASK)
		{
			S_DWORD IconID = GetMsgBoxIcon(pMsgDat->dwflag);
			hai_DrawIcon(hDC, (S_SHORT)(x-2), (S_SHORT)(y-8), (SH_ICON)hai_LoadBitmap(MAKEINTRESOURCE(IconID)));
			x += MB_ICON_W+2;
		}
		pptext = pMsgDat->ptext; 
		while(*pptext)
		{
			if ((pMsgDat->dwflag&MB_TEXT_MASK) == MB_TEXT_LEFT)
			{
				hai_TextOut(hDC, x, y, *pptext, 0);
			}
			else
			{
				hai_GetStringExtent(hDC, *pptext, &w, S_NULL);
				w = ((pSceneRect->w-x-MB_SIDE_W+2)-w)/2;
				hai_TextOut(hDC, (S_SHORT)(x+w), y, *pptext, 0);
			}
			y += (pfont->height+MB_ROW_SPACING);
			pptext++;
		}
		hai_SetTextColor(hDC, c);
		hai_SetBkMode(hDC, bk);
		hai_EndPaint(&ps);
		break;
	case EVT_KEYUP:
		break;
	case EVT_COMMAND:
		break;
	case EVT_TIMER:
		break;
	case EVT_DESTROY:
		break;
	case EVT_CTRLCOLOR:
		break;
		
	default:
		break;
	}
}

typedef struct tagOpenMsgBox
{
	S_BYTE MsgBoxID;
	S_BYTE CaretShow;
}
S_OMB, *SP_OMB;

SH_DLG hai_OpenMsgBox(S_BYTE *ptitle, S_BYTE *ptext, S_DWORD dwFlag)
{
	S_BYTE LineCnt;
	S_WORD MsgBoxX, MsgBoxY, MsgBoxW, MsgBoxH, StrLength;
	S_BYTE MsgboxID;
	S_BYTE *pLineFeed, *CurText = S_NULL;
	SH_DC hDC;
	_SP_FONT pFont;
	SP_SCENE pScene;
	SP_OMB pOMB = S_NULL;
	SP_MSGBOXDAT pmsgDat = S_NULL;

	if ((MsgboxID = _FindMsgBoxID()) == APPID_NULL)
		return (SH_DLG)S_NULL;
	if (ptext == S_NULL)
		return (SH_DLG)S_NULL;

	dwFlag = dwFlag&~((S_DWORD)MB_BTN_FOCUS_MASK);
	dwFlag = dwFlag&~((S_DWORD)MB_BTN_MASK);
	
	CurText = hai_MemAlloc(strlen(ptext)+2);
	if (CurText == S_NULL)
		return (SH_DLG)S_NULL;
	strcpy(CurText, ptext);
	pmsgDat = hai_MemAlloc(sizeof(*pmsgDat));
	if (pmsgDat == S_NULL)
		goto ERR_OPENMSGBOX;
	memset(pmsgDat, 0x00, sizeof(*pmsgDat));
	if (ptitle && !ptitle[0])
		ptitle = S_NULL;
	if (ptitle)
	{
		pmsgDat->ptitle = hai_MemAlloc(strlen(ptitle)+2);
		if (pmsgDat->ptitle == S_NULL)
			goto ERR_OPENMSGBOX;
		strcpy(pmsgDat->ptitle, ptitle);
	}

	hDC = hai_GetSceneDC();
	if (hDC == S_NULL)
		goto ERR_OPENMSGBOX;
	
	pFont = (_SP_FONT)((SP_DC)hDC)->hFont;
	
	MsgBoxH = MsgBoxW = StrLength = 0;
	pmsgDat->ptext[0] = CurText;
	for (LineCnt = 0; LineCnt < MB_TEXTLINE_MAX; LineCnt++)
	{
		pLineFeed = strchr(pmsgDat->ptext[LineCnt], '\n');
		if (pLineFeed)
			*pLineFeed = 0;
		hai_GetStringExtent(hDC, pmsgDat->ptext[LineCnt], &StrLength, S_NULL);
		MsgBoxW = MsgBoxW > StrLength? MsgBoxW:StrLength;
		if (!pLineFeed)
			break;
		pmsgDat->ptext[LineCnt+1] = pLineFeed+1;
	}
	LineCnt += 1;
	if (dwFlag & MB_ICON_MASK)
		MsgBoxW += MB_ICON_W;
	MsgBoxH += LineCnt * (pFont->height+MB_ROW_SPACING) + MB_ROW_SPACING;
	if (dwFlag & MB_ICON_MASK)
		MsgBoxH = MsgBoxH>(MB_ICON_H+MB_ROW_SPACING)? MsgBoxH:(MB_ICON_H+MB_ROW_SPACING);
	if (ptitle)
	{
		MsgBoxH += MB_TITLEBAR_HEIGHT;
		hai_GetStringExtent(hDC, ptitle, &StrLength, S_NULL);
		MsgBoxW = MsgBoxW > StrLength? MsgBoxW:StrLength;	
	}
	else
		MsgBoxH += MB_TOP_HEIGHT;
	hai_ReleaseDC(hDC);
	if (dwFlag & MB_BTN_MASK)
	{
		MsgBoxH += MB_BTNBAR_HEIGHT;
		if ((dwFlag & MB_BTN_MASK) == MB_BTNOK)
			StrLength = MB_BTN_W + 5;
//		else if ((dwFlag & MB_BTN_MASK) == MB_BTNYESNOCANCEL)
//			StrLength = (MB_BTN_W + MB_BTN_SPACEBETWEEN)*3;
		else
			StrLength = (MB_BTN_W + MB_BTN_SPACEBETWEEN)*2;
		StrLength += 6;
		MsgBoxW = MsgBoxW > StrLength? MsgBoxW:StrLength;
	}
	else
		MsgBoxH += MB_BOTTOM_HEIGHT;
	MsgBoxW += MB_SIDE_W * 2;
	
	if (MsgBoxW+20 >= LCD_WIDTH || MsgBoxH+10 >= LCD_HEIGHT)
		goto ERR_OPENMSGBOX;
	
	if (dwFlag & MB_BKICON_MASK)
	{
	}
	else
	{
		MsgBoxX = (LCD_WIDTH - MsgBoxW)/2;
		MsgBoxY = (LCD_HEIGHT - MsgBoxH)/2;
	}

	pmsgDat->ptitle = ptitle;
	pmsgDat->dwflag = dwFlag;

	pOMB = hai_MemAlloc(sizeof(S_OMB));
	if (pOMB = S_NULL)
		goto ERR_OPENMSGBOX;
	HG_SetUserInputFunction(USERIP_DISABLE);
	pOMB->MsgBoxID = MsgboxID;
	pOMB->CaretShow = hai_HideCaret();
	_SaveCurrentScene4MsgBox(MsgboxID);
	pScene = hai_GetCurSceneAddr();
	hai_SetScene(pScene, MsgboxID, SID_MSGBOX, pmsgDat, _OpenMsgBoxProc);
	pScene->SceneRect.x = MsgBoxX;
	pScene->SceneRect.y = MsgBoxY;
	pScene->SceneRect.w = MsgBoxW;
	pScene->SceneRect.h = MsgBoxH;
//	hai_InitEventQueue();
	_hai_SetInvalidRectEx(MsgBoxX, MsgBoxY, MsgBoxW, MsgBoxH);
	hai_SendEvent(EVT_CREAT, 0, 0);
	hai_SendEvent(EVT_PAINT, 0, 0);
			
	return (SH_DLG)pOMB;

ERR_OPENMSGBOX:

	if (CurText) hai_MemFree(CurText);
	if (pOMB) hai_MemFree(pOMB);
	if (pmsgDat)
	{
		if (pmsgDat->ptitle)
			hai_MemFree(pmsgDat->ptitle);
		hai_MemFree(pmsgDat);
	}
	return (SH_DLG)S_NULL;
}

S_WORD hai_CloseMsgBox(SH_DLG hMsgbox, S_DWORD dwFlag)
{
	S_BYTE MsgboxID = ((SP_OMB)hMsgbox)->MsgBoxID;
	S_WORD MsgBoxX, MsgBoxY, MsgBoxW, MsgBoxH;
	SP_MSGBOXDAT pmsgDat = hai_GetSceneData(0);

	if (pmsgDat->ptitle)
		hai_MemFree(pmsgDat->ptitle);
	hai_MemFree(pmsgDat->ptext[0]);
	hai_GetSceneRect(&MsgBoxX, &MsgBoxY, &MsgBoxW, &MsgBoxH);
	hai_ReleaseSceneData();
	_RestoreCurrentScene4MsgBox(MsgboxID);
	_ClearSceneInfo4MsgBox(MsgboxID);
	if ((dwFlag & MB_RESTOREBG_MASK) == MB_RESTORE_BG)
	{
		hai_InvalidRectEx(MsgBoxX, MsgBoxY, MsgBoxW, MsgBoxH);
		if (((SP_OMB)hMsgbox)->CaretShow)
			hai_ShowCaret();
	}
	HG_SetUserInputFunction(USERIP_ENABLE);

	return MB_RTN_YESOK;
}












