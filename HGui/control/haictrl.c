//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haictrl.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-12-24  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#include "haitype.h"
#include "haihandle.h"
#include "_haictrl.h"
#include "haictrl.h"
#include "_haiscene.h"
#include "haiscene.h"
#include "_haigdi.h"
#include "haigdi.h"
#include "haimem.h"
#include "hailib.h"
#include "haigui.h"
#include "virkey.h"
#include "haitime.h"
#include "haicaret.h"
#include "bmpicon.h"

static SH_CTRL pCtrlCapture = S_NULL;


Hai_CtrlProc *hai_GetCtrlTypeProc(S_BYTE CtrlType);

S_VOID _hai_InitCtrlSystem(S_VOID)
{
	pCtrlCapture = S_NULL;
}

SH_CTRL hai_SetCapture(SH_CTRL hCtrl)
{
	SH_CTRL pPrevCapture = pCtrlCapture;

	if (hCtrl)
		pCtrlCapture = hCtrl;

	return pPrevCapture;
}

S_BOOL hai_ReleaseCapture(S_VOID)
{
	if (pCtrlCapture)
	{
		pCtrlCapture = S_NULL;
		return S_TRUE;
	}
	else
		return S_FALSE;
}

SH_CTRL hai_CreateCtrl(S_BYTE CtrlType, S_BYTE CtrlID, S_DWORD style, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h)
{
	SP_CTRL pCtrl;
	SP_CTRL pListHead, pListEnd;

	if (!(CtrlType > CT_START && CtrlType < CT_END) || !CtrlID)
		return S_NULL;
	if (x < 0 || y < 0 || x >= LCD_WIDTH || y >= LCD_HEIGHT)
		return S_NULL;
	if (w < 0 || h < 0 || w > LCD_WIDTH || h > LCD_HEIGHT)
		return S_NULL;

	pCtrl = hai_MemAlloc(sizeof(S_CTRL));
	if (!pCtrl)
		return S_NULL;
	memset(pCtrl, 0x00, sizeof(S_CTRL));

	HAI_SETHDLTYPE(pCtrl, HT_CTRL);
	pListHead = hai_GetCurSceneCtrlListHead();
	pListEnd  = hai_GetCurSceneCtrlListEnd();
	if (pListEnd)
	{
		pCtrl->prev = S_NULL;
		pListHead->prev = pCtrl;
	}
	else
	{
		pCtrl->prev = S_NULL;
		pListEnd    = pCtrl;
		hai_SetCurSceneCtrlListEnd(pListEnd);
	}
	pCtrl->next = pListHead;
	pListHead   = pCtrl;
	hai_SetCurSceneCtrlListHead(pListHead);

	pCtrl->CtrlType = CtrlType;
	pCtrl->CtrlID   = CtrlID;
	pCtrl->visible  = !!(style & CS_VISIBLE);
	pCtrl->grayed   = !!(style & CS_GRAYED);
	pCtrl->style    = style & CS_STYLE_MASK;
	pCtrl->x = x;
	pCtrl->y = y;
	pCtrl->w = w;
	pCtrl->h = h;
	if (style & CS_GROUP_FLAG)
	{
		if (pCtrl->prev)
			pCtrl->group = (pCtrl->prev)->group + 1;
		else
			pCtrl->group = 0;
	}
	else
	{
		if (pCtrl->prev)
			pCtrl->group = (pCtrl->prev)->group;
		else
			pCtrl->group = 0;
	}
	pCtrl->CtrlProc = hai_GetCtrlTypeProc(CtrlType);

	return (SH_CTRL)pCtrl;
}

S_BOOL _hai_DestroyCtrl(SH_CTRL hCtrl, S_BYTE bShow)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	SP_CTRL pListHead, pListEnd;

	if (!hCtrl || HAI_GETHDLTYPE(hCtrl) != HT_CTRL)
		return S_FALSE;
	if (hai_SetCapture(S_NULL) == hCtrl)
		hai_ReleaseCapture();

	pListHead = hai_GetCurSceneCtrlListHead();
	pListEnd  = hai_GetCurSceneCtrlListEnd();
	if (pCtrl->next)
	{
		if (pCtrl->prev)
			pCtrl->prev->next = pCtrl->next;
		else
		{
			pListHead = pCtrl->next;
			pListHead->prev = S_NULL;
			hai_SetCurSceneCtrlListHead(pListHead);
		}
	}
	else
	{
		pListEnd = pCtrl->prev;
		if (pListEnd)
			pListEnd->next = S_NULL;
		hai_SetCurSceneCtrlListEnd(pListEnd);
	}
	if (pCtrl->prev)
	{
		if (pCtrl->next)
			pCtrl->next->prev = pCtrl->prev;
		else
		{
			pListEnd = pCtrl->prev;
			pListEnd->next = S_NULL;
			hai_SetCurSceneCtrlListEnd(pListEnd);
		}
	}
	else
	{
		pListHead = pCtrl->next;
		if (pListHead)
			pListHead->prev = S_NULL;
		hai_SetCurSceneCtrlListHead(pListHead);
	}

	pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_DESTROY, 0, 0);
	switch (pCtrl->CtrlType)
	{
	case CT_BUTTON:
		if (pCtrl->ButtonData.pText)
			hai_MemFree(pCtrl->ButtonData.pText);
		break;
	case CT_STATIC:
		if (pCtrl->StaticData.pText)
			hai_MemFree(pCtrl->StaticData.pText);
		break;
	case CT_EDITBOX:
		if (pCtrl->EditBoxData.pText)
			hai_MemFree(pCtrl->EditBoxData.pText);
	default:
		break;
	}

	if (bShow && !hai_IsNeedRedraw())
	{
		S_WORD x, y;
		hai_GetSceneRect(&x, &y, S_NULL, S_NULL);
		hai_InvalidRectEx((S_WORD)(x+pCtrl->x), (S_WORD)(y+pCtrl->y), pCtrl->w, pCtrl->h);
	}
	hai_MemFree(pCtrl);

	return S_TRUE;
}

S_BOOL hai_DestroyCtrl(SH_CTRL hCtrl)
{
	return _hai_DestroyCtrl(hCtrl, S_TRUE);
}

S_BOOL _hai_DestroyAllCtrl(S_BYTE bShow)
{
	S_WORD x, y;
	SP_CTRL pListHead;

	hai_GetSceneRect(&x, &y, S_NULL, S_NULL);
	
	pListHead = hai_GetCurSceneCtrlListHead();
	if (!pListHead)
		return S_FALSE;

	while (pListHead)
	{
		if (bShow)
			_hai_SetInvalidRectEx((S_WORD)(x+pListHead->x), (S_WORD)(y+pListHead->y), pListHead->w, pListHead->h);
		_hai_DestroyCtrl((SH_CTRL)pListHead, bShow);
		pListHead = hai_GetCurSceneCtrlListHead();
	}

	if (bShow)
		hai_SendEvent(EVT_PAINT, 0, 0);
	
	return S_TRUE;
}

S_BOOL hai_DestroyAllCtrl(S_VOID)
{
	return _hai_DestroyAllCtrl(S_TRUE);
}

S_BOOL hai_ShowCtrl(SH_CTRL hCtrl, S_BOOL bShow)
{
	if (!hCtrl || ((SP_CTRL)hCtrl)->visible == bShow)
		return S_FALSE;

	((SP_CTRL)hCtrl)->visible = bShow;
	if (!hai_IsNeedRedraw())
		((SP_CTRL)hCtrl)->CtrlProc(hCtrl, EVT_PAINT, 0, 0);

	return S_TRUE;
}

S_BOOL hai_ShowAllCtrl(S_VOID)
{
	SP_CTRL pCtrl;

	pCtrl = hai_GetCurSceneCtrlListHead();
	if (!pCtrl)
		return S_FALSE;

	while (pCtrl)
	{
		if (pCtrl->visible != 1)
		{
			pCtrl->visible = 1;
			if (!hai_IsNeedRedraw())
				pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_PAINT, 0, 0);
		}
		pCtrl = pCtrl->next;
	}

	return S_TRUE;
}

S_BOOL hai_EnableCtrl(SH_CTRL hCtrl, S_BOOL bEnable)
{
	if (!hCtrl)
		return S_FALSE;
	if (_AlwaysGrayCtrl((SP_CTRL)hCtrl))
		return S_FALSE;
	
	if (((SP_CTRL)hCtrl)->grayed == (S_UINT)(!bEnable))
		return S_FALSE;

	((SP_CTRL)hCtrl)->grayed = !bEnable;
	((SP_CTRL)hCtrl)->CtrlProc(hCtrl, EVT_PAINT, 0, 0);

	return S_TRUE;
}

SH_CTRL hai_GetCurSceneCtrl(S_BYTE ID)
{
	SP_CTRL pCtrl;

	pCtrl = hai_GetCurSceneCtrlListHead();
	while (pCtrl)
	{
		if (pCtrl->CtrlID == ID)
			break;
		pCtrl = pCtrl->next;
	}

	return (SH_CTRL)pCtrl;
}

S_BYTE hai_GetCurSceneCtrlID(SH_CTRL hCtrl)
{
	if (!hCtrl)
		return 0;
	return ((SP_CTRL)hCtrl)->CtrlID;
}

SH_CTRL hai_GetSpecifiedCtrl(S_SHORT x, S_SHORT y)
{
	SP_CTRL pCtrl = hai_GetCurSceneCtrlListHead();

	while (pCtrl)
	{
		if (pCtrl->visible && !pCtrl->grayed)
			if (x>pCtrl->x && x<pCtrl->x+pCtrl->w && y>pCtrl->y && y<pCtrl->y+pCtrl->h)
				break;
		pCtrl = pCtrl->next;
	}

	return (SH_CTRL)pCtrl;
}

S_BOOL _hai_SetNextFocusCtrl(S_VOID)
{
	SP_CTRL pCtrl, pOldFocusCtrl = S_NULL;

	pCtrl = hai_GetCurSceneCtrlListEnd();
	if (!pCtrl)
		return S_FALSE;

	while (pCtrl) //found current focus ctrl
	{
		if (pCtrl->focus)
		{
			pOldFocusCtrl = pCtrl;
			break;
		}
		pCtrl = pCtrl->prev;
	}

	if (pCtrl)//current focus at ctrl
	{
		pCtrl = pCtrl->prev;
		while (pCtrl)
		{
			if (pCtrl->visible && !pCtrl->grayed && !_CantCtrlGetFocus(pCtrl))
				break;
			pCtrl = pCtrl->prev;
		}
	}
	else //current focus at scene
	{
		pCtrl = hai_GetCurSceneCtrlListEnd();
		while (pCtrl)
		{
			if (pCtrl->visible && !pCtrl->grayed && !_CantCtrlGetFocus(pCtrl))
				break;
			pCtrl = pCtrl->prev;
		}
	}

	if (pCtrl)
	{
		pCtrl->focus = 1;
		if (pOldFocusCtrl)
		{
			pOldFocusCtrl->focus = 0;
			pOldFocusCtrl->CtrlProc((SH_CTRL)pOldFocusCtrl, EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_KILLFOCUS, pCtrl->CtrlID), 0);
			pOldFocusCtrl->CtrlProc((SH_CTRL)pOldFocusCtrl, EVT_PAINT, 0, 0);
			pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_GETFOCUS, pOldFocusCtrl->CtrlID), 0);
		}
		else
		{
			hai_SendEvent(EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_KILLFOCUS, pCtrl->CtrlID), 0);
			pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_GETFOCUS, SCENE_FOCUS), 0);
		}
		pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_PAINT, 0, 0);
	}
	else
	{
		if (pOldFocusCtrl)
		{
			pOldFocusCtrl->focus = 0;
			pOldFocusCtrl->CtrlProc((SH_CTRL)pOldFocusCtrl, EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_KILLFOCUS, SCENE_FOCUS), 0);
			pOldFocusCtrl->CtrlProc((SH_CTRL)pOldFocusCtrl, EVT_PAINT, 0, 0);
			hai_SendEvent(EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_GETFOCUS, pOldFocusCtrl->CtrlID), 0);
		}
	}
	return S_TRUE;
}

SH_CTRL hai_GetCurSceneFocusCtrl(S_VOID)
{
	SP_CTRL pCtrl;

	pCtrl = hai_GetCurSceneCtrlListHead();
	while (pCtrl)
	{
		if (pCtrl->focus)
			break;
		pCtrl = pCtrl->next;
	}
	return (SH_CTRL)pCtrl;
}

S_BOOL _hai_SetFocusCtrl(SH_CTRL hCtrl, S_BOOL draw)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	if (!hCtrl || !pCtrl->visible || pCtrl->grayed)
		return S_FALSE;
	if (_CantCtrlGetFocus(pCtrl))
		return S_FALSE;

	pCtrl = hai_GetCurSceneCtrlListHead();
	while (pCtrl)
	{
		if (pCtrl->focus)
		{
			if (pCtrl == (SP_CTRL)hCtrl)
				return S_TRUE;
			else
				break;
		}
		pCtrl = pCtrl->next;
	}

	((SP_CTRL)hCtrl)->focus = 1;
	if (pCtrl)
	{
		pCtrl->focus = 0;
		pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_KILLFOCUS, ((SP_CTRL)hCtrl)->CtrlID), 0);
		pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_PAINT, 0, 0);
		((SP_CTRL)hCtrl)->CtrlProc(hCtrl, EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_GETFOCUS, pCtrl->CtrlID), 0);
	}
	else
	{
		hai_SendEvent(EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_KILLFOCUS, ((SP_CTRL)hCtrl)->CtrlID), 0);
		((SP_CTRL)hCtrl)->CtrlProc(hCtrl, EVT_FOCUS, (WPARAM)HAI_COMBWORD(HG_GETFOCUS, SCENE_FOCUS), 0);
	}
	
	if (draw)
		((SP_CTRL)hCtrl)->CtrlProc(hCtrl, EVT_PAINT, 0, 0);
	
	return S_TRUE;
}

