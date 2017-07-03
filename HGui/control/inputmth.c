//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			inputmth.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2007-09-06  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#include "haitype.h"
#include "haihandle.h"
#include "haievent.h"
#include "haiscene.h"
#include "_haiscene.h"
#include "_haictrl.h"
#include "haigui.h"
#include "_haigdi.h"
#include "haigdi.h"
#include "virkey.h"
#include "haimem.h"


//data
//
static const S_BYTE WuBiData[] = 
{
#include "wubidata.h"
	0x00
};

static const S_BYTE WuBiOffset[] = 
{
#include "wubiindex.h"
	0x00
};

static const S_WORD WuBiIndex[] = 
{
	0x0000, 0x0504, 0x075A, 0x0970, 0x0F25, 0x11BA, 0x16FC, 0x1C51,
	0x1E1C, 0x23F1, 0x272D, 0x2AE4, 0x2D72, 0x2FC1, 0x332E, 0x3594,
	0x3910, 0x3DC8, 0x435D, 0x473D, 0x4DBE, 0x5320, 0x552B, 0x5B85,
	0x5E98, 0x6392
};

static const S_BYTE NumPrompt[5][3] =
{
	"1.", "2.", "3.", "4.", "5."
};

//macro
//
#define WUBI_DATA_OFFSET_MAX_BYTE		3


//variable
//
//SP_CTRL pInputCtrl = S_NULL;
//S_CTRL  InputCtrl;
//S_BYTE  InputText[INPUTM_INPUT_TEXT_LENGTH_MAX];
//S_BYTE  InputPromptText[INPUTM_TEXT_LINE_MAX][INPUTM_DISP_TEXT_LENGTH_MAX];

//function prototype
//
S_DWORD hai_WubiInputMethodCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam);

//function
//
SP_CTRL _hai_CreateWubiInputMethod(S_VOID)
{
	S_BYTE i;
	S_BYTE *pInputText=S_NULL, *pInputPromptText=S_NULL;
	SP_CTRL pInputCtrl=S_NULL;

	pInputCtrl = hai_MemAlloc(sizeof(*pInputCtrl));
	pInputText = hai_MemAlloc(INPUTM_INPUT_TEXT_LENGTH_MAX);
	pInputPromptText = hai_MemAlloc(INPUTM_TEXT_LINE_MAX*INPUTM_DISP_TEXT_LENGTH_MAX);
	if (!pInputCtrl || !pInputText || !pInputPromptText)
	{
		if (pInputCtrl)  hai_MemFree(pInputCtrl);
		if (pInputText) hai_MemFree(pInputText);
		if (pInputPromptText) hai_MemFree(pInputPromptText);
		return S_NULL;
	}

	memset(pInputCtrl, 0x00, sizeof(*pInputCtrl));
	pInputCtrl->CtrlID   = 0xFF;
	pInputCtrl->CtrlType = CT_INPUTM;
	pInputCtrl->style   = INPUTM_TYPE_WUBI;
	pInputCtrl->visible = 1;
	pInputCtrl->x = (LCD_WIDTH-320)>>1;
	pInputCtrl->y = LCD_HEIGHT - 20;
	pInputCtrl->w = 320;
	pInputCtrl->h = 20;
	pInputCtrl->CtrlProc = hai_WubiInputMethodCtrlProc;
	pInputCtrl->InputMethodData.pInputText = pInputText;
	for (i = 0; i < INPUTM_TEXT_LINE_MAX; i++)
		pInputCtrl->InputMethodData.DisplayText[i] = &pInputPromptText[i*INPUTM_DISP_TEXT_LENGTH_MAX];

	memset(pInputCtrl->InputMethodData.pInputText, 0x00, INPUTM_INPUT_TEXT_LENGTH_MAX);
	for (i = 0; i < INPUTM_TEXT_LINE_MAX; i++)
		memset(pInputCtrl->InputMethodData.DisplayText[i], 0x00, INPUTM_DISP_TEXT_LENGTH_MAX);
	for (i = 0; i < INPUTM_TEXT_LINE_MAX; i++)
		strcpy(pInputCtrl->InputMethodData.DisplayText[i], NumPrompt[i]);

	return pInputCtrl;
}

