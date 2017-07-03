//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haicaret.c
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
#include "haimem.h"
#include "_haigdi.h"
#include "haigdi.h"
#include "haicaret.h"
#include "_haiscene.h"

#define  CARET_COLOR		COLOR_BLACK
#define  CARET_WIDTH		6
#define  CARET_HEIGHT		1

typedef struct _tagCaret
{
	S_BYTE  fShow;
	S_BYTE  fBlink;
	S_BYTE  BlinkTime;
	S_BYTE  Reserved;
	S_SHORT x;
	S_SHORT y;
	S_SHORT SaveScrnX;
	S_SHORT SaveScrnY;
	S_WORD  width;
	S_WORD  height;
}
_S_CARET, *_SP_CARET;


HGui_SIZE_ASSERT(S_CARET, sizeof(S_CARET)>=sizeof(_S_CARET));


static _S_CARET caret;


S_VOID hai_InitCaret(S_VOID)
{
	memset(&caret, 0x00, sizeof(_S_CARET));
	caret.fBlink = 1;
	caret.height = CARET_HEIGHT;
	caret.width = CARET_WIDTH;
	caret.BlinkTime = 2;
}

static S_VOID _hai_InversetPixel(S_BYTE *pPixels, S_WORD num)
{
	while (num--)
	{
		*pPixels = ~(*pPixels);
		pPixels++;
	}
}

S_VOID  _hai_BlinkCaret(S_VOID)
{
	S_WORD i, ScrnX, ScrnY;
	S_BYTE *pDis;
	SH_DC hScrnDC;
	_SP_BITMAP pBmp;

	if ( caret.fShow )
	{
		hScrnDC = hai_GetSceneDC();
		pDis  = (S_BYTE *)(((SP_DC)hScrnDC)->lpImage);
		pBmp  = (_SP_BITMAP)(((SP_DC)hScrnDC)->hBitmap);
		if ( caret.fBlink )
		{
			caret.SaveScrnX = caret.x;
			caret.SaveScrnY = caret.y;
			ScrnX = caret.x + ((SP_DC)hScrnDC)->x;
			ScrnY = caret.y + ((SP_DC)hScrnDC)->y;

			pDis += (ScrnY+1-caret.height)*pBmp->WidthBytes + ScrnX*pBmp->BitsPixel/8;
			for (i = 0; i < caret.height; i++)
			{
				_hai_InversetPixel(pDis, (S_WORD)(caret.width*pBmp->BitsPixel/8));
				pDis += pBmp->WidthBytes;
			}
			caret.fBlink = 0;
			_hai_ObjectUpdateRect(caret.x, (S_WORD)(caret.y+1-caret.height), caret.width, caret.height);
		}
		else
		{
			ScrnX = caret.SaveScrnX + ((SP_DC)hScrnDC)->x;
			ScrnY = caret.SaveScrnY + ((SP_DC)hScrnDC)->y;
			
			pDis += (ScrnY+1-caret.height)*pBmp->WidthBytes + ScrnX*pBmp->BitsPixel/8;
			for (i = 0; i < caret.height; i++)
			{
				_hai_InversetPixel(pDis, (S_WORD)(caret.width*pBmp->BitsPixel/8));
				pDis += pBmp->WidthBytes;
			}
			caret.fBlink = 1;
			_hai_ObjectUpdateRect(caret.SaveScrnX, (S_WORD)(caret.SaveScrnY+1-caret.height), caret.width, caret.height);
		}
		hai_ReleaseDC(hScrnDC);
	}
}

S_BOOL  _hai_ShowCaret(S_VOID)
{
	if ( caret.fShow )
	{
		_hai_BlinkCaret();
	}
	
	return S_TRUE;
}

S_BOOL  _hai_HideCaret(S_VOID)
{
	S_BOOL fBlink = caret.fBlink;
	
	if ( caret.fShow )
	{
		if ( !caret.fBlink )
			_hai_BlinkCaret();
		return !fBlink;
	}
	
	return S_FALSE;
}


S_BOOL  hai_ShowCaret(S_VOID)
{
	S_BOOL fShow = caret.fShow;

	if ( !caret.fShow )
	{
		caret.fShow = 1;
		_hai_BlinkCaret();
	}

	return fShow;
}

S_BOOL  hai_HideCaret(S_VOID)
{
	S_BOOL fShow = caret.fShow;
	
	if ( caret.fShow )
	{
		if ( !caret.fBlink )
			_hai_BlinkCaret();
		caret.fShow = 0;
	}

	return fShow;
}

S_VOID  hai_GetCaretPos(S_SHORT *x, S_SHORT *y)
{
	*x = caret.x;
	*y = caret.y;
}

S_VOID  hai_SetCaretPos(S_SHORT x, S_SHORT y)
{
	S_WORD CaretH, CaretW, ClientH, ClientW;
	SH_DC hScrnDC;

	if (y < 0 || x < 0)
		return;

	hScrnDC = hai_GetSceneDC();
	ClientH = ((SP_DC)hScrnDC)->cy;
	ClientW = ((SP_DC)hScrnDC)->cx;
	CaretH   = caret.height;
	CaretW  = caret.width;
	hai_ReleaseDC(hScrnDC);
	if (y+1 < CaretH || y >= ClientH || x+CaretW >= ClientW)
		return;

	caret.x = x;
	caret.y = y;

	if ( !caret.fShow )
		return;

	if ( !caret.fBlink )
		_hai_BlinkCaret();
	_hai_BlinkCaret();
}

S_VOID  hai_SetCaretWidthHeight(S_WORD w, S_WORD h)
{
	S_WORD BmpH, BmpW;
	SH_DC hScrnDC;
	_SP_BITMAP pBmp;

	if (w == 0 || h == 0)
		return;

	hScrnDC = hai_GetSceneDC();
	pBmp    = (_SP_BITMAP)(((SP_DC)hScrnDC)->hBitmap);
	BmpW   = pBmp->width;
	BmpH    = pBmp->height;
	hai_ReleaseDC(hScrnDC);
	if (h >= (S_WORD)caret.y || caret.x+w >= BmpW)
		return;

	caret.width= w;
	caret.height= h;

	if ( !caret.fShow )
		return;

	if ( !caret.fBlink )
		_hai_BlinkCaret();
	_hai_BlinkCaret();
}

S_VOID  hai_SetCaretBlinkTime(S_BYTE  TenthSecond)
{
	if (TenthSecond)
		caret.BlinkTime = TenthSecond;
}

S_BYTE  hai_GetCaretBlinkTime(S_VOID)
{
	return caret.BlinkTime;
}

S_WORD HG_SaveCurCaret(S_VOID *Buffer, S_WORD BufSize)
{
	if (BufSize >= sizeof(_S_CARET))
		memcpy(Buffer,  &caret, sizeof(_S_CARET));

	return sizeof(_S_CARET);
}

S_VOID HG_RestoreCurCaret(S_VOID *Buffer)
{
	memcpy(&caret, Buffer, sizeof(_S_CARET));
}