S_BOOL hai_SetFocusCtrl(SH_CTRL hCtrl)
{
	return _hai_SetFocusCtrl(hCtrl, 1);
}

S_DWORD  hai_SendCtrlEvt(SH_CTRL hCtrl, S_BYTE EventType, WPARAM wParam, LPARAM lParam)
{
	return (((SP_CTRL)hCtrl)->CtrlProc(hCtrl, EventType, wParam, lParam));
}

#define CTRL_TEXT_DISPLAY_OFFSET	2

S_VOID hai_CtrlDrawText(SH_CTRL hCtrl, SH_DC hDC)
{
	S_BYTE *pText, bkmode = BKM_TRANSPARENT;
	S_WORD w, h;
	S_SHORT x, y;
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	S_COLOR textcolor = ((SP_DC)hDC)->TextColor;

	if (!hCtrl || !hDC)
		return;

	if (pCtrl->CtrlType == CT_BUTTON)
	{
		if (!(pCtrl->ButtonData.pText))
			return;
		pText = pCtrl->ButtonData.pText;
		hai_GetStringExtent(hDC, pCtrl->ButtonData.pText, &w, &h);
		switch (pCtrl->style & BS_BTN_TYPE_MASK)
		{
		case BS_PUSHBTN:
		case BS_SELECTBTN:
			if ((pCtrl->style & BS_BTN_TYPE_MASK) == BS_PUSHBTN)
				textcolor = (pCtrl->grayed?  COLOR_GRAY15:COLOR_GRAY00);
			if (h > pCtrl->h)
				y = pCtrl->y;
			else
			{
				if ((pCtrl->style & CS_TEXT_TB_MASK) == CS_TEXT_TOP)
					y = pCtrl->y;
				else if ((pCtrl->style & CS_TEXT_TB_MASK) == CS_TEXT_BUTTOM)
					y = (pCtrl->y + pCtrl->h) - h;
				else
					y = pCtrl->y + (pCtrl->h - h)/2;
			}
			if (w > pCtrl->w)
				x = pCtrl->x;
			else
			{
				if ((pCtrl->style & CS_TEXT_LF_MASK) == CS_TEXT_LEFT)
					x = pCtrl->x;
				else if ((pCtrl->style & CS_TEXT_LF_MASK) == CS_TEXT_RIGHT)
					x = (pCtrl->x + pCtrl->w) - w;
				else
					x = pCtrl->x + (pCtrl->w - w)/2;
			}
			break;
		case BS_CHECKBTN:
		case BS_RADIOBTN:
			if (h > pCtrl->h)
				y = pCtrl->y;
			else
				y = pCtrl->y + (pCtrl->h - h)/2;
			if (w > pCtrl->w)
				x = pCtrl->x;
			else
				x = pCtrl->x + CTRL_CHECKBTN_FLAG_W+6;
			break;
		default:
			break;
		}
	}
	else if (pCtrl->CtrlType == CT_STATIC)
	{
		if (!(pCtrl->StaticData.pText))
			return;
		pText = pCtrl->StaticData.pText;
		hai_GetStringExtent(hDC, pText, &w, &h);
		textcolor = ((SP_DC)hDC)->TextColor;
		bkmode = ((SP_DC)hDC)->BkMode;
		if (h > pCtrl->h)
			y = pCtrl->y;
		else
		{
			if ((pCtrl->style & CS_TEXT_TB_MASK) == CS_TEXT_TOP)
				y = pCtrl->y;
			else if ((pCtrl->style & CS_TEXT_TB_MASK) == CS_TEXT_BUTTOM)
				y = (pCtrl->y + pCtrl->h) - h;
			else
				y = pCtrl->y + (pCtrl->h - h)/2;
		}
		if (w > pCtrl->w)
			x = pCtrl->x;
		else
		{
			if ((pCtrl->style & CS_TEXT_LF_MASK) == CS_TEXT_LEFT)
				x = pCtrl->x;
			else if ((pCtrl->style & CS_TEXT_LF_MASK) == CS_TEXT_RIGHT)
				x = (pCtrl->x + pCtrl->w) - w;
			else
				x = pCtrl->x + (pCtrl->w - w)/2;
		}
	}
	else if (pCtrl->CtrlType == CT_EDITBOX)
	{
		if (!(pCtrl->EditBoxData.pText))
			return;
		pText = pCtrl->EditBoxData.pText;
		hai_GetStringExtent(hDC, pText, &w, &h);
		textcolor = ((SP_DC)hDC)->TextColor;
		bkmode = ((SP_DC)hDC)->BkMode;
		if (h > pCtrl->h)
			y = pCtrl->y;
		else
		{
//			if ((pCtrl->style & CS_TEXT_TB_MASK) == CS_TEXT_TOP)
//				y = pCtrl->y + CTRL_TEXT_DISPLAY_OFFSET;
//			else if ((pCtrl->style & CS_TEXT_TB_MASK) == CS_TEXT_BUTTOM)
//				y = (pCtrl->y + pCtrl->h) - h - CTRL_TEXT_DISPLAY_OFFSET;
//			else
				y = pCtrl->y + (pCtrl->h - h)/2;
		}
		if (w > pCtrl->w)
			x = pCtrl->x;
		else
		{
//			if ((pCtrl->style & CS_TEXT_LF_MASK) == CS_TEXT_LEFT)
				x = pCtrl->x + CTRL_TEXT_DISPLAY_OFFSET;
//			else if ((pCtrl->style & CS_TEXT_LF_MASK) == CS_TEXT_RIGHT)
//				x = (pCtrl->x + pCtrl->w) - w - CTRL_TEXT_DISPLAY_OFFSET;
//			else
//				x = pCtrl->x + (pCtrl->w - w)/2;
		}
	}
	else
		return;
	bkmode = hai_SetBkMode(hDC, bkmode);
	textcolor = hai_SetTextColor(hDC, textcolor);
	hai_TextOut(hDC, x, y, pText, 0);
	hai_SetTextColor(hDC, textcolor);
	hai_SetBkMode(hDC, bkmode);
}

//button
//
S_BOOL hai_SetButtonCtrlInfo(SH_CTRL hCtrl, S_BYTE *pText, S_BYTE  *UpPicID, S_BYTE  *DownPicID, S_BYTE  *GrayPicID)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	if (!pCtrl || pCtrl->CtrlType != CT_BUTTON)
		return S_FALSE;

	if (pCtrl->ButtonData.pText)
		hai_MemFree(pCtrl->ButtonData.pText);

	pCtrl->ButtonData.pText = pText;
	if (pText)
	{
		pCtrl->ButtonData.pText = hai_MemAlloc(hai_strlen(pText)+1);
		if (!pCtrl->ButtonData.pText)
			return S_FALSE;
		hai_strcpy(pCtrl->ButtonData.pText, pText);
	}
	pCtrl->ButtonData.UpPicID   = UpPicID;
	pCtrl->ButtonData.DownPicID = DownPicID;
	pCtrl->ButtonData.GrayPicID = GrayPicID;

	return S_TRUE;
}

SH_CTRL hai_CreateButton(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BYTE *pText, S_BYTE  *UpPicID, S_BYTE  *DownPicID, S_BYTE  *GrayPicID)
{
	S_BOOL  ok;
	SH_CTRL hCtrl;

	hCtrl = hai_CreateCtrl(CT_BUTTON, CtrlID, style, x, y, w, h);
	if (hCtrl)
		ok = hai_SetButtonCtrlInfo(hCtrl, pText, UpPicID, DownPicID, GrayPicID);
	if (hCtrl && !ok)
	{
		_hai_DestroyCtrl(hCtrl, S_FALSE);
		hCtrl = S_NULL;
	}
	return hCtrl;
}

S_DWORD hai_PushBtnCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	S_SHORT x, y;

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		{
			SH_DC hDC;
			SH_BITMAP hTemp, hbitmap;

			if (!pCtrl->visible)
				break;
			hDC = hai_GetSceneDC();
			hai_SendEvent(EVT_CTRLCOLOR, (WPARAM)(pCtrl->CtrlID), (LPARAM)hDC);
			if (pCtrl->grayed)
			{
				if (pCtrl->ButtonData.GrayPicID)
				{
					hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.GrayPicID));
					hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
					hai_DeleteObject((SH_GDIOBJ)hTemp);
					hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
					hai_DeleteObject((SH_GDIOBJ)hbitmap);
				}
				else
					hai_Draw3DBlock(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, BLOCK_3D_UP, DKGRAY_BRUSH);
			}
			else
			{
				if (pCtrl->state == BS_PUSHBTN_UP)
					if (pCtrl->ButtonData.UpPicID)
					{
						hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.UpPicID));
						hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
						hai_DeleteObject((SH_GDIOBJ)hTemp);
						hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
						hai_DeleteObject((SH_GDIOBJ)hbitmap);
					}
					else
					{
						hai_Draw3DBlock(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, BLOCK_3D_UP, GRAY_BRUSH);
					}
				else if (pCtrl->state == BS_PUSHBTN_DN)
					if (pCtrl->ButtonData.DownPicID)
					{
						hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.DownPicID));
						hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
						hai_DeleteObject((SH_GDIOBJ)hTemp);
						hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
						hai_DeleteObject((SH_GDIOBJ)hbitmap);
					}
					else
					{
						hai_Draw3DBlock(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, BLOCK_3D_DN, GRAY_BRUSH);
					}
			}
			if (pCtrl->ButtonData.pText)
				hai_CtrlDrawText(hCtrl, hDC);
			if (!pCtrl->grayed && pCtrl->state == BS_PUSHBTN_UP && pCtrl->focus)
				hai_DrawBoxByDashed(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y+2), (S_SHORT)(pCtrl->w-5), (S_SHORT)(pCtrl->h-5), COLOR_BLACK);
			hai_ReleaseDC(hDC);
			_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		}
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		if (wParam == ' ' || wParam == VK_RETURN)
		{
			pCtrl->state = BS_PUSHBTN_DN;
			_hai_SetFocusCtrl(hCtrl, S_FALSE);
			hai_PushBtnCtrlProc(hCtrl, EVT_PAINT, 0, 0);
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(BS_PUSHBTN_DN,pCtrl->CtrlID), 0);
		}
		else
			hai_SendEvent(EVT_KEYDN, wParam, lParam);
		break;	
	case EVT_KEYUP:
		if (wParam == ' ' || wParam == VK_RETURN)
		{
			pCtrl->state = BS_PUSHBTN_UP;
			hai_PushBtnCtrlProc(hCtrl, EVT_PAINT, 0, 0);
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(BS_PUSHBTN_UP,pCtrl->CtrlID), 0);
		}
		else
			hai_SendEvent(EVT_KEYUP, wParam, lParam);
		break;
	case EVT_CHAR:
		break;
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_MOUSEDN:
		hai_SetCapture(hCtrl);
		pCtrl->state = BS_PUSHBTN_DN;
		_hai_SetFocusCtrl(hCtrl, S_FALSE);
		hai_PushBtnCtrlProc(hCtrl, EVT_PAINT, 0, 0);
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(BS_PUSHBTN_DN,pCtrl->CtrlID), 0);
		break;
	case EVT_MOUSEUP:
		pCtrl->state = BS_PUSHBTN_UP;
		hai_PushBtnCtrlProc(hCtrl, EVT_PAINT, 0, 0);
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(BS_PUSHBTN_UP,pCtrl->CtrlID), 0);
		hai_ReleaseCapture();
		break;
	case EVT_MOUSEDB:
		hai_ReleaseCapture();
		break;
	case EVT_MOUSEMV:
		x = HAI_GETLOWORD(lParam);
		y = HAI_GETHIWORD(lParam);
		if (x>pCtrl->x && x<pCtrl->x+pCtrl->w && y>pCtrl->y && y<pCtrl->y+pCtrl->h)
		{
			if (pCtrl->state == BS_PUSHBTN_UP)
			{
				hai_SetCapture(hCtrl);
				pCtrl->state = BS_PUSHBTN_DN;
				_hai_SetFocusCtrl(hCtrl, S_FALSE);
				hai_PushBtnCtrlProc(hCtrl, EVT_PAINT, 0, 0);
			}
		}
		else
		{
			((SP_CTRL)hCtrl)->state = BS_PUSHBTN_UP;
			hai_PushBtnCtrlProc(hCtrl, EVT_PAINT, 0, 0);
			hai_ReleaseCapture();
		}
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
	return 0;
}

S_VOID hai_DrawRight(SH_DC hDC, S_SHORT x, S_SHORT y)
{
	hai_MoveTo(hDC, (S_SHORT)(x+1), (S_SHORT)(y+5), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+2), (S_SHORT)(y+5));
	hai_MoveTo(hDC, (S_SHORT)(x+2), (S_SHORT)(y+6), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+3), (S_SHORT)(y+6));
	hai_MoveTo(hDC, (S_SHORT)(x+3), (S_SHORT)(y+7), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+6), (S_SHORT)(y+7));
	hai_MoveTo(hDC, (S_SHORT)(x+4), (S_SHORT)(y+8), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+5), (S_SHORT)(y+8));
	hai_MoveTo(hDC, (S_SHORT)(x+6), (S_SHORT)(y+6), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+7), (S_SHORT)(y+6));
	hai_MoveTo(hDC, (S_SHORT)(x+7), (S_SHORT)(y+5), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+8), (S_SHORT)(y+5));
	hai_MoveTo(hDC, (S_SHORT)(x+8), (S_SHORT)(y+4), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+9), (S_SHORT)(y+4));
	hai_MoveTo(hDC, (S_SHORT)(x+9), (S_SHORT)(y+3), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+10), (S_SHORT)(y+3));
}

