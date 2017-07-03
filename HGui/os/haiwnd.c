//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haiwnd.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-09-06  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#include "haitype.h"
#include "haiwnd.h"
#include "haihandle.h"
#include "haimem.h"
#include "haigdi.h"


SH_WND hai_CreateWindow(const S_BYTE* ptitle, S_WORD style, S_SHORT x, S_SHORT y, 
						S_SHORT width, S_SHORT height, SH_WND hParentWnd, S_WORD ChildID)
{
	SP_WND pwnd, pParent;

	x      = (x>=0 && x<LCD_WIDTH) ? x : 0 ;
	y      = (y>=0 && y<LCD_HEIGHT) ? y : 0 ;
	width  = HAI_ABS(width);
	height = HAI_ABS(height);
	width  = (x+width < LCD_WIDTH) ? width : (LCD_WIDTH-x);
	height = (y+height < LCD_HEIGHT) ? height : (LCD_HEIGHT-y);

	pwnd = (SP_WND)hai_MemAlloc(sizeof(S_WND));
	if (pwnd == S_NULL)
		return S_NULL;

	pwnd->hdltype = HT_WND;
	pwnd->style   = style;
	pwnd->ptitle  = ptitle;
	pwnd->x		  = x;
	pwnd->y		  = y;
	pwnd->width   = width;
	pwnd->height  = height;
	pwnd->ChildID = ChildID;
	pwnd->pWinData = S_NULL;
	pwnd->hBrush   = S_NULL;
	pwnd->ChildWndList = S_NULL;

	if (hParentWnd)
	{
		pParent = (SP_WND)hParentWnd;
		pwnd->ChildWndList = pParent->ChildWndList;
		pParent->ChildWndList = (SH_WND)pwnd;
	}
	return (SH_WND)pwnd;
}

S_BOOL hai_DestroyWindow(SH_WND hwnd)
{
	if (hwnd == S_NULL)
		return S_FALSE;
	//judge main wnd,not del child wnd
//destroy child wnd;
	hai_MemFree(hwnd);
	return S_TRUE;
}

SH_BRUSH hai_SetWindowBrush(SH_WND hWnd, SH_BRUSH hBrush)
{
	SH_BRUSH hCur;

	hCur = ((SP_WND)hWnd)->hBrush;
	((SP_WND)hWnd)->hBrush = hBrush;

	return hCur;
}