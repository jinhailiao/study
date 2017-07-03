//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			_haigdi.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef __HAIGDI_H__
#define __HAIGDI_H__

#include "haitype.h"
#include "haihandle.h"


typedef struct _tagPen
{
	S_WORD  HdlType;

	S_WORD  width;
	S_COLOR color;
	S_BYTE  style;
}
_S_PEN, *_SP_PEN;

typedef struct _tagBrush
{
	S_WORD  HdlType;

	S_BYTE  style;
	S_COLOR color;
	S_DWORD hatch;
}
_S_BRUSH, *_SP_BRUSH;

typedef struct _tagFont
{
	S_WORD  HdlType;

	S_BYTE  FontID;
	S_BYTE  style;
	S_WORD  width;
	S_WORD  height;
}
_S_FONT, *_SP_FONT;

typedef struct _tagBitmap
{
	S_WORD  HdlType;

	S_WORD  width;
	S_WORD  height;
	S_WORD  BitsPixel;
	S_WORD	WidthBytes;
	S_VOID  *lpImage;
}
_S_BITMAP, *_SP_BITMAP;

typedef struct _tagIcon
{
	S_WORD  HdlType;

	S_WORD  width;
	S_WORD  height;
	S_WORD  BitsPixel;
	S_WORD	WidthBytes;
	S_VOID  *lpImage;
	S_VOID  *lpMask;
}
_S_ICON, *_SP_ICON;


typedef struct tagDC
{
	S_WORD   HdlType;

	S_SHORT  OrgX;
	S_SHORT  OrgY;
	S_SHORT  x;
	S_SHORT  y;
	S_SHORT  cx;
	S_SHORT  cy;
	S_BOOL   BkMode;
	S_BYTE   Alpha;
	S_COLOR  ColorKey;
	S_COLOR  TextColor;
	S_COLOR  BkColor;
	SH_PEN	 hPen;
	SH_FONT	 hFont;
	SH_BRUSH hBrush;
	SH_BITMAP hBitmap;
	S_VOID   *lpImage;
}
S_DC, *SP_DC;


S_VOID _hai_InitGDISystem(S_VOID);
S_BOOL _hai_UpdateRect(S_VOID);
S_VOID _hai_DrawHLine(SH_DC hDC, S_SHORT SX, S_SHORT EX, S_SHORT SY, S_COLOR c);
S_VOID _hai_DrawVLine(SH_DC hDC, S_SHORT SX, S_SHORT SY, S_SHORT EY, S_COLOR c);


#endif //_HAIGDI_H_