S_DWORD hai_CheckBtnCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		{
			SH_DC hDC;
			SH_BITMAP hTemp, hbitmap;

			if (!pCtrl->visible)
				break;
			if (pCtrl->h < CTRL_CHECKBTN_FLAG_W)
				pCtrl->h = CTRL_CHECKBTN_FLAG_W;
			hDC = hai_GetSceneDC();
			hai_SendEvent(EVT_CTRLCOLOR, (WPARAM)(pCtrl->CtrlID), (LPARAM)hDC);
			hai_BitBlt(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, S_NULL, 0, 0, PATCOPY);
			if (pCtrl->grayed)
			{
				if (pCtrl->ButtonData.GrayPicID)
					{
						hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.GrayPicID));
						hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
						hai_DeleteObject((SH_GDIOBJ)hTemp);
						hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
						hai_DeleteObject((SH_GDIOBJ)hbitmap);
					}
				else
					hai_Draw3DBlock(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y + (pCtrl->h-CTRL_CHECKBTN_FLAG_W)/2), CTRL_CHECKBTN_FLAG_W, CTRL_CHECKBTN_FLAG_W, BLOCK_3D_DN, GRAY_BRUSH);
			}
			else
			{
				if (pCtrl->state == BS_CHECKBTN_CHECKED)
					if (pCtrl->ButtonData.UpPicID)
					{
						hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.UpPicID));
						hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
						hai_DeleteObject((SH_GDIOBJ)hTemp);
						hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
						hai_DeleteObject((SH_GDIOBJ)hbitmap);
					}
					else
					{
						hai_Draw3DBlock(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y + (pCtrl->h-CTRL_CHECKBTN_FLAG_W)/2), CTRL_CHECKBTN_FLAG_W, CTRL_CHECKBTN_FLAG_W, BLOCK_3D_DN, WHITE_BRUSH);
						hai_DrawRight(hDC, (S_SHORT)(pCtrl->x + 2), (S_SHORT)(pCtrl->y + (pCtrl->h-CTRL_CHECKBTN_FLAG_W)/2));
					}
				else if (pCtrl->state == BS_CHECKBTN_NOCHECK)
					if (pCtrl->ButtonData.DownPicID)
					{
						hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.DownPicID));
						hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
						hai_DeleteObject((SH_GDIOBJ)hTemp);
						hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
						hai_DeleteObject((SH_GDIOBJ)hbitmap);
					}
					else
					{
						hai_Draw3DBlock(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y + (pCtrl->h-CTRL_CHECKBTN_FLAG_W)/2), CTRL_CHECKBTN_FLAG_W, CTRL_CHECKBTN_FLAG_W, BLOCK_3D_DN, WHITE_BRUSH);
					}
			}
			if (pCtrl->ButtonData.pText)
				hai_CtrlDrawText(hCtrl, hDC);
			if (!pCtrl->grayed && pCtrl->focus)
				hai_DrawBoxByDashed(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y+2), (S_SHORT)(pCtrl->w-5), (S_SHORT)(pCtrl->h-5), COLOR_BLACK);
			hai_ReleaseDC(hDC);
			_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		}
		break;	
	case EVT_TIMER:
		break;	
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_CHAR:
		break;
	case EVT_KEYUP:
		if (wParam != ' ' && wParam != VK_RETURN)
			hai_SendEvent(EVT_KEYUP, wParam, lParam);
		break;
	case EVT_KEYDN:
		if (wParam != ' ' && wParam != VK_RETURN)
		{
			hai_SendEvent(EVT_KEYDN, wParam, lParam);
			break;
		}
	case EVT_MOUSEDN:
		pCtrl->state = !pCtrl->state;
		_hai_SetFocusCtrl(hCtrl, S_FALSE);
		hai_CheckBtnCtrlProc(hCtrl, EVT_PAINT, 0, 0);
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(pCtrl->state,pCtrl->CtrlID), 0);
		break;
	case EVT_MOUSEUP:
		break;
	case EVT_MOUSEDB:
		break;
	case EVT_MOUSEMV:
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
	return 0;
}

S_DWORD hai_RadioBtnCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		{
			SH_DC hDC;
			SH_BITMAP hTemp, hbitmap;

			if (!pCtrl->visible)
				break;
			if (pCtrl->h < CTRL_CHECKBTN_FLAG_W)
				pCtrl->h = CTRL_CHECKBTN_FLAG_W;
			hDC = hai_GetSceneDC();
			if (pCtrl->grayed)
				hai_SelectObject(hDC, (SH_GDIOBJ)hai_GetStockObject(GRAY_BRUSH));
			hai_SendEvent(EVT_CTRLCOLOR, (WPARAM)(pCtrl->CtrlID), (LPARAM)hDC);
			hai_BitBlt(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, S_NULL, 0, 0, PATCOPY);
			if (pCtrl->grayed)
			{
				if (pCtrl->ButtonData.GrayPicID)
				{
					hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.GrayPicID));
					hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
					hai_DeleteObject((SH_GDIOBJ)hTemp);
					hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
					hai_DeleteObject((SH_GDIOBJ)hbitmap);
				}
				else
				{
					if (pCtrl->focus)
						hTemp = hai_LoadBitmap(MAKEINTRESOURCE(IDI_RADIOSELG));
					else
						hTemp = hai_LoadBitmap(MAKEINTRESOURCE(IDI_RADIONORG));
					hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
					hai_DeleteObject((SH_GDIOBJ)hTemp);
					hai_DrawIcon(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y + ((pCtrl->h-CTRL_CHECKBTN_FLAG_W)>>1)), (SH_ICON)hbitmap);
					hai_DeleteObject((SH_GDIOBJ)hbitmap);
				}
			}
			else
			{
				if (pCtrl->state == BS_CHECKBTN_CHECKED)
					if (pCtrl->ButtonData.UpPicID)
					{
						hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.UpPicID));
						hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
						hai_DeleteObject((SH_GDIOBJ)hTemp);
						hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
						hai_DeleteObject((SH_GDIOBJ)hbitmap);
					}
					else
					{
						hTemp = hai_LoadBitmap(MAKEINTRESOURCE(IDI_RADIOSEL));
						hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
						hai_DeleteObject((SH_GDIOBJ)hTemp);
						hai_DrawIcon(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y + ((pCtrl->h-CTRL_CHECKBTN_FLAG_W)>>1)), (SH_ICON)hbitmap);
						hai_DeleteObject((SH_GDIOBJ)hbitmap);
					}
				else if (pCtrl->state == BS_CHECKBTN_NOCHECK)
					if (pCtrl->ButtonData.DownPicID)
					{
						hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.DownPicID));
						hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
						hai_DeleteObject((SH_GDIOBJ)hTemp);
						hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
						hai_DeleteObject((SH_GDIOBJ)hbitmap);
					}
					else
					{
						hTemp = hai_LoadBitmap(MAKEINTRESOURCE(IDI_RADIONOR));
						hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
						hai_DeleteObject((SH_GDIOBJ)hTemp);
						hai_DrawIcon(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y + ((pCtrl->h-CTRL_CHECKBTN_FLAG_W)>>1)), (SH_ICON)hbitmap);
						hai_DeleteObject((SH_GDIOBJ)hbitmap);
					}
			}
			if (pCtrl->ButtonData.pText)
				hai_CtrlDrawText(hCtrl, hDC);
			if (!pCtrl->grayed && pCtrl->focus)
				hai_DrawBoxByDashed(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y+2), (S_SHORT)(pCtrl->w-5), (S_SHORT)(pCtrl->h-5), COLOR_BLACK);
			hai_ReleaseDC(hDC);
			_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		}
		break;	
	case EVT_TIMER:
		break;	
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_CHAR:
		break;
	case EVT_KEYUP:
		if (wParam != ' ' && wParam != VK_RETURN)
			hai_SendEvent(EVT_KEYUP, wParam, lParam);
		break;
	case EVT_KEYDN:
		if (wParam != ' ' && wParam != VK_RETURN)
		{
			hai_SendEvent(EVT_KEYDN, wParam, lParam);
			break;
		}
	case EVT_MOUSEDN:
		_hai_CheckRadioButton(hCtrl, S_FALSE);
		_hai_SetFocusCtrl(hCtrl, S_FALSE);
		hai_RadioBtnCtrlProc(hCtrl, EVT_PAINT, 0, 0);
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(pCtrl->state,pCtrl->CtrlID), 0);
		break;
	case EVT_MOUSEUP:
		break;
	case EVT_MOUSEDB:
		break;
	case EVT_MOUSEMV:
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
	return 0;
}

S_DWORD hai_SelectBtnCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	S_SHORT x, y;

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		{
			SH_DC hDC;
			SH_BITMAP hTemp, hbitmap;

			if (!pCtrl->visible)
				break;
			hDC = hai_GetSceneDC();
			hai_SendEvent(EVT_CTRLCOLOR, (WPARAM)(pCtrl->CtrlID), (LPARAM)hDC);
			hai_BitBlt(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, S_NULL, 0, 0, PATCOPY);
			if (pCtrl->grayed)
			{
				if (pCtrl->ButtonData.GrayPicID)
				{
					hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.GrayPicID));
					hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
					hai_DeleteObject((SH_GDIOBJ)hTemp);
					hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
					hai_DeleteObject((SH_GDIOBJ)hbitmap);
				}
				else
					hai_Draw3DBlock(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, BLOCK_3D_UP, DKGRAY_BRUSH);
			}
			else
			{
				if (!pCtrl->focus)
				{
					pCtrl->state = BS_CHECKBTN_NOCHECK;
					if (pCtrl->ButtonData.UpPicID)
					{
						hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.UpPicID));
						hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
						hai_DeleteObject((SH_GDIOBJ)hTemp);
						hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
						hai_DeleteObject((SH_GDIOBJ)hbitmap);
					}
					else
						hai_Draw3DBlock(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, BLOCK_3D_UP, GRAY_BRUSH);
				}
				else
				{
					pCtrl->state = BS_CHECKBTN_CHECKED;
					if (pCtrl->ButtonData.DownPicID)
					{
						hTemp = hai_LoadBitmap((S_BYTE *)(pCtrl->ButtonData.DownPicID));
						hbitmap = hai_CreateFormatBitmap(hDC, hTemp);
						hai_DeleteObject((SH_GDIOBJ)hTemp);
						hai_DrawIcon(hDC, pCtrl->x, pCtrl->y, (SH_ICON)hbitmap);
						hai_DeleteObject((SH_GDIOBJ)hbitmap);
					}
					else
						hai_Draw3DBlock(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, BLOCK_3D_DN, GRAY_BRUSH);
				}
			}
			if (pCtrl->ButtonData.pText)
				hai_CtrlDrawText(hCtrl, hDC);
			if (!pCtrl->grayed && pCtrl->state == BS_CHECKBTN_CHECKED && pCtrl->focus)
				hai_DrawBoxByDashed(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y+2), (S_SHORT)(pCtrl->w-5), (S_SHORT)(pCtrl->h-5), COLOR_BLACK);
			hai_ReleaseDC(hDC);
			_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		}
		break;	
	case EVT_TIMER:
		break;
	case EVT_CHAR:
		break;
	case EVT_KEYUP:
		if (wParam == VK_RETURN)
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(EVT_MOUSEUP,pCtrl->CtrlID), 0);
		else
			hai_SendEvent(EVT_KEYUP, wParam, lParam);
		break;
	case EVT_KEYDN:
		if (wParam != VK_RETURN)
			hai_SendEvent(EVT_KEYDN, wParam, lParam);
		break;	
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_MOUSEDN:
		pCtrl->state = BS_CHECKBTN_CHECKED;
		_hai_SetFocusCtrl(hCtrl, S_FALSE);
		hai_SelectBtnCtrlProc(hCtrl, EVT_PAINT, 0, 0);
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(EVT_MOUSEDN,pCtrl->CtrlID), 0);
		break;
	case EVT_MOUSEUP:
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(EVT_MOUSEUP,pCtrl->CtrlID), 0);
		break;
	case EVT_MOUSEDB:
		break;
	case EVT_MOUSEMV:
		x = HAI_GETLOWORD(lParam);
		y = HAI_GETHIWORD(lParam);
		if (x>pCtrl->x && x<pCtrl->x+pCtrl->w && y>pCtrl->y && y<pCtrl->y+pCtrl->h)
		{
			if (pCtrl->state == BS_CHECKBTN_NOCHECK)
			{
				pCtrl->state = BS_CHECKBTN_CHECKED;
				_hai_SetFocusCtrl(hCtrl, S_FALSE);
				hai_SelectBtnCtrlProc(hCtrl, EVT_PAINT, 0, 0);
			}
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(EVT_MOUSEMV,pCtrl->CtrlID), 0);
		}
		else
		{
			((SP_CTRL)hCtrl)->state = BS_CHECKBTN_NOCHECK;
			hai_SelectBtnCtrlProc(hCtrl, EVT_PAINT, 0, 0);
		}
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
	return 0;
}

S_DWORD hai_ButtonCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	switch (((SP_CTRL)hCtrl)->style & BS_BTN_TYPE_MASK)
	{
	case BS_PUSHBTN:
		return hai_PushBtnCtrlProc(hCtrl, event, wParam, lParam);
	case BS_CHECKBTN:
		return hai_CheckBtnCtrlProc(hCtrl, event, wParam, lParam);
	case BS_RADIOBTN:
		return hai_RadioBtnCtrlProc(hCtrl, event, wParam, lParam);
	case BS_SELECTBTN:
		return hai_SelectBtnCtrlProc(hCtrl, event, wParam, lParam);
	default:
		return 0;
	}
}

S_BOOL hai_IsButtonChecked(SH_CTRL hCtrl)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	if (!hCtrl || pCtrl->CtrlType != CT_BUTTON)
		return S_FALSE;

	return (!!pCtrl->state);
}

S_BOOL hai_CheckButton(SH_CTRL hCtrl, S_BOOL checked)
{
	SP_CTRL  pCtrl = (SP_CTRL)hCtrl;

	if (!hCtrl || pCtrl->CtrlType != CT_BUTTON)
		return S_FALSE;
	if (pCtrl->state == checked)
		return S_FALSE;

	pCtrl->state = !!checked;
	pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_PAINT, 0, 0);

	return S_TRUE;
}