S_BOOL _hai_DestroyWubiInputMethod(SP_CTRL pInputCtrl)
{
	if (!pInputCtrl || pInputCtrl->CtrlType != CT_INPUTM)
		return S_FALSE;
	if (pInputCtrl->style != INPUTM_TYPE_WUBI)
		return S_FALSE;

	hai_MemFree(pInputCtrl->InputMethodData.pInputText);
	hai_MemFree(pInputCtrl->InputMethodData.DisplayText[0]);
	hai_MemFree(pInputCtrl);

	return S_TRUE;
}

S_BYTE *hai_FindAlpha(S_BYTE *StrBuf)
{
	while (*StrBuf)
	{
		if (*StrBuf & 0x80)
			StrBuf += 2;
		else
			break;
	}
	return StrBuf;
}

S_VOID _hai_WuBiDataSearch(SP_CTRL pInputCtrl)
{
	S_BYTE i, ch;
	S_SHORT match;
	S_WORD  LowItem, MidItem, HighItem;
	S_DWORD CurrOffset, NextOffset;
	S_BYTE  WuBiDataBuf[20], *pAlpha;

	ch = pInputCtrl->InputMethodData.pInputText[0];
	if (!ch || ch == 'z')
	{
		pInputCtrl->InputMethodData.CurrDataIndex = 0;
		for (i = 0; i < INPUTM_TEXT_LINE_MAX; i++)
			memset(pInputCtrl->InputMethodData.DisplayText[i], 0x00, INPUTM_DISP_TEXT_LENGTH_MAX);
		for (i = 0; i < INPUTM_TEXT_LINE_MAX; i++)
			strcpy(pInputCtrl->InputMethodData.DisplayText[i], NumPrompt[i]);
		return;
	}

	ch -= 'a';
	LowItem = WuBiIndex[ch];
	if (pInputCtrl->InputMethodData.pInputText[1])
	{
		HighItem = WuBiIndex[ch+1];
		while (LowItem < HighItem)
		{
			MidItem = (LowItem + HighItem) / 2;
			CurrOffset = 0; NextOffset = 0;
			memcpy(&CurrOffset, &WuBiOffset[MidItem*3], 3);
			memcpy(&NextOffset, &WuBiOffset[(MidItem+1)*3], 3);
			memcpy(WuBiDataBuf, &WuBiData[CurrOffset], NextOffset-CurrOffset);
			WuBiDataBuf[NextOffset-CurrOffset] = 0;
			pAlpha = hai_FindAlpha(WuBiDataBuf);
			match = strcmp(pInputCtrl->InputMethodData.pInputText, pAlpha);
			if (match == 0)
				break;
			else if (match > 0)
				LowItem = MidItem+1;
			else
				HighItem = MidItem-1;
		}
		if (match == 0)
		{
			LowItem = MidItem;
		}
	}

	pInputCtrl->InputMethodData.CurrDataIndex = LowItem;

	for (i = 0; i < INPUTM_TEXT_LINE_MAX; i++)
		strcpy(pInputCtrl->InputMethodData.DisplayText[i], NumPrompt[i]);
	for (i = 0; i < INPUTM_TEXT_LINE_MAX && LowItem < WuBiIndex[25]; i++)
	{
		CurrOffset = 0; NextOffset = 0;
		memcpy(&CurrOffset, &WuBiOffset[LowItem * WUBI_DATA_OFFSET_MAX_BYTE], WUBI_DATA_OFFSET_MAX_BYTE);
		memcpy(&NextOffset, &WuBiOffset[(LowItem+1) * WUBI_DATA_OFFSET_MAX_BYTE], WUBI_DATA_OFFSET_MAX_BYTE);
		memcpy(&(pInputCtrl->InputMethodData.DisplayText[i][2]), &WuBiData[CurrOffset], NextOffset-CurrOffset);
		pInputCtrl->InputMethodData.DisplayText[i][NextOffset-CurrOffset+2] = 0;
		LowItem += 1;
	}

	return;
}

S_VOID  _hai_SendFont(S_BYTE *Font)
{
	S_WORD HaZi;
	SH_CTRL hFocusCtrl = hai_GetCurSceneFocusCtrl();

	while (*Font & 0x80)
	{
		HaZi = (S_WORD)*Font<<8 | *(Font+1);
		if (hFocusCtrl)
			((SP_CTRL)hFocusCtrl)->CtrlProc(hFocusCtrl, EVT_CHAR, (WPARAM)HaZi, 0);
		else
			hai_SendEvent(EVT_CHAR, (WPARAM)HaZi, 0);
		Font += 2;
	}
}

