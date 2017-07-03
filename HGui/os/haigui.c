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
#include "haitype.h"
#include "haihandle.h"
#include "_haigdi.h"
#include "haigdi.h"
#include "haievent.h"
#include "virkey.h"
#include "hailib.h"
#include "schedule.h"
#include "haigui.h"
#include "haiscene.h"

//string
S_VOID hai_GetStringExtent(SH_DC hDC, S_BYTE *str, S_WORD *width, S_WORD *height)
{
	_SP_FONT pFont;
	S_WORD CharW;

	if (hDC == S_NULL || HAI_GETHDLTYPE(hDC) != HT_DC)
		return;
	if ((pFont = (_SP_FONT)(((SP_DC)hDC)->hFont)) == S_NULL)
		return;

	CharW = pFont->width;
	if (width)
	{
		*width = 0;
		while (*str)
		{
			if (*str & 0x80)
			{
				*width += CharW * 2;
				str += 2;
			}
			else
			{
				*width += CharW;
				str += 1;
			}
		}
	}
	if (height)
		*height = pFont->height;
}

S_VOID hai_DrawBoxByDashed(SH_DC hDC, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_COLOR color)
{
	S_BYTE bkmode;
	SH_PEN hPen;
	
	if (!hDC || x <= 0 || y <= 0 || w <= 0 || h <= 0)
		return;

	bkmode = hai_SetBkMode(hDC, BKM_TRANSPARENT);
	hPen = (SH_PEN)hai_SelectObject(hDC, (SH_GDIOBJ)hai_CreatePen(1, color, PS_DOT));
	hai_MoveTo(hDC, x, y, S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+w), y);
	hai_LineTo(hDC, (S_SHORT)(x+w), (S_SHORT)(y+h));
	hai_LineTo(hDC, x, (S_SHORT)(y+h));
	hai_LineTo(hDC, x, y);
	hai_SetBkMode(hDC, bkmode);
	hPen = (SH_PEN)hai_SelectObject(hDC, (SH_GDIOBJ)hPen);
	hai_DeleteObject((SH_GDIOBJ)hPen);
}

S_BOOL hai_DrawQuitButton(SH_DC hDC, S_SHORT x, S_SHORT y)
{
	return S_TRUE;
}