S_BOOL _hai_CheckRadioButton(SH_CTRL hCtrl, S_BOOL bshow)
{
	S_UINT   BtnType;
	SP_CTRL  pListHead;
	SP_CTRL  pCtrl = (SP_CTRL)hCtrl;

	if (!hCtrl || pCtrl->CtrlType != CT_BUTTON)
		return S_FALSE;
	BtnType = pCtrl->style&BS_BTN_TYPE_MASK;
	if (BtnType != BS_RADIOBTN && BtnType != BS_SELECTBTN)
		return S_FALSE;

	pListHead = hai_GetCurSceneCtrlListHead();
	if (!pListHead)
		return S_FALSE;

	while (pListHead)
	{
		if (pListHead->CtrlType == CT_BUTTON && (pListHead->style&BS_BTN_TYPE_MASK) == BtnType
			&& pListHead->group == pCtrl->group && pListHead->state == BS_CHECKBTN_CHECKED)
		{
			pListHead->state = BS_CHECKBTN_NOCHECK;
			pListHead->CtrlProc((SH_CTRL)pListHead, EVT_PAINT, 0, 0);
		}
		pListHead = pListHead->next;
	}
	pCtrl->state = BS_CHECKBTN_CHECKED;
	if (bshow)
	{
		pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_PAINT, 0, 0);	
//		hai_PostEvent(EVT_PAINT, 0, 0);	
	}
	return S_TRUE;
}

S_BOOL hai_CheckRadioButton(SH_CTRL hCtrl)
{
	return _hai_CheckRadioButton(hCtrl, S_TRUE);
}


//event area
//
S_BOOL hai_SetEvtAreaCtrlInfo(SH_CTRL hCtrl, S_DWORD EvtCode)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	if (!pCtrl || pCtrl->CtrlType != CT_EVTAREA)
		return S_FALSE;
	
	pCtrl->EvtAreaData.EvtCode = EvtCode;

	return S_TRUE;
}

SH_CTRL hai_CreateEvtArea(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_DWORD EvtCode)
{
	S_BOOL  ok;
	SH_CTRL hCtrl;

	hCtrl = hai_CreateCtrl(CT_EVTAREA, CtrlID, style, x, y, w, h);
	if (hCtrl)
		ok = hai_SetEvtAreaCtrlInfo(hCtrl, EvtCode);
	if (hCtrl && !ok)
	{
		_hai_DestroyCtrl(hCtrl, S_FALSE);
		hCtrl = S_NULL;
	}
	return hCtrl;
}

S_DWORD hai_EvtAreaCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		break;	
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_MOUSEDN:
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(EVT_MOUSEDN,pCtrl->CtrlID), pCtrl->EvtAreaData.EvtCode);
		break;
	case EVT_MOUSEUP:
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(EVT_MOUSEUP,pCtrl->CtrlID), pCtrl->EvtAreaData.EvtCode);
		break;
	case EVT_MOUSEDB:
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(EVT_MOUSEDB,pCtrl->CtrlID), pCtrl->EvtAreaData.EvtCode);
		break;
	case EVT_MOUSEMV:
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(EVT_MOUSEMV,pCtrl->CtrlID), pCtrl->EvtAreaData.EvtCode);
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
	return 0;
}

//list area
//
S_BOOL hai_SetListAreaCtrlInfo(SH_CTRL hCtrl, S_SHORT LineH)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	if (!pCtrl || pCtrl->CtrlType != CT_LISTAREA)
		return S_FALSE;

	pCtrl->ListAreaData.LineH = LineH;
	pCtrl->ListAreaData.TotalLine = pCtrl->h / LineH;

	return S_TRUE;
}

SH_CTRL hai_CreateListArea(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_SHORT LineH)
{
	S_BOOL  ok;
	SH_CTRL hCtrl;

	hCtrl = hai_CreateCtrl(CT_LISTAREA, CtrlID, style, x, y, w, h);
	if (hCtrl)
		ok = hai_SetListAreaCtrlInfo(hCtrl, LineH);
	if (hCtrl && !ok)
	{
		_hai_DestroyCtrl(hCtrl, S_FALSE);
		hCtrl = S_NULL;
	}
	return hCtrl;
}

S_DWORD hai_ListAreaCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	S_SHORT x, y;

	x = HAI_GETLOWORD(lParam);
	y = HAI_GETHIWORD(lParam);

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		break;	
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_MOUSEDN:
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(EVT_MOUSEDN,pCtrl->CtrlID), (y - pCtrl->y) / pCtrl->ListAreaData.LineH);
		break;
	case EVT_MOUSEUP:
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(EVT_MOUSEUP,pCtrl->CtrlID), (y - pCtrl->y) / pCtrl->ListAreaData.LineH);
		break;
	case EVT_MOUSEDB:
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(EVT_MOUSEDB,pCtrl->CtrlID), (y - pCtrl->y) / pCtrl->ListAreaData.LineH);
		break;
	case EVT_MOUSEMV:
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
	return 0;
}

//scroll bar
//
S_BOOL hai_SetScrlBarCtrlInfo(SH_CTRL hCtrl, S_DWORD TotalLine, S_DWORD CurStart, S_WORD LinePerPage)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	S_DWORD totallines;

	if (!pCtrl || pCtrl->CtrlType != CT_SCRLBAR)
		return S_FALSE;

	if (TotalLine <= LinePerPage)
		CurStart = 0;
	else
	{
		if (CurStart > TotalLine - LinePerPage)
			CurStart = 0;
	}
	if ((pCtrl->style&BS_SCRLBAR_VH_MASK) == BS_VSCRLBAR)
	{
		if (pCtrl->h < BS_SCRLBAR_WIDTH*3)
			pCtrl->h = BS_SCRLBAR_WIDTH*3;
		pCtrl->w = BS_SCRLBAR_WIDTH;
	}
	else
	{
		if (pCtrl->w < BS_SCRLBAR_WIDTH*3)
			pCtrl->w = BS_SCRLBAR_WIDTH*3;
		pCtrl->h = BS_SCRLBAR_WIDTH;
	}
	pCtrl->ScrlBarData.TotalLine = TotalLine;
	pCtrl->ScrlBarData.CurStart  = CurStart;
	pCtrl->ScrlBarData.LinePerPage = LinePerPage;

	totallines = pCtrl->ScrlBarData.TotalLine ? pCtrl->ScrlBarData.TotalLine : 1;
	if ((pCtrl->style&BS_SCRLBAR_VH_MASK) == BS_VSCRLBAR)
		pCtrl->ScrlBarData.ThumbW = pCtrl->h - BS_SCRLBAR_WIDTH*2;
	else
		pCtrl->ScrlBarData.ThumbW = pCtrl->w - BS_SCRLBAR_WIDTH*2;

	if(pCtrl->ScrlBarData.LinePerPage && pCtrl->ScrlBarData.LinePerPage < totallines)
	{
		pCtrl->ScrlBarData.ThumbW = (S_WORD)(pCtrl->ScrlBarData.ThumbW * pCtrl->ScrlBarData.LinePerPage / totallines);
		if( pCtrl->ScrlBarData.ThumbW < 8 ) 
			pCtrl->ScrlBarData.ThumbW = 8;
	}

	pCtrl->ScrlBarData.ThumbPos = 0;
	if ((pCtrl->style&BS_SCRLBAR_VH_MASK) == BS_VSCRLBAR)
	{
		if(pCtrl->ScrlBarData.CurStart)
		{
			pCtrl->ScrlBarData.ThumbPos = (S_WORD)((pCtrl->h-BS_SCRLBAR_WIDTH*2)*(pCtrl->ScrlBarData.CurStart+pCtrl->ScrlBarData.LinePerPage)/totallines-pCtrl->ScrlBarData.ThumbW);
			if( pCtrl->ScrlBarData.ThumbPos < 0 )
				pCtrl->ScrlBarData.ThumbPos = 0;
		}
		if(pCtrl->ScrlBarData.ThumbPos + pCtrl->ScrlBarData.ThumbW > pCtrl->h-BS_SCRLBAR_WIDTH*2)
			pCtrl->ScrlBarData.ThumbPos = pCtrl->h-BS_SCRLBAR_WIDTH*2-pCtrl->ScrlBarData.ThumbW;
	}
	else
	{
		if(pCtrl->ScrlBarData.CurStart)
		{
			pCtrl->ScrlBarData.ThumbPos = (S_WORD)((pCtrl->w-BS_SCRLBAR_WIDTH*2)*(pCtrl->ScrlBarData.CurStart+pCtrl->ScrlBarData.LinePerPage)/totallines-pCtrl->ScrlBarData.ThumbW);
			if( pCtrl->ScrlBarData.ThumbPos < 0 )
				pCtrl->ScrlBarData.ThumbPos = 0;
		}
		if(pCtrl->ScrlBarData.ThumbPos + pCtrl->ScrlBarData.ThumbW > pCtrl->w-BS_SCRLBAR_WIDTH*2)
			pCtrl->ScrlBarData.ThumbPos = pCtrl->w-BS_SCRLBAR_WIDTH*2-pCtrl->ScrlBarData.ThumbW;
	}

	return S_TRUE;
}

SH_CTRL hai_CreateScrlBar(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_DWORD TotalLine, S_DWORD CurStart, S_WORD LinePerPage)
{
	S_BOOL  ok;
	SH_CTRL hCtrl;

	hCtrl = hai_CreateCtrl(CT_SCRLBAR, CtrlID, style, x, y, w, h);
	if (hCtrl)
		ok = hai_SetScrlBarCtrlInfo(hCtrl, TotalLine, CurStart, LinePerPage);
	if (hCtrl && !ok)
	{
		_hai_DestroyCtrl(hCtrl, S_FALSE);
		hCtrl = S_NULL;
	}
	return hCtrl;
}

S_BOOL hai_GetScrlBarInfo(SH_CTRL hCtrl, S_DWORD *CurStart, S_DWORD *TotalLines, S_WORD *LinesPerPage)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	if (!pCtrl || pCtrl->CtrlType != CT_SCRLBAR)
		return S_FALSE;

	if (CurStart) *CurStart = pCtrl->ScrlBarData.CurStart;
	if (TotalLines) *TotalLines = pCtrl->ScrlBarData.TotalLine;
	if (LinesPerPage) *LinesPerPage = pCtrl->ScrlBarData.LinePerPage;

	return S_TRUE;
}

S_BOOL hai_SetScrlBarInfo(SH_CTRL hCtrl, S_DWORD CurStart, S_DWORD TotalLines, S_WORD LinesPerPage)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	S_DWORD totallines;

	if (!pCtrl || pCtrl->CtrlType != CT_SCRLBAR)
		return S_FALSE;

	if (TotalLines <= LinesPerPage)
		CurStart = 0;
	else
	{
		if (CurStart > TotalLines - LinesPerPage)
			CurStart = 0;
	}
//	if (pCtrl->ScrlBarData.TotalLine == TotalLines && pCtrl->ScrlBarData.CurStart == CurStart
//				&& pCtrl->ScrlBarData.LinePerPage == LinesPerPage)
//		return S_FALSE;


	pCtrl->ScrlBarData.TotalLine = TotalLines;
	pCtrl->ScrlBarData.CurStart  = CurStart;
	pCtrl->ScrlBarData.LinePerPage = LinesPerPage;

	totallines = pCtrl->ScrlBarData.TotalLine ? pCtrl->ScrlBarData.TotalLine : 1;
	if ((pCtrl->style&BS_SCRLBAR_VH_MASK) == BS_VSCRLBAR)
		pCtrl->ScrlBarData.ThumbW = pCtrl->h - BS_SCRLBAR_WIDTH*2;
	else
		pCtrl->ScrlBarData.ThumbW = pCtrl->w - BS_SCRLBAR_WIDTH*2;

	if(pCtrl->ScrlBarData.LinePerPage && pCtrl->ScrlBarData.LinePerPage < totallines)
	{
		pCtrl->ScrlBarData.ThumbW = (S_WORD)(pCtrl->ScrlBarData.ThumbW * pCtrl->ScrlBarData.LinePerPage / totallines);
		if( pCtrl->ScrlBarData.ThumbW < 8 ) 
			pCtrl->ScrlBarData.ThumbW = 8;
	}

	pCtrl->ScrlBarData.ThumbPos = 0;
	if ((pCtrl->style&BS_SCRLBAR_VH_MASK) == BS_VSCRLBAR)
	{
		if(pCtrl->ScrlBarData.CurStart)
		{
			pCtrl->ScrlBarData.ThumbPos = (S_WORD)((pCtrl->h-BS_SCRLBAR_WIDTH*2)*(pCtrl->ScrlBarData.CurStart+pCtrl->ScrlBarData.LinePerPage)/totallines-pCtrl->ScrlBarData.ThumbW);
			if( pCtrl->ScrlBarData.ThumbPos < 0 )
				pCtrl->ScrlBarData.ThumbPos = 0;
		}
		if(pCtrl->ScrlBarData.ThumbPos + pCtrl->ScrlBarData.ThumbW > pCtrl->h-BS_SCRLBAR_WIDTH*2)
			pCtrl->ScrlBarData.ThumbPos = pCtrl->h-BS_SCRLBAR_WIDTH*2-pCtrl->ScrlBarData.ThumbW;
	}
	else
	{
		if(pCtrl->ScrlBarData.CurStart)
		{
			pCtrl->ScrlBarData.ThumbPos = (S_WORD)((pCtrl->w-BS_SCRLBAR_WIDTH*2)*(pCtrl->ScrlBarData.CurStart+pCtrl->ScrlBarData.LinePerPage)/totallines-pCtrl->ScrlBarData.ThumbW);
			if( pCtrl->ScrlBarData.ThumbPos < 0 )
				pCtrl->ScrlBarData.ThumbPos = 0;
		}
		if(pCtrl->ScrlBarData.ThumbPos + pCtrl->ScrlBarData.ThumbW > pCtrl->w-BS_SCRLBAR_WIDTH*2)
			pCtrl->ScrlBarData.ThumbPos = pCtrl->w-BS_SCRLBAR_WIDTH*2-pCtrl->ScrlBarData.ThumbW;
	}

	pCtrl->CtrlProc(hCtrl, EVT_RESTORE, 0, 0);

	return S_TRUE;
}