S_DWORD hai_WubiInputMethodCtrlProc(SH_CTRL hCtrl, S_BYTE event, WPARAM wParam, LPARAM lParam)
{
	S_BYTE  bkmode;
	SH_DC   hDC;
	SP_CTRL pCtrl = (SP_CTRL)hCtrl;
	SH_CTRL hFocusCtrl = hai_GetCurSceneFocusCtrl();
	
	switch (event)
	{
	case EVT_CREAT:
		break;
	case EVT_PAINT:
		if (!pCtrl->visible)
			break;
		hDC = hai_GetScreenDC();
		hai_Draw3DBlock(hDC, pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h, BLOCK_3D_UP, GRAY_BRUSH);
		hai_Draw3DBlock(hDC, (S_SHORT)(pCtrl->x+2), (S_SHORT)(pCtrl->y+2), 44, 16, BLOCK_3D_DN, LTGRAY_BRUSH);
		hai_Draw3DBlock(hDC, (S_SHORT)(pCtrl->x+46), (S_SHORT)(pCtrl->y+2), 272, 16, BLOCK_3D_DN, LTGRAY_BRUSH);
		bkmode = hai_SetBkMode(hDC, BKM_TRANSPARENT);
		hai_TextOut(hDC, (S_SHORT)(pCtrl->x+6), (S_SHORT)(pCtrl->y+4), pCtrl->InputMethodData.pInputText, 0);
		hai_TextOut(hDC, (S_SHORT)(pCtrl->x+50), (S_SHORT)(pCtrl->y+4), pCtrl->InputMethodData.DisplayText[0], 0);
		hai_TextOut(hDC, (S_SHORT)(pCtrl->x+142), (S_SHORT)(pCtrl->y+4), pCtrl->InputMethodData.DisplayText[1], 0);
		hai_TextOut(hDC, (S_SHORT)(pCtrl->x+234), (S_SHORT)(pCtrl->y+4), pCtrl->InputMethodData.DisplayText[2], 0);
		hai_SetBkMode(hDC, bkmode);
		hai_ReleaseDC(hDC);
		if (!hai_IsNeedRedraw())
			hai_UpdateWindow(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		break;	
	case EVT_TIMER:
		break;	
	case EVT_KEYDN:
		switch (wParam)
		{
		case VK_ESCAPE:
		case VK_SPACE:
		case VK_RETURN:
			if (pCtrl->InputMethodData.InputIndicate)
			{
			}
			else
			{
				if (hFocusCtrl)
					((SP_CTRL)hFocusCtrl)->CtrlProc(hFocusCtrl, EVT_KEYDN, wParam, lParam);
				else
					hai_SendEvent(EVT_KEYDN, wParam, lParam);
			}
			break;
		case VK_BACK:
			if (pCtrl->InputMethodData.InputIndicate)
			{
				pCtrl->InputMethodData.pInputText[--pCtrl->InputMethodData.InputIndicate] = '\0';
				_hai_WuBiDataSearch(pCtrl);
				pCtrl->CtrlProc(hCtrl, EVT_PAINT, 0, 0);
			}
			else
			{
				if (hFocusCtrl)
					((SP_CTRL)hFocusCtrl)->CtrlProc(hFocusCtrl, EVT_KEYDN, wParam, lParam);
				else
					hai_SendEvent(EVT_KEYDN, wParam, lParam);
			}
			break;
		default:
			if (hFocusCtrl)
				((SP_CTRL)hFocusCtrl)->CtrlProc(hFocusCtrl, EVT_KEYDN, wParam, lParam);
			else
				hai_SendEvent(EVT_KEYDN, wParam, lParam);
			break;
		}
		break;
	case EVT_KEYUP:
		switch (wParam)
		{
		case VK_SPACE:
		case VK_RETURN:
			if (pCtrl->InputMethodData.InputIndicate)
			{
				if (pCtrl->InputMethodData.DisplayText[0][2])
					_hai_SendFont(&(pCtrl->InputMethodData.DisplayText[0][2]));
				memset(pCtrl->InputMethodData.pInputText, 0x00, INPUTM_INPUT_TEXT_LENGTH_MAX);
				pCtrl->InputMethodData.InputIndicate = 0;
				pCtrl->CtrlProc(hCtrl, EVT_PAINT, 0, 0);
			}
			else
			{
				if (hFocusCtrl)
					((SP_CTRL)hFocusCtrl)->CtrlProc(hFocusCtrl, EVT_KEYUP, wParam, lParam);
				else
					hai_SendEvent(EVT_KEYUP, wParam, lParam);
			}
			break;
		case VK_ESCAPE:
			if (pCtrl->InputMethodData.InputIndicate)
			{
				pCtrl->InputMethodData.InputIndicate = 0;
				pCtrl->InputMethodData.pInputText[0] = '\0';
				_hai_WuBiDataSearch(pCtrl);
				pCtrl->CtrlProc(hCtrl, EVT_PAINT, 0, 0);
			}
			else
			{
				if (hFocusCtrl)
					((SP_CTRL)hFocusCtrl)->CtrlProc(hFocusCtrl, EVT_KEYUP, wParam, lParam);
				else
					hai_SendEvent(EVT_KEYUP, wParam, lParam);
			}
			break;
		case VK_BACK:
			if (pCtrl->InputMethodData.InputIndicate)
			{
			}
			else
			{
				if (hFocusCtrl)
					((SP_CTRL)hFocusCtrl)->CtrlProc(hFocusCtrl, EVT_KEYUP, wParam, lParam);
				else
					hai_SendEvent(EVT_KEYUP, wParam, lParam);
			}
			break;
		default:
			if (hFocusCtrl)
				((SP_CTRL)hFocusCtrl)->CtrlProc(hFocusCtrl, EVT_KEYUP, wParam, lParam);
			else
				hai_SendEvent(EVT_KEYUP, wParam, lParam);
			break;
		}
		break;
	case EVT_CHAR:
		if (wParam > '0' && wParam <= '0'+INPUTM_TEXT_LINE_MAX)
		{
			if (pCtrl->InputMethodData.DisplayText[wParam-'0'-1][2])
				_hai_SendFont(&(pCtrl->InputMethodData.DisplayText[wParam-'0'-1][2]));
			memset(pCtrl->InputMethodData.pInputText, 0x00, INPUTM_INPUT_TEXT_LENGTH_MAX);
			pCtrl->InputMethodData.InputIndicate = 0;
			pCtrl->CtrlProc(hCtrl, EVT_PAINT, 0, 0);
		}
		else if (wParam >= 'a' && wParam <= 'z')
		{
			if (pCtrl->InputMethodData.InputIndicate < 4)
				pCtrl->InputMethodData.pInputText[pCtrl->InputMethodData.InputIndicate++] = (S_BYTE)wParam;
			_hai_WuBiDataSearch(pCtrl);
			pCtrl->CtrlProc(hCtrl, EVT_PAINT, 0, 0);
		}
		else
		{
			if (!pCtrl->InputMethodData.pInputText[0])
			{
				if (hFocusCtrl)
					((SP_CTRL)hFocusCtrl)->CtrlProc(hFocusCtrl, EVT_CHAR, wParam, lParam);
				else
					hai_SendEvent(EVT_CHAR, wParam, lParam);
			}
		}
		break;
	case EVT_SYSKEY:
		break;
	case EVT_DESTROY:
		if (hai_IsNeedRedraw())
			_hai_SetInvalidRectEx(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
		else
			hai_InvalidRectEx(pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h);
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

SP_CTRL _hai_CreateInputMethod(S_BYTE InputMethodType)
{
	SP_CTRL pCtrl = S_NULL;
	
	switch (InputMethodType)
	{
	case INPUTM_TYPE_WUBI:
		pCtrl = _hai_CreateWubiInputMethod();
		break;
	default:
		break;
	}

	return pCtrl;
}

S_BOOL _hai_DestroyInputMethod(SP_CTRL pInputCtrl)
{
	if (!pInputCtrl)
		return S_FALSE;
	if (pInputCtrl->CtrlType != CT_INPUTM)
		return S_FALSE;

	switch (pInputCtrl->style)
	{
	case INPUTM_TYPE_WUBI:
		_hai_DestroyWubiInputMethod(pInputCtrl);
		break;
	default:
		return S_FALSE;
	}
	
	return S_TRUE;
}