S_BOOL hai_DrawWndFrame(SH_DC hDC, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BYTE *pszTitle)
{
	SH_BRUSH hBrush;
	S_BYTE text[256];

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_FALSE;
	if (!((SP_DC)hDC)->lpImage)
		return S_FALSE;

	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x > ((SP_DC)hDC)->cx) x = ((SP_DC)hDC)->cx - 1;
	if (y > ((SP_DC)hDC)->cy) y = ((SP_DC)hDC)->cy - 1;
	if (w < 0) w = 0;
	if (h < 0) h = 0;
	if (w+x > ((SP_DC)hDC)->cx) w = ((SP_DC)hDC)->cx - x;
	if (h+y > ((SP_DC)hDC)->cy) h = ((SP_DC)hDC)->cy - y;

	hBrush = (SH_BRUSH)hai_GetStockObject(GRAY_BRUSH);
	hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
	hai_BitBlt(hDC, x, y, w, 17, S_NULL, 0, 0, PATCOPY);
	hBrush = (SH_BRUSH)hai_GetStockObject(LTGRAY_BRUSH);
	hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
	hai_BitBlt(hDC, x, (S_SHORT)(y+16), w, (S_SHORT)(h-16), S_NULL, 0, 0, PATCOPY);

	_hai_DrawHLine(hDC, x, (S_SHORT)(x+w-1), y, COLOR_LIGHT);
	_hai_DrawHLine(hDC, x, (S_SHORT)(x+w-1), (S_SHORT)(y+15), COLOR_DARK);
	_hai_DrawVLine(hDC, (S_SHORT)(x+w-1), (S_SHORT)(y+1), (S_SHORT)(y+15), COLOR_LIGHT);
	_hai_DrawHLine(hDC, x, (S_SHORT)(x+w-1), (S_SHORT)(y+h-1), COLOR_BLACK);
	_hai_DrawVLine(hDC, x, y, (S_SHORT)(y+h-2), COLOR_LIGHT);
	_hai_DrawVLine(hDC, (S_SHORT)(x+w-1), (S_SHORT)(y+1), (S_SHORT)(y+h), COLOR_BLACK);

	_hai_DrawHLine(hDC, (S_SHORT)(x+w-14), (S_SHORT)(x+w-4), (S_SHORT)(y+3),COLOR_WHITE);
	_hai_DrawVLine(hDC, (S_SHORT)(x+w-14), (S_SHORT)(y+3), (S_SHORT)(y+13),COLOR_WHITE);
	_hai_DrawHLine(hDC, (S_SHORT)(x+w-13), (S_SHORT)(x+w-4), (S_SHORT)(y+13),COLOR_BLACK);
	_hai_DrawVLine(hDC, (S_SHORT)(x+w-4), (S_SHORT)(y+4), (S_SHORT)(y+13),COLOR_BLACK);
	hBrush = (SH_BRUSH)hai_GetStockObject(LTGRAY_BRUSH);
	hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
	hai_BitBlt(hDC, (S_SHORT)(x+w-13), (S_SHORT)(y+4), 9, 9, S_NULL, 0, 0, PATCOPY);
	hBrush = (SH_BRUSH)hai_GetStockObject(WHITE_BRUSH);
	hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);

	hai_MoveTo(hDC, (S_SHORT)(x+w-13), (S_SHORT)(y+5), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+w-7), (S_SHORT)(y+11));
	hai_MoveTo(hDC, (S_SHORT)(x+w-12), (S_SHORT)(y+5), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+w-6), (S_SHORT)(y+11));
	hai_MoveTo(hDC, (S_SHORT)(x+w-13), (S_SHORT)(y+11), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+w-7), (S_SHORT)(y+5));
	hai_MoveTo(hDC, (S_SHORT)(x+w-12), (S_SHORT)(y+11), S_NULL);
	hai_LineTo(hDC, (S_SHORT)(x+w-6), (S_SHORT)(y+5));

	hai_strcpy(text, pszTitle);
	if(pszTitle)
	{
		S_COLOR c;
		hai_CutString(text, 6, (S_SHORT)(w-18));
		c = hai_SetTextColor(hDC, COLOR_WHITE);
		w = hai_SetBkMode(hDC, BKM_TRANSPARENT);
		hai_TextOut(hDC, (S_SHORT)(x+2), (S_SHORT)(y+2), text, 0);
		hai_SetTextColor(hDC, c);
		hai_SetBkMode(hDC, (S_BYTE)w);
	}
	return S_TRUE;
}

S_BOOL hai_Draw3DBlock(SH_DC hDC, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BYTE status, S_BYTE SysBrushType)
{
	S_COLOR  c1,c2;
	SH_BRUSH hBrush;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_FALSE;
	if (!((SP_DC)hDC)->lpImage)
		return S_FALSE;

	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x > ((SP_DC)hDC)->cx) x = ((SP_DC)hDC)->cx - 1;
	if (y > ((SP_DC)hDC)->cy) y = ((SP_DC)hDC)->cy - 1;
	if (w < 0) w = 0;
	if (h < 0) h = 0;
	if (w+x > ((SP_DC)hDC)->cx) w = ((SP_DC)hDC)->cx - x;
	if (h+y > ((SP_DC)hDC)->cy) h = ((SP_DC)hDC)->cy - y;
	
	hBrush = (SH_BRUSH)hai_GetStockObject(SysBrushType);
	hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);
	hai_BitBlt(hDC, (S_SHORT)(x+1), (S_SHORT)(y+1), (S_SHORT)(w-2), (S_SHORT)(h-2), S_NULL, 0, 0, PATCOPY);
	hBrush = (SH_BRUSH)hai_GetStockObject(WHITE_BRUSH);
	hai_SelectObject(hDC, (SH_GDIOBJ)hBrush);

	c1 = status? COLOR_LIGHT : COLOR_BLACK;
	c2 = status? COLOR_BLACK : COLOR_LIGHT;

	_hai_DrawHLine(hDC, x, (S_SHORT)(x+w-1), y, c1);
	_hai_DrawHLine(hDC, x, (S_SHORT)(x+w), (S_SHORT)(y+h-1), c2);
	_hai_DrawVLine(hDC, x, y, (S_SHORT)(y+h), c1);
	_hai_DrawVLine(hDC, (S_SHORT)(x+w-1), y, (S_SHORT)(y+h-1),c2);
	return S_TRUE;
}