S_DWORD hai_VScrlBarCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	S_SHORT x, y, i;
	SH_DC hDC;
	SH_BRUSH hBrush;
	S_DWORD PreStart;

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		if (!pCtrl->visible)
			break;
		hDC = hai_GetSceneDC();
		x = pCtrl->x; 	y = pCtrl->y;
		hai_Draw3DBlock(hDC, x, y, BS_SCRLBAR_WIDTH, BS_SCRLBAR_WIDTH, BLOCK_3D_UP, GRAY_BRUSH);
		y += pCtrl->h - BS_SCRLBAR_WIDTH;
		hai_Draw3DBlock(hDC, x, y, BS_SCRLBAR_WIDTH, BS_SCRLBAR_WIDTH, BLOCK_3D_UP, GRAY_BRUSH);
		x = pCtrl->x + BS_SCRLBAR_WIDTH/2;
		y = pCtrl->y + (BS_SCRLBAR_WIDTH - (BS_SCRLBAR_WIDTH+3)/4)/2-1;
		for(i = 0 ; i < (BS_SCRLBAR_WIDTH + 3)/4+1 ; ++i)
			_hai_DrawHLine(hDC, (S_SHORT)(x-i), (S_SHORT)(x+i), y++, COLOR_BLACK);
		y = pCtrl->y+pCtrl->h-1-(BS_SCRLBAR_WIDTH-(BS_SCRLBAR_WIDTH+3)/4)/2+1;
		for(i=0 ; i < (BS_SCRLBAR_WIDTH+3)/4+1; ++i)
			_hai_DrawHLine(hDC, (S_SHORT)(x-i), (S_SHORT)(x+i), y--, COLOR_BLACK);
		hBrush = (SH_BRUSH)hai_GetStockObject(LTGRAY_BRUSH);
		hBrush = (SH_BRUSH)hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
		hai_BitBlt(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+BS_SCRLBAR_WIDTH), BS_SCRLBAR_WIDTH, (S_SHORT)(pCtrl->h-BS_SCRLBAR_WIDTH*2), S_NULL, 0, 0, PATCOPY);
		hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
		hai_Draw3DBlock(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+pCtrl->ScrlBarData.ThumbPos+BS_SCRLBAR_WIDTH), BS_SCRLBAR_WIDTH, pCtrl->ScrlBarData.ThumbW, BLOCK_3D_UP, GRAY_BRUSH);
		if (pCtrl->focus)
			hai_DrawBoxByDashed(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y+pCtrl->ScrlBarData.ThumbPos+BS_SCRLBAR_WIDTH+2), (S_SHORT)(BS_SCRLBAR_WIDTH-5), (S_SHORT)(pCtrl->ScrlBarData.ThumbW-5), COLOR_BLACK);
		hai_ReleaseDC(hDC);
		_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		switch (wParam)
		{
		case VK_PRIOR:
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_PAGEUP,pCtrl->CtrlID), 0);
			break;
		case VK_NEXT:
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_PAGEDOWN,pCtrl->CtrlID), 0);
			break;
		case VK_END:
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_BOTTOM,pCtrl->CtrlID), 0);
			break;
		case VK_HOME:
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_TOP,pCtrl->CtrlID), 0);
			break;
		case VK_UP:
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_LINEUP,pCtrl->CtrlID), 0);
			break;
		case VK_DOWN:
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_LINEDOWN,pCtrl->CtrlID), 0);
			break;
		default:
			hai_SendEvent(EVT_KEYDN, wParam, lParam);
			break;
		}
		break;	
	case EVT_KEYUP:
		switch (wParam)
		{
		case VK_PRIOR:
		case VK_NEXT:
		case VK_END:
		case VK_HOME:
		case VK_UP:
		case VK_DOWN:
			break;
		default:
			hai_SendEvent(EVT_KEYUP, wParam, lParam);
			break;
		}
		break;	
	case EVT_CHAR:
		break;
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_MOUSEDN:
		hai_SetCapture(hCtrl);
		x = HAI_GETLOWORD(lParam);
		y = HAI_GETHIWORD(lParam);
		hDC = hai_GetSceneDC();
		if (y < pCtrl->y+BS_SCRLBAR_WIDTH)
		{
			pCtrl->ScrlBarData.ClickArea = 1;
			hai_Draw3DBlock(hDC, pCtrl->x, pCtrl->y, BS_SCRLBAR_WIDTH, BS_SCRLBAR_WIDTH, BLOCK_3D_DN, GRAY_BRUSH);
			x = pCtrl->x + BS_SCRLBAR_WIDTH/2;
			y = pCtrl->y + (BS_SCRLBAR_WIDTH - (BS_SCRLBAR_WIDTH+3)/4)/2-1;
			for(i = 0 ; i < (BS_SCRLBAR_WIDTH + 3)/4+1 ; ++i)
				_hai_DrawHLine(hDC, (S_SHORT)(x-i), (S_SHORT)(x+i), y++, COLOR_BLACK);
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_LINEUP, pCtrl->CtrlID), 0);
		}
		else if (y < pCtrl->y+BS_SCRLBAR_WIDTH+pCtrl->ScrlBarData.ThumbPos)
		{
			pCtrl->ScrlBarData.ClickArea = 2;
			hBrush = (SH_BRUSH)hai_GetStockObject(GRAY_BRUSH);
			hBrush = (SH_BRUSH)hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			hai_BitBlt(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+BS_SCRLBAR_WIDTH), BS_SCRLBAR_WIDTH, (S_SHORT)(pCtrl->ScrlBarData.ThumbPos), S_NULL, 0, 0, PATCOPY);
			hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_PAGEUP, pCtrl->CtrlID), 0);
		}
		else if (y < pCtrl->y+BS_SCRLBAR_WIDTH+pCtrl->ScrlBarData.ThumbPos+pCtrl->ScrlBarData.ThumbW)
		{
			pCtrl->ScrlBarData.ClickArea = 3;
			pCtrl->ScrlBarData.PriorMouseY = y;
		}
		else if (y < pCtrl->y+pCtrl->h-BS_SCRLBAR_WIDTH)
		{
			pCtrl->ScrlBarData.ClickArea = 4;
			hBrush = (SH_BRUSH)hai_GetStockObject(GRAY_BRUSH);
			hBrush = (SH_BRUSH)hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			hai_BitBlt(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+BS_SCRLBAR_WIDTH+pCtrl->ScrlBarData.ThumbPos+pCtrl->ScrlBarData.ThumbW), BS_SCRLBAR_WIDTH, (S_SHORT)(pCtrl->h-pCtrl->ScrlBarData.ThumbPos-pCtrl->ScrlBarData.ThumbW-BS_SCRLBAR_WIDTH*2), S_NULL, 0, 0, PATCOPY);
			hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_PAGEDOWN, pCtrl->CtrlID), 0);
		}
		else
		{
			pCtrl->ScrlBarData.ClickArea = 5;
			y = pCtrl->y + pCtrl->h - BS_SCRLBAR_WIDTH;
			hai_Draw3DBlock(hDC, pCtrl->x, y, BS_SCRLBAR_WIDTH, BS_SCRLBAR_WIDTH, BLOCK_3D_DN, GRAY_BRUSH);
			x = pCtrl->x + BS_SCRLBAR_WIDTH/2;
			y = pCtrl->y+pCtrl->h-1-(BS_SCRLBAR_WIDTH-(BS_SCRLBAR_WIDTH+3)/4)/2+1;
			for(i=0 ; i < (BS_SCRLBAR_WIDTH+3)/4+1; ++i)
				_hai_DrawHLine(hDC, (S_SHORT)(x-i), (S_SHORT)(x+i), y--, COLOR_BLACK);
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_LINEDOWN, pCtrl->CtrlID), 0);
		}
		hai_ReleaseDC(hDC);
		_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		break;
	case EVT_MOUSEUP:
		x = HAI_GETLOWORD(lParam);
		y = HAI_GETHIWORD(lParam);
		hDC = hai_GetSceneDC();
		if (pCtrl->ScrlBarData.ClickArea == 1)//(y < pCtrl->y+BS_SCRLBAR_WIDTH)
		{
			hai_Draw3DBlock(hDC, pCtrl->x, pCtrl->y, BS_SCRLBAR_WIDTH, BS_SCRLBAR_WIDTH, BLOCK_3D_UP, GRAY_BRUSH);
			x = pCtrl->x + BS_SCRLBAR_WIDTH/2;
			y = pCtrl->y + (BS_SCRLBAR_WIDTH - (BS_SCRLBAR_WIDTH+3)/4)/2-1;
			for(i = 0 ; i < (BS_SCRLBAR_WIDTH + 3)/4+1 ; ++i)
				_hai_DrawHLine(hDC, (S_SHORT)(x-i), (S_SHORT)(x+i), y++, COLOR_BLACK);
		}
		else if (pCtrl->ScrlBarData.ClickArea == 2)//(y < pCtrl->y+BS_SCRLBAR_WIDTH+pCtrl->ScrlBarData.ThumbPos)
		{
			hBrush = (SH_BRUSH)hai_GetStockObject(LTGRAY_BRUSH);
			hBrush = (SH_BRUSH)hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			if (pCtrl->ScrlBarData.ThumbPos)
				hai_BitBlt(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+BS_SCRLBAR_WIDTH), BS_SCRLBAR_WIDTH, (S_SHORT)(pCtrl->ScrlBarData.ThumbPos), S_NULL, 0, 0, PATCOPY);
			hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
		}
		else if (pCtrl->ScrlBarData.ClickArea == 3)//(y < pCtrl->y+BS_SCRLBAR_WIDTH+pCtrl->ScrlBarData.ThumbPos+pCtrl->ScrlBarData.ThumbW)
		{
			PreStart = 0;
			if (pCtrl->ScrlBarData.TotalLine && pCtrl->ScrlBarData.TotalLine > pCtrl->ScrlBarData.LinePerPage)
			{
				PreStart = (pCtrl->ScrlBarData.ThumbPos + pCtrl->ScrlBarData.ThumbW) * pCtrl->ScrlBarData.TotalLine;
				PreStart = PreStart / (pCtrl->h - BS_SCRLBAR_WIDTH*2); 
				if (PreStart > pCtrl->ScrlBarData.LinePerPage)
					PreStart = PreStart - pCtrl->ScrlBarData.LinePerPage;
				else
					PreStart = 0;
			}
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_THUMBPOSITION, pCtrl->CtrlID), PreStart);
			hai_SetScrlBarInfo(hCtrl, pCtrl->ScrlBarData.CurStart, pCtrl->ScrlBarData.TotalLine, pCtrl->ScrlBarData.LinePerPage);
			pCtrl->ScrlBarData.PriorMouseY = 0;
		}
		else if (pCtrl->ScrlBarData.ClickArea == 4)//(y < pCtrl->y+pCtrl->h-BS_SCRLBAR_WIDTH)
		{
			hBrush = (SH_BRUSH)hai_GetStockObject(LTGRAY_BRUSH);
			hBrush = (SH_BRUSH)hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			if (pCtrl->h-pCtrl->ScrlBarData.ThumbPos-pCtrl->ScrlBarData.ThumbW-BS_SCRLBAR_WIDTH*2)
				hai_BitBlt(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+BS_SCRLBAR_WIDTH+pCtrl->ScrlBarData.ThumbPos+pCtrl->ScrlBarData.ThumbW), BS_SCRLBAR_WIDTH, (S_SHORT)(pCtrl->h-pCtrl->ScrlBarData.ThumbPos-pCtrl->ScrlBarData.ThumbW-BS_SCRLBAR_WIDTH*2), S_NULL, 0, 0, PATCOPY);
			hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
		}
		else if (pCtrl->ScrlBarData.ClickArea == 5)
		{
			y = pCtrl->y + pCtrl->h - BS_SCRLBAR_WIDTH;
			hai_Draw3DBlock(hDC, pCtrl->x, y, BS_SCRLBAR_WIDTH, BS_SCRLBAR_WIDTH, BLOCK_3D_UP, GRAY_BRUSH);
			x = pCtrl->x + BS_SCRLBAR_WIDTH/2;
			y = pCtrl->y+pCtrl->h-1-(BS_SCRLBAR_WIDTH-(BS_SCRLBAR_WIDTH+3)/4)/2+1;
			for(i=0 ; i < (BS_SCRLBAR_WIDTH+3)/4+1; ++i)
				_hai_DrawHLine(hDC, (S_SHORT)(x-i), (S_SHORT)(x+i), y--, COLOR_BLACK);
		}
		hai_ReleaseDC(hDC);
		pCtrl->ScrlBarData.ClickArea = 0;
		_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		hai_ReleaseCapture();
		break;
	case EVT_MOUSEDB:
		hai_ReleaseCapture();
		break;
	case EVT_MOUSEMV:
		x = HAI_GETLOWORD(lParam);
		y = HAI_GETHIWORD(lParam);
		hDC = hai_GetSceneDC();
		if (x>pCtrl->x && x<pCtrl->x+pCtrl->w && y>pCtrl->y && y<pCtrl->y+pCtrl->h)
		{
			if (pCtrl->ScrlBarData.ClickArea == 0)
			{
			}
			else
			{
				if (y < pCtrl->y+BS_SCRLBAR_WIDTH)
				{
					if (pCtrl->ScrlBarData.ClickArea != 1)
					{
						hai_VScrlBarCtrlProc(hCtrl, EVT_MOUSEUP, wParam, lParam);
					}
				}
				else if (y < pCtrl->y+BS_SCRLBAR_WIDTH+pCtrl->ScrlBarData.ThumbPos)
				{
					if (pCtrl->ScrlBarData.ClickArea != 2)
					{
						hai_VScrlBarCtrlProc(hCtrl, EVT_MOUSEUP, wParam, lParam);
					}
				}
				else if (y < pCtrl->y+BS_SCRLBAR_WIDTH+pCtrl->ScrlBarData.ThumbPos+pCtrl->ScrlBarData.ThumbW)
				{
					if (pCtrl->ScrlBarData.ClickArea != 3)
					{
						hai_VScrlBarCtrlProc(hCtrl, EVT_MOUSEUP, wParam, lParam);
					}
					else
//					if (pCtrl->ScrlBarData.ClickArea == 3)
					{
						pCtrl->ScrlBarData.PriorMouseY -= y;
						if (pCtrl->ScrlBarData.PriorMouseY && pCtrl->ScrlBarData.TotalLine > pCtrl->ScrlBarData.LinePerPage)
						{
							if (pCtrl->ScrlBarData.PriorMouseY < 0 && pCtrl->ScrlBarData.ThumbPos+pCtrl->ScrlBarData.ThumbW < pCtrl->h-BS_SCRLBAR_WIDTH*2)
							{//the thumb down
								pCtrl->ScrlBarData.ThumbPos += -pCtrl->ScrlBarData.PriorMouseY;
								if (pCtrl->ScrlBarData.ThumbPos+pCtrl->ScrlBarData.ThumbW > pCtrl->h-BS_SCRLBAR_WIDTH*2)
									pCtrl->ScrlBarData.ThumbPos = (pCtrl->h-BS_SCRLBAR_WIDTH*2)-pCtrl->ScrlBarData.ThumbW;
							}
							else if (pCtrl->ScrlBarData.PriorMouseY > 0 && pCtrl->ScrlBarData.ThumbPos > 0)
							{//the thumb up
								if (pCtrl->ScrlBarData.ThumbPos > pCtrl->ScrlBarData.PriorMouseY)
									pCtrl->ScrlBarData.ThumbPos -= pCtrl->ScrlBarData.PriorMouseY;
								else
									pCtrl->ScrlBarData.ThumbPos = 0;
							}
							hai_VScrlBarCtrlProc(hCtrl, EVT_RESTORE, 0, 0);
							PreStart = 0;
							if (pCtrl->ScrlBarData.TotalLine && pCtrl->ScrlBarData.TotalLine > pCtrl->ScrlBarData.LinePerPage)
							{
								PreStart = (pCtrl->ScrlBarData.ThumbPos + pCtrl->ScrlBarData.ThumbW) * pCtrl->ScrlBarData.TotalLine;
								PreStart = PreStart / (pCtrl->h - BS_SCRLBAR_WIDTH*2); 
								if (PreStart > pCtrl->ScrlBarData.LinePerPage)
									PreStart = PreStart - pCtrl->ScrlBarData.LinePerPage;
								else
									PreStart = 0;
								if (pCtrl->ScrlBarData.PriorMouseY > 0)//the thumb up
								{
									PreStart += 1;
									if (pCtrl->ScrlBarData.ThumbPos == 0)
										PreStart = 0;
								}
							}
							if (PreStart != pCtrl->ScrlBarData.CurStart)
								hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_THUMBTRACK, pCtrl->CtrlID), PreStart);
						}
						pCtrl->ScrlBarData.PriorMouseY = y;
					}
				}
				else if (y < pCtrl->y+pCtrl->h-BS_SCRLBAR_WIDTH)
				{
					if (pCtrl->ScrlBarData.ClickArea != 4)
					{
						hai_VScrlBarCtrlProc(hCtrl, EVT_MOUSEUP, wParam, lParam);
					}
				}
				else
				{
					if (pCtrl->ScrlBarData.ClickArea != 5)
					{
						hai_VScrlBarCtrlProc(hCtrl, EVT_MOUSEUP, wParam, lParam);
					}
				}
			}
			_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		}
		else
		{
			if (pCtrl->ScrlBarData.ClickArea == 3)
			{
				pCtrl->ScrlBarData.PriorMouseY -= y;
				if (pCtrl->ScrlBarData.PriorMouseY && pCtrl->ScrlBarData.TotalLine > pCtrl->ScrlBarData.LinePerPage)
				{
					if (pCtrl->ScrlBarData.PriorMouseY < 0 && pCtrl->ScrlBarData.ThumbPos+pCtrl->ScrlBarData.ThumbW < pCtrl->h-BS_SCRLBAR_WIDTH*2)
					{
						pCtrl->ScrlBarData.ThumbPos += -pCtrl->ScrlBarData.PriorMouseY;
						if (pCtrl->ScrlBarData.ThumbPos+pCtrl->ScrlBarData.ThumbW > pCtrl->h-BS_SCRLBAR_WIDTH*2)
							pCtrl->ScrlBarData.ThumbPos = (pCtrl->h-BS_SCRLBAR_WIDTH*2)-pCtrl->ScrlBarData.ThumbW;
					}
					else if (pCtrl->ScrlBarData.PriorMouseY > 0 && pCtrl->ScrlBarData.ThumbPos > 0)
					{
						if (pCtrl->ScrlBarData.ThumbPos > pCtrl->ScrlBarData.PriorMouseY)
							pCtrl->ScrlBarData.ThumbPos -= pCtrl->ScrlBarData.PriorMouseY;
						else
							pCtrl->ScrlBarData.ThumbPos = 0;
					}
					hai_VScrlBarCtrlProc(hCtrl, EVT_RESTORE, 0, 0);
					PreStart = 0;
					if (pCtrl->ScrlBarData.TotalLine && pCtrl->ScrlBarData.TotalLine > pCtrl->ScrlBarData.LinePerPage)
					{
						PreStart = (pCtrl->ScrlBarData.ThumbPos + pCtrl->ScrlBarData.ThumbW) * pCtrl->ScrlBarData.TotalLine;
						PreStart = PreStart / (pCtrl->h - BS_SCRLBAR_WIDTH*2); 
						if (PreStart > pCtrl->ScrlBarData.LinePerPage)
							PreStart = PreStart - pCtrl->ScrlBarData.LinePerPage;
						else
							PreStart = 0;
						if (pCtrl->ScrlBarData.PriorMouseY > 0)//the thumb up
						{
							PreStart += 1;
							if (pCtrl->ScrlBarData.ThumbPos == 0)
								PreStart = 0;
						}
					}
					if (PreStart != pCtrl->ScrlBarData.CurStart)
						hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_THUMBTRACK, pCtrl->CtrlID), PreStart);
				}
				pCtrl->ScrlBarData.PriorMouseY = y;
			}
			else
				hai_VScrlBarCtrlProc(hCtrl, EVT_MOUSEUP, 0, 0);
		}
		hai_ReleaseDC(hDC);
		break;
	case EVT_RESTORE:
		{
			SH_DC hDC;
			SH_BRUSH hBrush;

			if (!pCtrl->visible)
				break;
			hDC = hai_GetSceneDC();
			hBrush = (SH_BRUSH)hai_GetStockObject(LTGRAY_BRUSH);
			hBrush = (SH_BRUSH)hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			hai_BitBlt(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+BS_SCRLBAR_WIDTH), BS_SCRLBAR_WIDTH, (S_SHORT)(pCtrl->h-BS_SCRLBAR_WIDTH*2), S_NULL, 0, 0, PATCOPY);
			hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			hai_Draw3DBlock(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+pCtrl->ScrlBarData.ThumbPos+BS_SCRLBAR_WIDTH), BS_SCRLBAR_WIDTH, pCtrl->ScrlBarData.ThumbW, BLOCK_3D_UP, GRAY_BRUSH);
			if (pCtrl->focus)
				hai_DrawBoxByDashed(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y+pCtrl->ScrlBarData.ThumbPos+BS_SCRLBAR_WIDTH+2), (S_SHORT)(BS_SCRLBAR_WIDTH-5), (S_SHORT)(pCtrl->ScrlBarData.ThumbW-5), COLOR_BLACK);
			hai_ReleaseDC(hDC);
			_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		}
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
	return 0;
}

