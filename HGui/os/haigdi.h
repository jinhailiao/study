//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haigdi.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef _HAIGDI_H_
#define _HAIGDI_H_

#include "haitype.h"
#include "haihandle.h"

typedef struct tagPen
{
	S_WORD  width;
	S_COLOR color;
	S_BYTE  style;
}
S_PEN, *SP_PEN;

typedef struct tagBrush
{
	S_BYTE  style;
	S_COLOR color;
	S_DWORD hatch;
}
S_BRUSH, *SP_BRUSH;

typedef struct tagFont
{
	S_BYTE  FontID;
	S_BYTE  style;
	S_WORD  width;
	S_WORD  height;
}
S_FONT, *SP_FONT;

typedef struct tagBitmap
{
	S_WORD  width;
	S_WORD  height;
	S_WORD  BitsPixel;
	S_WORD	WidthBytes;
	S_VOID  *lpImage;
}
S_BITMAP, *SP_BITMAP;

typedef struct tagPaintStruct
{
	SH_DC  hDC;
	S_RECT   rcPaint;
	S_BYTE CaretShow;
	S_BYTE reserved1;
	S_BYTE reserved2;
	S_BYTE reserved3;
}
S_PAINTSTRUCT, *SP_PAINTSTRUCT;



#ifdef HAI_16BITS_COLOR_REVERSION
#define  COLOR_GRAY00	0xF
#define  COLOR_GRAY01	0xE
#define  COLOR_GRAY02	0xD
#define  COLOR_GRAY03	0xC
#define  COLOR_GRAY04	0xB
#define  COLOR_GRAY05	0xA
#define  COLOR_GRAY06	0x9
#define  COLOR_GRAY07	0x8
#define  COLOR_GRAY08	0x7
#define  COLOR_GRAY09	0x6
#define  COLOR_GRAY10	0x5
#define  COLOR_GRAY11	0x4
#define  COLOR_GRAY12	0x3
#define  COLOR_GRAY13	0x2
#define  COLOR_GRAY14	0x1
#define  COLOR_GRAY15	0x0
#else
#define  COLOR_GRAY00	0x0
#define  COLOR_GRAY01	0x1
#define  COLOR_GRAY02	0x2
#define  COLOR_GRAY03	0x3
#define  COLOR_GRAY04	0x4
#define  COLOR_GRAY05	0x5
#define  COLOR_GRAY06	0x6
#define  COLOR_GRAY07	0x7
#define  COLOR_GRAY08	0x8
#define  COLOR_GRAY09	0x9
#define  COLOR_GRAY10	0xA
#define  COLOR_GRAY11	0xB
#define  COLOR_GRAY12	0xC
#define  COLOR_GRAY13	0xD
#define  COLOR_GRAY14	0xE
#define  COLOR_GRAY15	0xF
#endif

#define  COLOR_BLACK_8		0x00
#define  COLOR_WHITE_8		0xFF
#define  COLOR_BLACK_16	0x0000
#define  COLOR_WHITE_16	0xFFFF
#define  COLOR_BLACK_24	0x000000
#define  COLOR_WHITE_24	0xFFFFFF



#define  LCD_WIDTH		480//320
#define  LCD_HEIGHT		272//240

#define  BKM_OPAQUE			0
#define  BKM_TRANSPARENT	1

#define  PS_NONE		0
#define  PS_SOLID		1
#define  PS_DOT			2
#define  PS_DASH		3
#define  PS_DASHDOT		4
#define  PS_DASHDOTDOT	5
#define  PS_DASHDASHDOT 6

#define  FS_NORMAL    0
#define  FS_BOLD      1
#define  FS_ITALIC    2
#define  FS_TOPLINE   4
#define  FS_MIDLINE   8
#define  FS_UNDLINE   16
#define  FS_UNDDOT    32

enum
{
	BLACK_BRUSH,
	DKGRAY_BRUSH,
	GRAY_BRUSH,
	LTGRAY_BRUSH,
	NULL_BRUSH,
	WHITE_BRUSH,
	BLACK_PEN,
	WHITE_PEN,
	NULL_PEN,
	SYSTEM_FONT, 
	SYSTEM_FIXED_FONT,
	DEFAULT_PALETTE 
};

enum
{
	BLACKNESS,
	CAPTUREBLT,
	DSTINVERT,
	MERGECOPY,
	MERGEPAINT,
	NOMIRRORBITMAP,
	NOTSRCCOPY,
	NOTSRCERASE,
	PATCOPY,
	PATINVERT,
	PATPAINT,
	SRCAND,
	SRCCOPY,
	SRCERASE,
	SRCINVERT,
	SRCPAINT,
	WHITENESS,
	BRIGHTCOPY,
	ALPHACOPY,
	TRANSCOPY,
	TRANSALPHACPY
};