S_BOOL hai_DrawButton(SH_DC hDC, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BOOL IsUp, S_BYTE *pszText)
{
	if (!hai_Draw3DBlock(hDC,x,y,w,h,IsUp, GRAY_BRUSH))
		return S_FALSE;

	if(pszText)
	{
		S_COLOR c;
		c = hai_SetTextColor(hDC, COLOR_WHITE);
		w = hai_SetBkMode(hDC, BKM_TRANSPARENT);
		hai_TextOut(hDC, (S_SHORT)(x+2), (S_SHORT)(y+2), pszText, 0);
		hai_SetTextColor(hDC, c);
		hai_SetBkMode(hDC, (S_BYTE)w);
	}
	return S_TRUE;
}

S_BOOL hai_IsRectIntersect(SP_RECT pR1, SP_RECT pR2)
{
	S_SHORT l, t, r, b;

	if (!pR1 || !pR2)
		return S_FALSE;
		
	l =  (pR1->x > pR2->x)? (pR1->x):(pR2->x);
	t =  (pR1->y > pR2->y)? (pR1->y):(pR2->y);
	r =  (pR1->x+pR1->w < pR2->x+pR2->w)? (pR1->x+pR1->w):(pR2->x+pR2->w);
	b =  (pR1->y+pR1->h < pR2->y+pR2->h)? (pR1->y+pR1->h):(pR2->y+pR2->h);

	return (l<r && t<b);
}

S_BOOL hai_RectAdd(SP_RECT pInR1, SP_RECT pInR2, SP_RECT pOutR)
{
	S_SHORT l, t, r, b;
	
	if (!pInR1 || !pInR2 || !pOutR)
		return S_FALSE;
	l =  (pInR1->x < pInR2->x)? (pInR1->x):(pInR2->x);
	t =  (pInR1->y < pInR2->y)? (pInR1->y):(pInR2->y);
	r =  (pInR1->x+pInR1->w > pInR2->x+pInR2->w)? (pInR1->x+pInR1->w):(pInR2->x+pInR2->w);
	b =  (pInR1->y+pInR1->h > pInR2->y+pInR2->h)? (pInR1->y+pInR1->h):(pInR2->y+pInR2->h);

	pOutR->x = l;
	pOutR->y = t;
	pOutR->w = r-l;
	pOutR->h = b-t;

	return S_TRUE;
}

S_BOOL hai_IsPointInRect(S_WORD x, S_WORD y, SP_RECT pRect)
{
	if (x >= pRect->x && x < pRect->x+pRect->w 
			&& y >= pRect->y && y < pRect->y+pRect->h)
		return S_TRUE;
	return S_FALSE;
}

S_BOOL hai_IsRectCovered(SP_RECT prt1, SP_RECT prt2)
{
	if (prt1->x <= prt2->x && prt1->x+prt1->w >= prt2->x+prt2->w
			&& prt1->y <= prt2->y && prt1->y+prt1->h >= prt2->y+prt2->h)
		return S_TRUE;
	return S_FALSE;
}