S_DWORD hai_HScrlBarCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	S_SHORT x, y, i;

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		{
			SH_DC hDC;
			SH_BRUSH hBrush;

			if (!pCtrl->visible)
				break;
			hDC = hai_GetSceneDC();
			x = pCtrl->x;
			y = pCtrl->y;
			hai_Draw3DBlock(hDC, x, y, BS_SCRLBAR_WIDTH, BS_SCRLBAR_WIDTH, BLOCK_3D_UP, GRAY_BRUSH);
			x += pCtrl->w - BS_SCRLBAR_WIDTH;
			hai_Draw3DBlock(hDC, x, y, BS_SCRLBAR_WIDTH, BS_SCRLBAR_WIDTH, BLOCK_3D_UP, GRAY_BRUSH);
			y = pCtrl->y + BS_SCRLBAR_WIDTH/2;
			x = pCtrl->x+(BS_SCRLBAR_WIDTH-(BS_SCRLBAR_WIDTH+3)/4)/2-1;
			for(i=0 ; i < (BS_SCRLBAR_WIDTH+3)/4+1; ++i)
				_hai_DrawVLine(hDC, x++, (S_SHORT)(y-i), (S_SHORT)(y+i), COLOR_BLACK);
			x = pCtrl->x+pCtrl->w-1-(BS_SCRLBAR_WIDTH-(BS_SCRLBAR_WIDTH+3)/4)/2+1;
			for(i=0 ; i < (BS_SCRLBAR_WIDTH+3)/4+1; ++i)
				_hai_DrawVLine(hDC, x--, (S_SHORT)(y-i), (S_SHORT)(y+i), COLOR_BLACK);
			hBrush = (SH_BRUSH)hai_GetStockObject(LTGRAY_BRUSH);
			hBrush = (SH_BRUSH)hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			hai_BitBlt(hDC, (S_SHORT)(pCtrl->x+BS_SCRLBAR_WIDTH), pCtrl->y, (S_SHORT)(pCtrl->w-BS_SCRLBAR_WIDTH*2), BS_SCRLBAR_WIDTH, S_NULL, 0, 0, PATCOPY);
			hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			hai_Draw3DBlock(hDC, (S_SHORT)(pCtrl->x+pCtrl->ScrlBarData.ThumbPos+BS_SCRLBAR_WIDTH), pCtrl->y, pCtrl->ScrlBarData.ThumbW, BS_SCRLBAR_WIDTH, BLOCK_3D_UP, GRAY_BRUSH);
			if (pCtrl->focus)
				hai_DrawBoxByDashed(hDC, (S_SHORT)(pCtrl->x+pCtrl->ScrlBarData.ThumbPos+BS_SCRLBAR_WIDTH+2), (S_SHORT)(pCtrl->y+2), (S_SHORT)(pCtrl->ScrlBarData.ThumbW-5), BS_SCRLBAR_WIDTH-5, COLOR_BLACK);
			hai_ReleaseDC(hDC);
			_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		}
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		switch (wParam)
		{
		case VK_END:
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_BOTTOM,pCtrl->CtrlID), 0);
			break;
		case VK_HOME:
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_TOP,pCtrl->CtrlID), 0);
			break;
		case VK_LEFT:
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_LINEUP,pCtrl->CtrlID), 0);
			break;
		case VK_RIGHT:
			hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(SB_LINEDOWN,pCtrl->CtrlID), 0);
			break;
		default:
			hai_SendEvent(EVT_KEYDN, wParam, lParam);
			break;
		}
		break;	
	case EVT_KEYUP:
		switch (wParam)
		{
		case VK_END:
		case VK_HOME:
		case VK_LEFT:
		case VK_RIGHT:
			break;
		default:
			hai_SendEvent(EVT_KEYUP, wParam, lParam);
			break;
		}
		break;
	case EVT_CHAR:
		break;
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_MOUSEDN:
		break;
	case EVT_MOUSEUP:
		break;
	case EVT_MOUSEDB:
		break;
	case EVT_MOUSEMV:
		break;
	case EVT_RESTORE:
		{
			SH_DC hDC;
			SH_BRUSH hBrush;

			if (!pCtrl->visible)
				break;
			hDC = hai_GetSceneDC();
			hBrush = (SH_BRUSH)hai_GetStockObject(LTGRAY_BRUSH);
			hBrush = (SH_BRUSH)hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			hai_BitBlt(hDC, (S_SHORT)(pCtrl->x+BS_SCRLBAR_WIDTH), pCtrl->y, (S_SHORT)(pCtrl->w-BS_SCRLBAR_WIDTH*2), BS_SCRLBAR_WIDTH, S_NULL, 0, 0, PATCOPY);
			hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			hai_Draw3DBlock(hDC, (S_SHORT)(pCtrl->x+pCtrl->ScrlBarData.ThumbPos+BS_SCRLBAR_WIDTH), pCtrl->y, pCtrl->ScrlBarData.ThumbW, BS_SCRLBAR_WIDTH, BLOCK_3D_UP, GRAY_BRUSH);
			if (pCtrl->focus)
				hai_DrawBoxByDashed(hDC, (S_SHORT)(pCtrl->x+pCtrl->ScrlBarData.ThumbPos+BS_SCRLBAR_WIDTH+2), (S_SHORT)(pCtrl->y+2), (S_SHORT)(pCtrl->ScrlBarData.ThumbW-5), (S_SHORT)(BS_SCRLBAR_WIDTH-5), COLOR_BLACK);
			hai_ReleaseDC(hDC);
			_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		}
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
	return 0;
}

S_DWORD hai_ScrlBarCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	if ((((SP_CTRL)hCtrl)->style&BS_SCRLBAR_VH_MASK) == BS_VSCRLBAR)
		return hai_VScrlBarCtrlProc(hCtrl, event, wParam, lParam);
	else
		return hai_HScrlBarCtrlProc(hCtrl, event, wParam, lParam);
}


//static
//
S_BOOL hai_SetStaticCtrlInfo(SH_CTRL hCtrl, S_BYTE *pText, S_BYTE *PicID)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	if (!pCtrl || pCtrl->CtrlType != CT_STATIC)
		return S_FALSE;

	pCtrl->grayed   = !!CS_GRAYED;

	if (pCtrl->StaticData.pText)
		hai_MemFree(pCtrl->StaticData.pText);

	pCtrl->StaticData.pText = pText;
	if (pText)
	{
		pCtrl->StaticData.pText = hai_MemAlloc(hai_strlen(pText)+1);
		if (!pCtrl->StaticData.pText)
			return S_FALSE;
		hai_strcpy(pCtrl->StaticData.pText, pText);
	}
	pCtrl->StaticData.PicID = PicID;
	
	return S_TRUE;
}