enum
{
	BS_DIBPATTERN,
	BS_DIBPATTERN8X8,
	BS_DIBPATTERNPT,
	BS_HATCHED,
	BS_HOLLOW,
	BS_NULL = BS_HOLLOW,
	BS_PATTERN,
	BS_PATTERN8X8,
	BS_SOLID
};

enum
{
	SCREEN_FLUSH_OFF,
	SCREEN_FLUSH_ON,
	SCREEN_FLUSH_GET = 0xFF
};


extern const S_COLOR gPalette4Bits[];
extern const S_COLOR gPalette8Bits[];


SH_GDIOBJ	hai_GetStockObject(S_BYTE ObjType);
S_VOID		_hai_InitGDISystem(S_VOID);
S_COLOR		hai_SetTextColor(SH_DC hDC, S_COLOR TextColor);
S_COLOR		hai_GetTextColor(SH_DC hDC);
S_COLOR		hai_SetBkColor(SH_DC hDC, S_COLOR bkcolor);
S_COLOR		hai_GetBkColor(SH_DC hDC);
S_BYTE		hai_SetBkMode(SH_DC hDC, S_BYTE BkMode);
S_BYTE		hai_GetBkMode(SH_DC hDC);
SH_DC		hai_GetScreenDC(S_VOID);
SH_DC		hai_GetSceneDC(S_VOID);
S_BOOL		hai_ReleaseDC(SH_DC hDC);
SH_GDIOBJ	hai_SelectObject(SH_DC hDC, SH_GDIOBJ hobj);
S_BOOL		hai_DeleteObject(SH_GDIOBJ hObj);
SH_DC		hai_CreateCompatibleDC(SH_DC hDC);
S_BOOL		hai_DeleteDC(SH_DC hDC);
S_COLOR		hai_SetPixel(SH_DC hDC, S_SHORT x, S_SHORT y, S_COLOR color);
S_COLOR		hai_GetPixel(SH_DC hDC, S_SHORT x, S_SHORT y);
S_BOOL		hai_MoveTo(SH_DC hDC,S_SHORT x, S_SHORT y, SP_POINT pPoint);
S_BOOL		hai_LineTo(SH_DC hDC, S_SHORT EX, S_SHORT EY);
SH_BITMAP	hai_LoadBitmap(S_BYTE *filename);
SH_BITMAP	hai_CreateCompatibleBitmap(SH_DC hDC);
SH_BITMAP	hai_CreateBitmap(S_WORD  w, S_WORD  h, S_WORD  BitsPixel, S_VOID  *lpImage);
SH_BITMAP	hai_CreateFormatBitmap(SH_DC hDC, SH_BITMAP hBmp);
S_BOOL		hai_StretchBlit(SH_DC hDst, S_WORD dx, S_WORD dy, S_WORD dw, S_WORD dh, SH_DC hSrc, S_WORD sx, S_WORD sy, S_WORD sw, S_WORD sh);
S_BOOL		hai_BitBlt(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc, S_DWORD dwRop);
SH_ICON		hai_LoadIcon(S_BYTE *filename);
SH_ICON		hai_CreateFormatIcon(SH_DC hDC, SH_ICON hIcon);
S_BOOL		hai_DrawIcon(SH_DC hDC, S_SHORT x, S_SHORT y, SH_ICON hIcon);
S_BOOL		hai_TextOut(SH_DC hDC, S_SHORT x, S_SHORT y, S_BYTE *lpString, S_SHORT cbstring);
S_BOOL		hai_Rectangle(SH_DC hDC, S_SHORT left, S_SHORT top, S_SHORT right, S_SHORT bottom);
SH_DC		hai_SceneDC2ClientDC(SH_DC hDC, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h);
SH_DC		hai_BeginPaint(SP_PAINTSTRUCT pPaint);
S_BOOL		hai_EndPaint(SP_PAINTSTRUCT pPaint);
S_WORD		hai_GetObject(SH_GDIOBJ hobj, S_WORD cbBuffer, S_VOID *pObject);
S_BYTE		hai_SetScreenFlush(S_BYTE flush);
 SH_PEN		hai_CreatePen(S_WORD  width, S_COLOR color, S_BYTE  style);
S_COLOR		hai_SetColorKey(SH_DC hDC, S_COLOR ColorKey);
S_COLOR		hai_GetColorKey(SH_DC hDC);
S_BYTE		hai_SetAlphaVal(SH_DC hDC, S_BYTE alpha);
S_BYTE		hai_GetAlphaVal(SH_DC hDC);
S_SHORT		hai_SetBrightVal(SH_DC hDC, S_SHORT bright);
S_SHORT		hai_GetBrightVal(SH_DC hDC);


#endif //_HAIGDI_H_