SH_CTRL hai_CreateStatic(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BYTE *pText, S_BYTE *PicID)
{
	S_BOOL  ok;
	SH_CTRL hCtrl;

	hCtrl = hai_CreateCtrl(CT_STATIC, CtrlID, style, x, y, w, h);
	if (hCtrl)
		ok = hai_SetStaticCtrlInfo(hCtrl, pText, PicID);
	if (hCtrl && !ok)
	{
		_hai_DestroyCtrl(hCtrl, S_FALSE);
		hCtrl = S_NULL;
	}
	return hCtrl;
}

S_DWORD hai_StaticCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		{
			SH_DC hDC;

			if (!pCtrl->visible)
				break;
			hDC = hai_GetSceneDC();
			hai_SendEvent(EVT_CTRLCOLOR, (WPARAM)(pCtrl->CtrlID), (LPARAM)hDC);
			if (pCtrl->StaticData.PicID)
			{
			}
			else
			{
				hai_BitBlt(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, S_NULL, 0, 0, PATCOPY);
			}
			if (pCtrl->StaticData.pText)
				hai_CtrlDrawText(hCtrl, hDC);
			hai_ReleaseDC(hDC);
			_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		}
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		break;	
	case EVT_KEYUP:
		break;	
	case EVT_CHAR:
		break;	
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_MOUSEDN:
		break;
	case EVT_MOUSEUP:
		break;
	case EVT_MOUSEDB:
		break;
	case EVT_MOUSEMV:
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
	return 0;
}


//progress bar
//
#define PROGRESS_WH_OFFSET	3
S_BOOL hai_SetProgressCtrlInfo(SH_CTRL hCtrl, S_SHORT total, S_SHORT MinPos, S_SHORT CurPos, S_SHORT MaxPos, S_BYTE  *BGPicID, S_BYTE  *FGPicID)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	if (!pCtrl || pCtrl->CtrlType != CT_PROGRESS)
		return S_FALSE;

	if (total<2||MinPos<0||CurPos<0||MaxPos<0)
		return S_FALSE;
	if (total <= MaxPos ||MinPos >= MaxPos)
		return S_FALSE;

	if (pCtrl->w <= PROGRESS_WH_OFFSET<<1 || pCtrl->h <= PROGRESS_WH_OFFSET<<1)
		return S_FALSE;
	if ((pCtrl->style&BS_PROGRESS_VHMASK) == PBS_HPROGRESS)
	{
		if (pCtrl->w-(PROGRESS_WH_OFFSET<<1) < total)
			return S_FALSE;
	}
	else //PBS_VPROGRESS
	{
		if (pCtrl->h-(PROGRESS_WH_OFFSET<<1) < total)
			return S_FALSE;
	}
	
	if (CurPos < MinPos || CurPos > MaxPos)
		CurPos = MinPos;
	
	pCtrl->ProgressData.BGPicID = BGPicID;
	pCtrl->ProgressData.FGPicID = FGPicID;
	pCtrl->ProgressData.total  = total;
	pCtrl->ProgressData.MinPos = MinPos;
	pCtrl->ProgressData.CurPos = CurPos;
	pCtrl->ProgressData.MaxPos = MaxPos;

	return S_TRUE;
}

SH_CTRL hai_CreateProgress(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_SHORT total, S_SHORT MinPos, S_SHORT CurPos, S_SHORT MaxPos, S_BYTE  *BGPicID, S_BYTE  *FGPicID)
{
	S_BOOL  ok;
	SH_CTRL hCtrl;

	hCtrl = hai_CreateCtrl(CT_PROGRESS, CtrlID, style, x, y, w, h);
	if (hCtrl)
		ok = hai_SetProgressCtrlInfo(hCtrl, total, MinPos, CurPos, MaxPos, BGPicID, FGPicID);
	if (hCtrl && !ok)
	{
		_hai_DestroyCtrl(hCtrl, S_FALSE);
		hCtrl = S_NULL;
	}
	return hCtrl;
}

S_SHORT hai_GetProgressCurPos(SH_CTRL hCtrl)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	
	if (!pCtrl || pCtrl->CtrlType != CT_PROGRESS)
		return -1;

	return pCtrl->ProgressData.CurPos;
}

S_BOOL hai_SetProgressCurPos(SH_CTRL hCtrl, S_SHORT CurPos)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	
	if (!pCtrl || pCtrl->CtrlType != CT_PROGRESS)
		return S_FALSE;

	if (CurPos < pCtrl->ProgressData.MinPos || CurPos > pCtrl->ProgressData.MaxPos)
		return S_FALSE;

	if (CurPos != pCtrl->ProgressData.CurPos)
		hai_ProgressCtrlProc(hCtrl, EVT_COMMAND, CurPos, 0);

	return S_TRUE;
}

S_DWORD hai_HProgressCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	S_SHORT i, x, y, w, h, step;
	S_COLOR PenC;
	SH_BRUSH hBrush;
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	SH_DC hDC;

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		if (!pCtrl->visible)
			break;
		hDC = hai_GetSceneDC();
		hai_SendEvent(EVT_CTRLCOLOR, (WPARAM)(pCtrl->CtrlID), (LPARAM)hDC);
		x = pCtrl->x+PROGRESS_WH_OFFSET; w = (S_SHORT)(pCtrl->w-(PROGRESS_WH_OFFSET<<1));
		y = pCtrl->y+PROGRESS_WH_OFFSET; h = (S_SHORT)(pCtrl->h-(PROGRESS_WH_OFFSET<<1));
		PenC = ((_SP_PEN)(((SP_DC)(hDC))->hPen))->color;
		if ((pCtrl->style&BS_PROGRESS_STYLE_MASK) == PBS_RULE) 	{
			if (pCtrl->ProgressData.BGPicID) {
			}
			else
				hai_BitBlt(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, S_NULL, 0, 0, PATCOPY);
			if (pCtrl->ProgressData.FGPicID) {
			}
			else	{
				step = w/(pCtrl->ProgressData.total-1);
				_hai_DrawHLine(hDC, x, (S_SHORT)(x+w), (S_SHORT)(y+(h>>1)), PenC);
				if ((h>>1)>2) {
					for (i = 0; i < pCtrl->ProgressData.total; i++)
						_hai_DrawVLine(hDC, (S_SHORT)(x+i*step), y, (S_SHORT)(y+(h>>1)), PenC);
					_hai_DrawVLine(hDC, (S_SHORT)(x+pCtrl->ProgressData.CurPos*step-1), (S_SHORT)(y+(h>>1)+3), (S_SHORT)(y+h), PenC);
					_hai_DrawVLine(hDC, (S_SHORT)(x+pCtrl->ProgressData.CurPos*step+0), (S_SHORT)(y+(h>>1)+2), (S_SHORT)(y+h), PenC);
					_hai_DrawVLine(hDC, (S_SHORT)(x+pCtrl->ProgressData.CurPos*step+1), (S_SHORT)(y+(h>>1)+3), (S_SHORT)(y+h), PenC);
				}
			}
		}
		else if ((pCtrl->style&BS_PROGRESS_STYLE_MASK) == PBS_SLOT) 	{
			if (pCtrl->ProgressData.BGPicID) {
			}
			else {
				hai_BitBlt(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, S_NULL, 0, 0, PATCOPY);
				hBrush = (SH_BRUSH)hai_SelectObject(hDC, (SH_GDIOBJ)hai_GetStockObject(LTGRAY_BRUSH));
				hai_Rectangle(hDC, x, y, (S_SHORT)(x+w-1), (S_SHORT)(y+h-1));
				hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			}
			if (pCtrl->ProgressData.FGPicID) {
			}
			else	{
				step = w/(pCtrl->ProgressData.total-1);
				hBrush = (SH_BRUSH)hai_SelectObject(hDC, (SH_GDIOBJ)hai_GetStockObject(DKGRAY_BRUSH));
				if (step*pCtrl->ProgressData.CurPos && h-2)
					hai_BitBlt(hDC, (S_SHORT)(x+1), (S_SHORT)(y+1), (S_SHORT)(step*pCtrl->ProgressData.CurPos), (S_SHORT)(h-2), S_NULL, 0, 0, PATCOPY);
				hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
			}
		}
		else { //PBS_BLOCK
			step = w/pCtrl->ProgressData.total;
			if (pCtrl->ProgressData.BGPicID) {
			}
			else {
				hai_BitBlt(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, S_NULL, 0, 0, PATCOPY);
				hai_Rectangle(hDC, x, y, (S_SHORT)(x+w-1), (S_SHORT)(y+h-1));
				if (step > 2 && h > 4)
					for (i = pCtrl->ProgressData.CurPos+1; i <= pCtrl->ProgressData.MaxPos; i++)
						hai_Draw3DBlock(hDC, (S_SHORT)(x+i*step+2), (S_SHORT)(y+2), (S_SHORT)(step-2), (S_SHORT)(h-4), S_FALSE, GRAY_BRUSH);
			}
			if (pCtrl->ProgressData.FGPicID) {
			}
			else	{
				if (step > 2 && h > 4)
					for (i = pCtrl->ProgressData.MinPos; i <= pCtrl->ProgressData.CurPos; i++)
						hai_Draw3DBlock(hDC, (S_SHORT)(x+i*step+2), (S_SHORT)(y+2), (S_SHORT)(step-2), (S_SHORT)(h-4), S_TRUE, GRAY_BRUSH);
			}
		}
		if (!pCtrl->grayed && pCtrl->focus)
			hai_DrawBoxByDashed(hDC, (S_SHORT)(pCtrl->x), (S_SHORT)(pCtrl->y), (S_SHORT)(pCtrl->w-1), (S_SHORT)(pCtrl->h-1), COLOR_BLACK);
		hai_ReleaseDC(hDC);
		_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		if (wParam == VK_LEFT) {
			if (pCtrl->ProgressData.CurPos > pCtrl->ProgressData.MinPos) {
				pCtrl->ProgressData.CurPos--;
				hai_HProgressCtrlProc(hCtrl, EVT_PAINT, 0, 0);
				hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(0,pCtrl->CtrlID), (LPARAM)HAI_COMBDWORD(0,pCtrl->ProgressData.CurPos));
			}
		}
		else if (wParam == VK_RIGHT) {
			if (pCtrl->ProgressData.CurPos < pCtrl->ProgressData.MaxPos) {
				pCtrl->ProgressData.CurPos++;
				hai_HProgressCtrlProc(hCtrl, EVT_PAINT, 0, 0);
				hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(0,pCtrl->CtrlID), (LPARAM)HAI_COMBDWORD(0,pCtrl->ProgressData.CurPos));
			}
		}
		else
			hai_SendEvent(EVT_KEYDN, wParam, lParam);
		break;
	case EVT_KEYUP:
		if (wParam != VK_LEFT && wParam != VK_RIGHT)
			hai_SendEvent(EVT_KEYUP, wParam, lParam);
		break;
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_MOUSEDN:
	case EVT_MOUSEMV:
		if (!pCtrl->focus)
			_hai_SetFocusCtrl(hCtrl, S_FALSE);
		x = HAI_GETLOWORD(lParam)-pCtrl->x;
		if (x < PROGRESS_WH_OFFSET)
			break;
		x -= PROGRESS_WH_OFFSET;
		w = (S_SHORT)(pCtrl->w-(PROGRESS_WH_OFFSET<<1));
		if ((pCtrl->style&BS_PROGRESS_STYLE_MASK) == PBS_BLOCK)
			step = w/(pCtrl->ProgressData.total);
		else
			step = w/(pCtrl->ProgressData.total-1);
		x = x/step;
		if (x < pCtrl->ProgressData.MinPos || x == pCtrl->ProgressData.CurPos || x > pCtrl->ProgressData.MaxPos)
			break;
		pCtrl->ProgressData.CurPos = x;
		hai_HProgressCtrlProc(hCtrl, EVT_PAINT, 0, 0);
		hai_SendEvent(EVT_COMMAND, (WPARAM)HAI_COMBWORD(0,pCtrl->CtrlID), (LPARAM)HAI_COMBDWORD(0,pCtrl->ProgressData.CurPos));
		break;
	case EVT_MOUSEUP:
		break;
	case EVT_MOUSEDB:
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		pCtrl->ProgressData.CurPos = (S_SHORT)wParam;
		hai_HProgressCtrlProc(hCtrl, EVT_PAINT, 0, 0);
		break;
	default:
		break;
	}
	return 0;
}

S_DWORD hai_VProgressCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		break;
	case EVT_KEYUP:
		break;
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		break;
	case EVT_MOUSEDN:
		break;
	case EVT_MOUSEUP:
		break;
	case EVT_MOUSEDB:
		break;
	case EVT_MOUSEMV:
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		break;
	default:
		break;
	}
	return 0;
}

S_DWORD hai_ProgressCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	switch (((SP_CTRL)hCtrl)->style & BS_PROGRESS_VHMASK)
	{
	case PBS_HPROGRESS:
		return hai_HProgressCtrlProc(hCtrl, event, wParam, lParam);
	case PBS_VPROGRESS:
		return hai_VProgressCtrlProc(hCtrl, event, wParam, lParam);
	default:
		break;
	}
	return 0;
}


//edit box
//
S_BYTE *hai_GetEditBoxData(SH_CTRL hCtrl)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	if (!pCtrl || pCtrl->CtrlType != CT_EDITBOX)
		return S_NULL;
	
	return (pCtrl->EditBoxData.pText);
}

S_BOOL hai_SetEditBoxCtrlInfo(SH_CTRL hCtrl, S_BYTE *pText, S_WORD TextLength)
{
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;

	if (!pCtrl || pCtrl->CtrlType != CT_EDITBOX)
		return S_FALSE;

	if (pCtrl->EditBoxData.pText)
		hai_MemFree(pCtrl->EditBoxData.pText);

	if (pText)
	{
		if (TextLength < hai_strlen(pText))
			TextLength = hai_strlen(pText);
	}
	pCtrl->EditBoxData.pText = hai_MemAlloc(TextLength+2);
	if (!pCtrl->EditBoxData.pText)
		return S_FALSE;
	memset(pCtrl->EditBoxData.pText, 0x00, TextLength+2);
	pCtrl->EditBoxData.TextLength = TextLength;
	if (pText)
	{
//		pCtrl->EditBoxData.CurTextLeng = hai_strlen(pText);
		pCtrl->EditBoxData.CursorX = hai_strlen(pText);//pCtrl->EditBoxData.CurTextLeng;
/*		if (pCtrl->EditBoxData.CursorX > TextLength)
		{
			pCtrl->EditBoxData.CursorX = TextLength;
			pText[TextLength] = 0;
		}*/
		hai_strcpy(pCtrl->EditBoxData.pText, pText);
	}

	return S_TRUE;
}

SH_CTRL hai_CreateEditBox(S_DWORD style, S_BYTE CtrlID, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BYTE *pText, S_WORD TextLength)
{
	S_BOOL  ok;
	SH_CTRL hCtrl;

	hCtrl = hai_CreateCtrl(CT_EDITBOX, CtrlID, style, x, y, w, h);
	if (hCtrl)
		ok = hai_SetEditBoxCtrlInfo(hCtrl, pText, TextLength);
	if (hCtrl && !ok)
	{
		_hai_DestroyCtrl(hCtrl, S_FALSE);
		hCtrl = S_NULL;
	}
	return hCtrl;
}

S_BYTE _hai_CaretLocation(S_BYTE *pStr, S_WORD CurPos)
{
	S_BYTE offset = 0;
	S_WORD leng;
	S_SHORT i;

	if (!pStr || !*pStr || CurPos == 0)
		return 0;
	if (CurPos == 1)
		return 1;
	leng = hai_strlen(pStr);
	if (CurPos > leng)
		return 0;

	for(i = CurPos-2; i > 0; i--)
	{
		if (!(pStr[i] & 0x80))
			break;
	}
	if (i)
		i += 1;
	while (i < CurPos)
	{
		if (pStr[i] & 0x80)
		{
			i += 2;
			offset = 2;
		}
		else
		{
			i += 1;
			offset = 1;
		}
	}

	return offset;
}

S_DWORD hai_EditBoxCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	S_BYTE offset, Focus, CaretShow;
	S_WORD TextLeng, i, w, h;
	SH_DC  hDC;
	SH_BRUSH hBrush;
	_SP_FONT pFont;
	SP_CTRL  pCtrl = (SP_CTRL)hCtrl;

	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		if (!pCtrl->visible)
			break;
		CaretShow = _hai_HideCaret();
		hDC = hai_GetSceneDC();
		hai_SendEvent(EVT_CTRLCOLOR, (WPARAM)(pCtrl->CtrlID), (LPARAM)hDC);
		if (pCtrl->grayed)
		{
			switch (pCtrl->style & BS_EDITBOX_STYLE_MASK)
			{
			case BS_EDITBOX_NULL:
			case BS_EDITBOX_UNDERLINE:
			case BS_EDITBOX_BORDER:
				hBrush = (SH_BRUSH)hai_GetStockObject(GRAY_BRUSH);
				hBrush = (SH_BRUSH)hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
				hai_BitBlt(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, S_NULL, 0, 0, PATCOPY);
				hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
				if ((pCtrl->style & BS_EDITBOX_STYLE_MASK) == BS_EDITBOX_NULL)
					break;
				hai_MoveTo(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+pCtrl->h), S_NULL);
				hai_LineTo(hDC, (S_SHORT)(pCtrl->x+pCtrl->w), (S_SHORT)(pCtrl->y+pCtrl->h));
				if ((pCtrl->style & BS_EDITBOX_STYLE_MASK) == BS_EDITBOX_BORDER)
				{
					hai_LineTo(hDC, (S_SHORT)(pCtrl->x+pCtrl->w), pCtrl->y);
					hai_LineTo(hDC, pCtrl->x, pCtrl->y);
					hai_LineTo(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+pCtrl->h));
				}
				break;
			case BS_EDITBOX_3DBOX:
				hai_Draw3DBlock(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, BLOCK_3D_DN, DKGRAY_BRUSH);
				break;
			}
		}
		else
		{
			switch (pCtrl->style & BS_EDITBOX_STYLE_MASK)
			{
			case BS_EDITBOX_NULL:
			case BS_EDITBOX_UNDERLINE:
			case BS_EDITBOX_BORDER:
				hai_BitBlt(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, S_NULL, 0, 0, PATCOPY);
				if ((pCtrl->style & BS_EDITBOX_STYLE_MASK) == BS_EDITBOX_NULL)
					break;
				hai_MoveTo(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+pCtrl->h), S_NULL);
				hai_LineTo(hDC, (S_SHORT)(pCtrl->x+pCtrl->w), (S_SHORT)(pCtrl->y+pCtrl->h));
				if ((pCtrl->style & BS_EDITBOX_STYLE_MASK) == BS_EDITBOX_BORDER)
				{
					hai_LineTo(hDC, (S_SHORT)(pCtrl->x+pCtrl->w), pCtrl->y);
					hai_LineTo(hDC, pCtrl->x, pCtrl->y);
					hai_LineTo(hDC, pCtrl->x, (S_SHORT)(pCtrl->y+pCtrl->h));
				}
				break;
			case BS_EDITBOX_3DBOX:
				hai_Draw3DBlock(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, BLOCK_3D_DN, LTGRAY_BRUSH);
				break;
			}
		}
		if (pCtrl->EditBoxData.pText)
			hai_CtrlDrawText(hCtrl, hDC);
		if (!pCtrl->grayed && pCtrl->focus)
		{
			if (CaretShow)
				_hai_ShowCaret();
		}
		hai_ReleaseDC(hDC);
		_hai_ObjectUpdateRect(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		hDC = hai_GetSceneDC();			
		hai_SendEvent(EVT_CTRLCOLOR, (WPARAM)(pCtrl->CtrlID), (LPARAM)hDC);
		pFont = (_SP_FONT)(((SP_DC)hDC)->hFont);
		w = pFont->width; h = pFont->height;
		hai_ReleaseDC(hDC);
		switch (wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_UP:
		case VK_DOWN:
			hai_SendEvent(EVT_KEYDN, wParam, lParam);
			break;
		case VK_LEFT:
			if (pCtrl->EditBoxData.CursorX == 0)
				break;
			pCtrl->EditBoxData.CursorX -= _hai_CaretLocation(pCtrl->EditBoxData.pText, pCtrl->EditBoxData.CursorX);
			hai_SetCaretPos((S_SHORT)(pCtrl->x+pCtrl->EditBoxData.CursorX*w+2), (S_SHORT)(pCtrl->y+pCtrl->h-(pCtrl->h-h)/2+2));
			break;
		case VK_RIGHT:
			if (pCtrl->EditBoxData.CursorX >= hai_strlen(pCtrl->EditBoxData.pText))
				break;
			if (pCtrl->EditBoxData.pText[pCtrl->EditBoxData.CursorX] & 0x80)
				pCtrl->EditBoxData.CursorX += 2;
			else
				pCtrl->EditBoxData.CursorX += 1;
			hai_SetCaretPos((S_SHORT)(pCtrl->x+pCtrl->EditBoxData.CursorX*w+2), (S_SHORT)(pCtrl->y+pCtrl->h-(pCtrl->h-h)/2+2));
			break;
		case VK_BACK:
			if (pCtrl->EditBoxData.CursorX > 0)
			{
				TextLeng = hai_strlen(pCtrl->EditBoxData.pText);
				offset = _hai_CaretLocation(pCtrl->EditBoxData.pText, pCtrl->EditBoxData.CursorX);
				pCtrl->EditBoxData.CursorX -= offset;
				for (i = pCtrl->EditBoxData.CursorX; i < TextLeng; i++)
					pCtrl->EditBoxData.pText[i] = pCtrl->EditBoxData.pText[i+offset];
				hai_SetCaretPos((S_SHORT)(pCtrl->x+pCtrl->EditBoxData.CursorX*w+2), (S_SHORT)(pCtrl->y+pCtrl->h-(pCtrl->h-h)/2+2));
				hai_SendCtrlEvt(hCtrl, EVT_PAINT, 0, 0);
			}
			break;
		case VK_DELETE:
			TextLeng = hai_strlen(pCtrl->EditBoxData.pText);
			if (pCtrl->EditBoxData.CursorX < TextLeng)
			{
				offset = 1;
				if (pCtrl->EditBoxData.pText[pCtrl->EditBoxData.CursorX] & 0x80)
					offset = 2;
				for (i = pCtrl->EditBoxData.CursorX; i < TextLeng; i++)
					pCtrl->EditBoxData.pText[i] = pCtrl->EditBoxData.pText[i+offset];
				hai_SendCtrlEvt(hCtrl, EVT_PAINT, 0, 0);
			}
			break;
		default:
			hai_SendEvent(EVT_KEYDN, wParam, lParam);
			break;
		}
		break;
	case EVT_KEYUP:
		switch (wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_UP:
		case VK_DOWN:
			hai_SendEvent(EVT_KEYUP, wParam, lParam);
		case VK_LEFT:
		case VK_RIGHT:
		case VK_BACK:
		case VK_DELETE:
			break;
		default:
			hai_SendEvent(EVT_KEYUP, wParam, lParam);
			break;
		}
		break;
	case EVT_CHAR:
		hDC = hai_GetSceneDC();			
		hai_SendEvent(EVT_CTRLCOLOR, (WPARAM)(pCtrl->CtrlID), (LPARAM)hDC);
		pFont = (_SP_FONT)(((SP_DC)hDC)->hFont);
		w = pFont->width; h = pFont->height;
		hai_ReleaseDC(hDC);
		TextLeng = hai_strlen(pCtrl->EditBoxData.pText);
		if (wParam & 0x80)
		{
			if (TextLeng+2 <= pCtrl->EditBoxData.TextLength)
			{
				for (i = TextLeng; i > pCtrl->EditBoxData.CursorX; i--)
					pCtrl->EditBoxData.pText[i+1] = pCtrl->EditBoxData.pText[i-1];
				pCtrl->EditBoxData.pText[pCtrl->EditBoxData.CursorX] = (wParam>>8);
				pCtrl->EditBoxData.pText[pCtrl->EditBoxData.CursorX+1] = (wParam&0xFF);
				pCtrl->EditBoxData.CursorX += 2;
				hai_SetCaretPos((S_SHORT)(pCtrl->x+pCtrl->EditBoxData.CursorX*w+2), (S_SHORT)(pCtrl->y+pCtrl->h-(pCtrl->h-h)/2+2));
				hai_SendCtrlEvt(hCtrl, EVT_PAINT, 0, 0);
			}
		}
		else
		{
			if (TextLeng+1 <= pCtrl->EditBoxData.TextLength)
			{
				for (i = TextLeng; i > pCtrl->EditBoxData.CursorX; i--)
					pCtrl->EditBoxData.pText[i] = pCtrl->EditBoxData.pText[i-1];
				pCtrl->EditBoxData.pText[pCtrl->EditBoxData.CursorX] = wParam & 0xFF;
				pCtrl->EditBoxData.CursorX += 1;
				hai_SetCaretPos((S_SHORT)(pCtrl->x+pCtrl->EditBoxData.CursorX*w+2), (S_SHORT)(pCtrl->y+pCtrl->h-(pCtrl->h-h)/2+2));
				hai_SendCtrlEvt(hCtrl, EVT_PAINT, 0, 0);
			}
		}
		break;
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		if (pCtrl->focus)
			hai_HideCaret();
		break;
	case EVT_MOUSEDN:
		_hai_SetFocusCtrl(hCtrl, S_FALSE);
		break;
	case EVT_MOUSEUP:
		break;
	case EVT_MOUSEDB:
		break;
	case EVT_MOUSEMV:
		break;
	case EVT_RESTORE:
		break;
	case EVT_COMMAND:
		break;
	case EVT_FOCUS:
		Focus = HAI_GETHIBYTE(wParam);
		if (Focus == HG_GETFOCUS) {
			hai_HideCaret();
			hai_InitCaret();
			hDC = hai_GetSceneDC();
			hai_SendEvent(EVT_CTRLCOLOR, (WPARAM)(pCtrl->CtrlID), (LPARAM)hDC);
			pFont = (_SP_FONT)(((SP_DC)hDC)->hFont);
			hai_SetCaretPos((S_SHORT)(pCtrl->x+pCtrl->EditBoxData.CursorX*pFont->width+2), (S_SHORT)(pCtrl->y+pCtrl->h-(pCtrl->h-pFont->height)/2+2));
			hai_ReleaseDC(hDC);
			hai_ShowCaret();
		}
		else if (Focus == HG_KILLFOCUS)
			hai_HideCaret();
		break;
	default:
		break;
	}
	return 0;
}

//scroll text box
//



//ctrl comm
//
Hai_CtrlProc *hai_GetCtrlTypeProc(S_BYTE CtrlType)
{
	switch (CtrlType)
	{
	case CT_BUTTON:
		return &hai_ButtonCtrlProc;
	case CT_EVTAREA:
		return &hai_EvtAreaCtrlProc;
	case CT_LISTAREA:
		return &hai_ListAreaCtrlProc;
	case CT_SCRLBAR:
		return &hai_ScrlBarCtrlProc;
	case CT_STATIC:
		return &hai_StaticCtrlProc;
	case CT_PROGRESS:
		return &hai_ProgressCtrlProc;
	case CT_EDITBOX:
		return &hai_EditBoxCtrlProc;
	default:
		break;
	}
	return S_NULL;
}

S_BOOL _CantCtrlGetFocus(SP_CTRL pCtrl)
{
	return (pCtrl->CtrlType == CT_EVTAREA || pCtrl->CtrlType == CT_LISTAREA);
}

S_BOOL _AlwaysGrayCtrl(SP_CTRL pCtrl)
{
	return (pCtrl->CtrlType == CT_STATIC || pCtrl->CtrlType == CT_SCRLTEXT);
}


