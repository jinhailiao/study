//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haiGDI.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#include "haitype.h"
#include "_haigdi.h"
#include "haigdi.h"
#include "haihandle.h"
#include "haimem.h"
#include "hailib.h"
#include "hzall.h"
#include "_haiscene.h"
#include "haiscene.h"
#include "_haictrl.h"
#include "haictrl.h"
#include "haigui.h"
#include "haicaret.h"
#include "haipic.h"
#include "haifile.h"


#define COPYPIXEL_24(pBmp, Color) *pBmp=(S_BYTE)Color,*(pBmp+1)=(S_BYTE)(Color>>8),*(pBmp+2)=(S_BYTE)(Color>>16)

extern S_BYTE resource[];


S_BYTE *pLcdDisRam = S_NULL;
S_DWORD LogDisRAM[(BITMAP_WIDTHBYTES(LCD_WIDTH, LCD_BITSPERPIX)*LCD_HEIGHT)/4];
//for BITSPERPIX == 4
S_BYTE	gFontMat[2][4]  = {0xFF, 0xF0, 0x0F, 0x00, 0xFF, 0xF0, 0x0F, 0x00};
S_BYTE  gFontMask[2][4] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xF0, 0x0F, 0x00};

static _S_PEN		_gPen;
static _S_BRUSH	_gBrush;
static _S_FONT	_gFont;
static _S_BITMAP	_gBitmap;

static S_BYTE gfScrnFlush = SCREEN_FLUSH_ON;

S_BYTE hai_SetScreenFlush(S_BYTE flush)
{
	S_BYTE old = gfScrnFlush;

	if (flush != SCREEN_FLUSH_GET)
		gfScrnFlush = flush;

	return old;
}

#ifdef PCSIM
S_VOID Ext_memcpy(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
#if (LCD_BITSPERPIX == 4)
	S_WORD i;
	S_COLOR c;
	S_WORD odd = nPixel&0x1;
	
	nPixel &= 0xFFFE;
	for (i = 0; i < nPixel; i += 2)
	{
		c = gPalette4Bits[*pSrc>>4];
		*pDst++ = (S_BYTE)c;
		*pDst++ = (S_BYTE)(c>>8);
		*pDst++ = (S_BYTE)(c>>16);

		c = gPalette4Bits[*pSrc&0xF];
		*pDst++ = (S_BYTE)c;
		*pDst++ = (S_BYTE)(c>>8);
		*pDst++ = (S_BYTE)(c>>16);

		pSrc++;
	}
	if (odd)
	{
		c = gPalette4Bits[*pSrc>>4];
		*pDst++ = (S_BYTE)c;
		*pDst++ = (S_BYTE)(c>>8);
		*pDst++ = (S_BYTE)(c>>16);
	}
#elif (LCD_BITSPERPIX == 8)
	S_WORD i;
	S_COLOR c;
	
	for (i = 0; i < nPixel; i++)
	{
		c = gPalette8Bits[*pSrc];
		*pDst++ = (S_BYTE)c;
		*pDst++ = (S_BYTE)(c>>8);
		*pDst++ = (S_BYTE)(c>>16);
		pSrc++;
	}
#elif (LCD_BITSPERPIX == 16)
	S_WORD i;
	S_WORD pixel;

	for (i = 0; i < nPixel; i++)
	{
		pixel = *(unsigned short *)pSrc;
		pSrc += 2;
		*pDst++ = pixel<<3;
		*pDst++ = (pixel>>5)<<2;
		*pDst++ = (pixel>>11)<<3;
	}
#elif (LCD_BITSPERPIX == 24)
	memcpy(pDst, pSrc, nPixel*3);
#else
	#error Unsupport color depth!
#endif
}

S_BOOL _hai_UpdateRect(S_VOID)
{
	S_WORD i, x, y, w, h;
	S_BYTE *pSrc, *pDst;
	SP_RECT pRect;
	
	if (hai_IsNeedRedraw() && hai_SetScreenFlush(SCREEN_FLUSH_GET))
	{
		pRect = hai_GetInvalidRect();
		x = pRect->x, y = pRect->y;
		w = pRect->w, h = pRect->h;
		pSrc = (S_BYTE *)LogDisRAM+y*BITMAP_WIDTHBYTES(LCD_WIDTH,LCD_BITSPERPIX)+x*LCD_BITSPERPIX/8;
		pDst = (S_BYTE *)pLcdDisRam+y*BITMAP_WIDTHBYTES(LCD_WIDTH,24)+x*24/8;
		for (i = 0; i < h; i++)
		{
			Ext_memcpy(pDst, pSrc, w);
			pSrc += BITMAP_WIDTHBYTES(LCD_WIDTH,LCD_BITSPERPIX);
			pDst += BITMAP_WIDTHBYTES(LCD_WIDTH,24);
		}
		memset(pRect, 0x00, sizeof(*pRect));
		return S_TRUE;
	}
	return S_FALSE;
}
#else
S_BOOL _hai_UpdateRect(S_VOID)
{
	S_WORD i, x, y, w, h;
	S_BYTE *pSrc, *pDst;
	SP_RECT pRect;
	
	if (hai_IsNeedRedraw() && hai_SetScreenFlush(SCREEN_FLUSH_GET) == SCREEN_FLUSH_ON)
	{
		pRect = hai_GetInvalidRect();
		x = pRect->x, y = pRect->y;
		w = pRect->w, h = pRect->h;
		pSrc = (S_BYTE *)LogDisRAM+y*BITMAP_WIDTHBYTES(LCD_WIDTH,LCD_BITSPERPIX)+x*LCD_BITSPERPIX/8;
		pDst = (S_BYTE *)pLcdDisRam+y*BITMAP_WIDTHBYTES(LCD_WIDTH,LCD_BITSPERPIX)+x*LCD_BITSPERPIX/8;
		for (i = 0; i < h; i++)
		{
			memcpy(pDst, pSrc, w*LCD_BITSPERPIX/8);
			pSrc += BITMAP_WIDTHBYTES(LCD_WIDTH,LCD_BITSPERPIX);
			pDst += BITMAP_WIDTHBYTES(LCD_WIDTH,LCD_BITSPERPIX);
		}
		memset(pRect, 0x00, sizeof(*pRect));
		return S_TRUE;
	}
	return S_FALSE;
}
#endif

SH_GDIOBJ hai_GetStockObject(S_BYTE ObjType)
{
	switch (ObjType)
	{
	case BLACK_BRUSH:
		_gBrush.HdlType = HT_BRUSH;
		_gBrush.style   = BS_SOLID;
		_gBrush.color   = COLOR_BLACK;
		_gBrush.hatch   = 0;
		return (SH_GDIOBJ)&_gBrush;

	case DKGRAY_BRUSH:
		_gBrush.HdlType = HT_BRUSH;
		_gBrush.style   = BS_SOLID;
		_gBrush.color   = COLOR_DARK;
		_gBrush.hatch   = 0;
		return (SH_GDIOBJ)&_gBrush;

	case GRAY_BRUSH:
		_gBrush.HdlType = HT_BRUSH;
		_gBrush.style   = BS_SOLID;
		_gBrush.color   = COLOR_GRAY;
		_gBrush.hatch   = 0;
		return (SH_GDIOBJ)&_gBrush;

	case LTGRAY_BRUSH:
		_gBrush.HdlType = HT_BRUSH;
		_gBrush.style   = BS_SOLID;
		_gBrush.color   = COLOR_LIGHT;
		_gBrush.hatch   = 0;
		return (SH_GDIOBJ)&_gBrush;

	case WHITE_BRUSH:
		_gBrush.HdlType = HT_BRUSH;
		_gBrush.style   = BS_SOLID;
		_gBrush.color   = COLOR_WHITE;
		_gBrush.hatch   = 0;
		return (SH_GDIOBJ)&_gBrush;

	case NULL_BRUSH:
		_gBrush.HdlType = HT_BRUSH;
		_gBrush.style   = BS_NULL;
		_gBrush.color   = COLOR_BLACK;
		_gBrush.hatch   = 0;
		return (SH_GDIOBJ)&_gBrush;

	case BLACK_PEN:
		_gPen.HdlType	= HT_PEN;
		_gPen.color		= COLOR_BLACK;
		_gPen.width		= 1;
		_gPen.style		= PS_SOLID;
		return (SH_GDIOBJ)&_gPen;

	case WHITE_PEN:
		_gPen.HdlType	= HT_PEN;
		_gPen.color		= COLOR_WHITE;
		_gPen.width		= 1;
		_gPen.style		= PS_SOLID;
		return (SH_GDIOBJ)&_gPen;

	case NULL_PEN:
		_gPen.HdlType	= HT_PEN;
		_gPen.color		= COLOR_BLACK;
		_gPen.width		= 1;
		_gPen.style		= PS_NONE;
		return (SH_GDIOBJ)&_gPen;

	case SYSTEM_FONT:
		_gFont.HdlType = HT_FONT;
		_gFont.FontID  = 1;
		_gFont.height   = 12;
		_gFont.width   = 6;
		_gFont.style   = 0;
		return (SH_GDIOBJ)&_gFont;

	case SYSTEM_FIXED_FONT:
		_gFont.HdlType = HT_FONT;
		_gFont.FontID  = 2;
		_gFont.height   = 12;
		_gFont.width   = 6;
		_gFont.style   = 0;
		return (SH_GDIOBJ)&_gFont;

	case DEFAULT_PALETTE:

	default:
		return S_NULL;
	}
}

S_VOID _hai_InitGDISystem(S_VOID)
{
//for BITSPERPIX == 4
	gFontMat[0][0] = 0xFF;
	gFontMat[0][1] = 0xF0;
	gFontMat[0][2] = 0x0F;
	gFontMat[0][3] = 0x00;
	gFontMat[1][0] = 0x00;
	gFontMat[1][1] = 0x00;
	gFontMat[1][2] = 0x00;
	gFontMat[1][3] = 0x00;

	gFontMask[0][0] = 0x00;
	gFontMask[0][1] = 0x00;
	gFontMask[0][2] = 0x00;
	gFontMask[0][3] = 0x00;
	gFontMask[1][0] = 0xFF;
	gFontMask[1][1] = 0xF0;
	gFontMask[1][2] = 0x0F;
	gFontMask[1][3] = 0x00;
}

S_COLOR hai_SetTextColor(SH_DC hDC, S_COLOR TextColor)
{
	S_COLOR CurColor;
	_SP_BITMAP pbmp;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x00;
	pbmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;
	if (!pbmp)
		return 0x00;

	CurColor  = ((SP_DC)hDC)->TextColor;
	switch (pbmp->BitsPixel)
	{
	case 4:
		((SP_DC)hDC)->TextColor = TextColor&0xF;
		break;
	case 8:
		((SP_DC)hDC)->TextColor = TextColor&0xFF;
		break;
	case 16:
		((SP_DC)hDC)->TextColor = TextColor&0xFFFF;
		break;
	case 24:
		((SP_DC)hDC)->TextColor = TextColor&0xFFFFFF;
		break;
	default:
		break;
	}
	if (pbmp->BitsPixel == 4)
	{
		gFontMat[0][0] = (S_BYTE)( ((((SP_DC)hDC)->BkColor)<<4) | ((SP_DC)hDC)->BkColor );
		gFontMat[0][1] = (S_BYTE)( ((((SP_DC)hDC)->BkColor)<<4) | ((SP_DC)hDC)->TextColor );
		gFontMat[0][2] = (S_BYTE)( ((((SP_DC)hDC)->TextColor)<<4) | ((SP_DC)hDC)->BkColor );
		gFontMat[0][3] = (S_BYTE)( ((((SP_DC)hDC)->TextColor)<<4) | ((SP_DC)hDC)->TextColor );
		gFontMat[1][0] = (S_BYTE)( 0x00 );
		gFontMat[1][1] = (S_BYTE)( ((SP_DC)hDC)->TextColor );
		gFontMat[1][2] = (S_BYTE)( ((((SP_DC)hDC)->TextColor)<<4) );
		gFontMat[1][3] = (S_BYTE)( ((((SP_DC)hDC)->TextColor)<<4) | ((SP_DC)hDC)->TextColor );
	}
	return CurColor;
}

S_COLOR hai_GetTextColor(SH_DC hDC)
{
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x0;
	return (((SP_DC)hDC)->TextColor);
}

S_COLOR hai_SetColorKey(SH_DC hDC, S_COLOR ColorKey)
{
	S_COLOR CurColor;
	_SP_BITMAP pbmp;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x00;
	pbmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;
	if (!pbmp)
		return 0x00;

	CurColor  = ((SP_DC)hDC)->ColorKey;
	switch (pbmp->BitsPixel)
	{
	case 4:
		((SP_DC)hDC)->ColorKey = ColorKey&0xF;
		break;
	case 8:
		((SP_DC)hDC)->ColorKey = ColorKey&0xFF;
		break;
	case 16:
		((SP_DC)hDC)->ColorKey = ColorKey&0xFFFF;
		break;
	case 24:
		((SP_DC)hDC)->ColorKey = ColorKey&0xFFFFFF;
		break;
	default:
		break;
	}
	return CurColor;
}

S_COLOR hai_GetColorKey(SH_DC hDC)
{
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x0;
	return (((SP_DC)hDC)->ColorKey);
}

S_BYTE hai_SetAlphaVal(SH_DC hDC, S_BYTE alpha)
{
	S_BYTE CurAlpha;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x00;

	CurAlpha  = ((SP_DC)hDC)->Alpha;
	((SP_DC)hDC)->Alpha = alpha;
		
	return CurAlpha;
}

S_BYTE hai_GetAlphaVal(SH_DC hDC)
{
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x0;
	return (((SP_DC)hDC)->Alpha);
}

S_SHORT hai_SetBrightVal(SH_DC hDC, S_SHORT bright)
{
	S_SHORT CurBright;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x00;

	CurBright  = (S_SHORT)((SP_DC)hDC)->ColorKey;
	((SP_DC)hDC)->ColorKey = (S_COLOR)bright;
		
	return CurBright;
}

S_SHORT hai_GetBrightVal(SH_DC hDC)
{
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x0;
	return (S_SHORT)(((SP_DC)hDC)->ColorKey);
}

S_COLOR hai_SetBkColor(SH_DC hDC, S_COLOR BkColor)
{
	S_COLOR CurColor;
	_SP_BITMAP pbmp;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x00;
	pbmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;
	if (!pbmp)
		return 0x00;
	
	CurColor = ((SP_DC)hDC)->BkColor;
	switch (pbmp->BitsPixel)
	{
	case 4:
		((SP_DC)hDC)->BkColor = BkColor&0xF;
		break;
	case 8:
		((SP_DC)hDC)->BkColor = BkColor&0xFF;
		break;
	case 16:
		((SP_DC)hDC)->BkColor = BkColor&0xFFFF;
		break;
	case 24:
		((SP_DC)hDC)->BkColor = BkColor&0xFFFFFF;
		break;
	default:
		break;
	}

	if (pbmp->BitsPixel == 4)
	{
		gFontMat[0][0] = (S_BYTE)( ((((SP_DC)hDC)->BkColor)<<4) | ((SP_DC)hDC)->BkColor );
		gFontMat[0][1] = (S_BYTE)( ((((SP_DC)hDC)->BkColor)<<4) | ((SP_DC)hDC)->TextColor );
		gFontMat[0][2] = (S_BYTE)( ((((SP_DC)hDC)->TextColor)<<4) | ((SP_DC)hDC)->BkColor );
		gFontMat[0][3] = (S_BYTE)( ((((SP_DC)hDC)->TextColor)<<4) | ((SP_DC)hDC)->TextColor );
	}
	return CurColor;
}

S_COLOR hai_GetBkColor(SH_DC hDC)
{
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x0;
	return (((SP_DC)hDC)->BkColor);
}

S_BYTE hai_SetBkMode(SH_DC hDC, S_BYTE BkMode)
{
	S_BYTE mode;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0xFF;
	mode = ((SP_DC)hDC)->BkMode;
	((SP_DC)hDC)->BkMode = !(!BkMode);

	return mode;
}

S_BYTE hai_GetBkMode(SH_DC hDC)
{
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0xFF;
	return ((SP_DC)hDC)->BkMode;
}

S_VOID _ResetDC(SP_DC pDC)
{
	HAI_SETHDLTYPE(&_gBitmap, HT_BITMAP);
	_gBitmap.width      = LCD_WIDTH;
	_gBitmap.height      = LCD_HEIGHT;
	_gBitmap.BitsPixel  = LCD_BITSPERPIX;
	_gBitmap.WidthBytes = BITMAP_WIDTHBYTES(LCD_WIDTH, LCD_BITSPERPIX);
	_gBitmap.lpImage    = LogDisRAM;
	
	pDC->OrgX		= 0;
	pDC->OrgY		= 0;
	pDC->x			= 0;
	pDC->y			= 0;
	pDC->cx			= LCD_WIDTH;
	pDC->cy			= LCD_HEIGHT;
	pDC->ColorKey	= 0;
	pDC->TextColor	= COLOR_BLACK;
	pDC->BkColor	= COLOR_WHITE;
	pDC->BkMode	= BKM_OPAQUE;
	pDC->Alpha		= 255;
	pDC->hPen		= (SH_PEN)hai_GetStockObject(BLACK_PEN);
	pDC->hFont		= (SH_FONT)hai_GetStockObject(SYSTEM_FONT);
	pDC->hBrush	= (SH_BRUSH)hai_GetStockObject(WHITE_BRUSH);
	pDC->hBitmap	= (SH_BITMAP)&_gBitmap;
	pDC->lpImage	= LogDisRAM;
}

SH_DC hai_GetScreenDC(S_VOID)
{
	SP_DC pDC;

	pDC = (SP_DC)hai_MemAlloc(sizeof(S_DC));
	if (pDC)
	{
		HAI_SETHDLTYPE(pDC, HT_DC);
		_ResetDC(pDC);
	}

	return (SH_DC)pDC;
}

SH_DC hai_GetSceneDC(S_VOID)
{
	SP_DC pDC = (SP_DC)hai_GetScreenDC();

	if (pDC)
		hai_GetSceneRect(&pDC->x, &pDC->y, &pDC->cx, &pDC->cy);

	return (SH_DC)pDC;
}

S_BOOL hai_ReleaseDC(SH_DC hDC)
{
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_FALSE;
	return hai_MemFree(hDC);
}

SH_DC hai_SceneDC2ClientDC(SH_DC hDC, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h)
{
	_SP_BITMAP pbmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;
	
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_NULL;
	if (!pbmp)
		return S_NULL;
	
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= pbmp->width) x = pbmp->width - 1;
	if (y >= pbmp->height) y = pbmp->height - 1;
	if (w <= 0 || w > pbmp->width-x) w = pbmp->width - x;
	if (h <= 0 || h > pbmp->height-y) h = pbmp->height - y;

	((SP_DC)hDC)->x = x;
	((SP_DC)hDC)->y = y;
	((SP_DC)hDC)->cx = w;
	((SP_DC)hDC)->cy = h;

	return hDC;
}

SH_GDIOBJ hai_SelectObject(SH_DC hDC, SH_GDIOBJ hobj)
{
	SH_GDIOBJ htmp;

	if (!hobj || !hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_NULL;

	switch (HAI_GETHDLTYPE(hobj))
	{
	case HT_PEN:
		htmp = (SH_GDIOBJ)(((SP_DC)hDC)->hPen);
		((SP_DC)hDC)->hPen = (SH_PEN)hobj;
		return htmp;

	case HT_BITMAP:
		htmp = (SH_GDIOBJ)(((SP_DC)hDC)->hBitmap);
		((SP_DC)hDC)->hBitmap = (SH_BITMAP)hobj;
		((SP_DC)hDC)->lpImage = ((_SP_BITMAP)hobj)->lpImage;
		((SP_DC)hDC)->OrgX = 0;
		((SP_DC)hDC)->OrgY = 0;
		((SP_DC)hDC)->x = 0;
		((SP_DC)hDC)->y = 0;
		((SP_DC)hDC)->cx = ((_SP_BITMAP)hobj)->width;
		((SP_DC)hDC)->cy = ((_SP_BITMAP)hobj)->height;
		((SP_DC)hDC)->ColorKey = 0;
		((SP_DC)hDC)->TextColor = 0;
		((SP_DC)hDC)->BkColor = ((0xFFFFFFU)<<(sizeof(S_COLOR)-((_SP_BITMAP)hobj)->BitsPixel))>>(sizeof(S_COLOR)-((_SP_BITMAP)hobj)->BitsPixel);
		return htmp;

	case HT_BRUSH:
		htmp = (SH_GDIOBJ)(((SP_DC)hDC)->hBrush);
		((SP_DC)hDC)->hBrush = (SH_BRUSH)hobj;
		return htmp;

	case HT_FONT:
		htmp = (SH_GDIOBJ)(((SP_DC)hDC)->hFont);
		((SP_DC)hDC)->hFont = (SH_FONT)hobj;
		return htmp;

	default:
		return S_NULL;
	}
}

S_WORD hai_GetObject(SH_GDIOBJ hobj, S_WORD cbBuffer, S_VOID *pObject)
{
	if (!hobj)
		return 0;

	switch (HAI_GETHDLTYPE(hobj))
	{
	case HT_PEN:
		if (pObject && cbBuffer >= sizeof(S_PEN))
		{
			_SP_PEN pobj = (_SP_PEN)hobj;
			SP_PEN pPen  = (SP_PEN)pObject;
			pPen->width = pobj->width;
			pPen->color = pobj->color;
			pPen->style = pobj->style;
		}
		return sizeof(S_PEN);

	case HT_BITMAP:
		if (pObject && cbBuffer >= sizeof(S_BITMAP))
		{
			_SP_BITMAP pobj = (_SP_BITMAP)hobj;
			SP_BITMAP pBmp  = (SP_BITMAP)pObject;
			pBmp->width = pobj->width;
			pBmp->height = pobj->height;
			pBmp->BitsPixel = pobj->BitsPixel;
			pBmp->WidthBytes = pobj->WidthBytes;
			pBmp->lpImage = pobj->lpImage;
		}
		return sizeof(S_BITMAP);

	case HT_BRUSH:
		if (pObject && cbBuffer >= sizeof(S_BRUSH))
		{
			_SP_BRUSH pobj = (_SP_BRUSH)hobj;
			SP_BRUSH pBrush  = (SP_BRUSH)pObject;
			pBrush->style = pobj->style;
			pBrush->color = pobj->color;
			pBrush->hatch = pobj->hatch;
		}
		return sizeof(S_BRUSH);

	case HT_FONT:
		if (pObject && cbBuffer >= sizeof(S_FONT))
		{
			_SP_FONT pobj = (_SP_FONT)hobj;
			SP_FONT pFont  = (SP_FONT)pObject;
			pFont->FontID = pobj->FontID;
			pFont->style = pobj->style;
			pFont->width = pobj->width;
			pFont->height = pobj->height;
		}
		return sizeof(S_FONT);

	case HT_ICON:
		if (pObject && cbBuffer >= sizeof(S_FONT))
		{
			_SP_ICON pobj = (_SP_ICON)hobj;
			SP_ICON pIcon  = (SP_ICON)pObject;
			pIcon->width = pobj->width;
			pIcon->height = pobj->height;
			pIcon->BitsPixel = pobj->BitsPixel;
			pIcon->WidthBytes = pobj->WidthBytes;
			pIcon->lpImage = pobj->lpImage;
			pIcon->lpMask = pobj->lpMask;
		}
		return sizeof(S_ICON);
	default:
		return 0;
	}
	return 0;
}

S_BOOL hai_DeleteObject(SH_GDIOBJ hObj)
{
	if (hObj == (SH_GDIOBJ)&_gPen || hObj == (SH_GDIOBJ)&_gBrush 
		|| hObj == (SH_GDIOBJ)&_gFont || hObj == (SH_GDIOBJ)&_gBitmap)
		return S_FALSE;
	return hai_MemFree(hObj);
}

SH_DC  hai_CreateCompatibleDC(SH_DC hDC)
{
	SP_DC pDC;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_NULL;

	pDC = (SP_DC)hai_MemAlloc(sizeof(S_DC));
	if (!pDC)
		return S_NULL;

	*pDC = *((SP_DC)hDC);
//	pDC->lpImage = S_NULL;
	
	return (SH_DC)pDC;
}

S_BOOL	hai_DeleteDC(SH_DC hDC)
{
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_FALSE;
	return hai_MemFree(hDC);
}

SH_DC  hai_BeginPaint(SP_PAINTSTRUCT pPaint)
{
	SH_DC hDC = hai_GetSceneDC();
	SP_RECT pRect = hai_GetInvalidRect();

	pPaint->hDC = hDC;
	pPaint->rcPaint.x = pRect->x;
	pPaint->rcPaint.y = pRect->y;
	pPaint->rcPaint.w = pRect->w;
	pPaint->rcPaint.h = pRect->h;
	pPaint->CaretShow = _hai_HideCaret();//hai_HideCaret();

	return hDC;
}

S_BOOL  hai_EndPaint(SP_PAINTSTRUCT pPaint)
{
	SP_SCENE pScene = hai_GetCurSceneAddr();
	SP_CTRL pCtrl = hai_GetCurSceneCtrlListEnd();

	hai_ReleaseDC(pPaint->hDC);
	while (pCtrl)
	{
		pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_PAINT, 0, 0);
		pCtrl = pCtrl->prev;
	}
	if (pCtrl = pScene->pInputMethod)
		pCtrl->CtrlProc((SH_CTRL)pCtrl, EVT_PAINT, 0, 0);
	if (pPaint->CaretShow)
		_hai_ShowCaret();//hai_ShowCaret();
	_hai_UpdateRect();

	return S_TRUE;
}

S_COLOR hai_SetPixel(SH_DC hDC, S_SHORT x, S_SHORT y, S_COLOR color)
{
	S_COLOR c;
	_SP_BITMAP pbmp;
	S_BYTE *pImage;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x0;
	pbmp = (_SP_BITMAP)(((SP_DC)hDC)->hBitmap);
	pImage = (S_BYTE *)(((SP_DC)hDC)->lpImage);
	if (!pImage || !pbmp)
		return 0x00;
	if (x < 0 || x >= ((SP_DC)hDC)->cx || y < 0 || y >= ((SP_DC)hDC)->cy)
		return 0x00;

	x += ((SP_DC)hDC)->x;//conver absolute
	y += ((SP_DC)hDC)->y;

	switch (pbmp->BitsPixel)
	{
	case 4:
		color &= 0xF;
		pImage += y * pbmp->WidthBytes+ (x>>1);
		c = (S_COLOR)*pImage;
		*pImage = (S_BYTE)((x&0x1) ? (c&0xF0 | color) : (c&0xF | color<<4));
		c = (S_COLOR)((x&0x1) ? (c&0xF) : (c>>4&0xF));
		break;
	case 8:
		color &= 0xFF;
		pImage += y * pbmp->WidthBytes+ x;
		c = (S_COLOR)*pImage;
		*pImage = (S_BYTE)color;
		break;
	case 16:
		color &= 0xFFFF;
		pImage += y * pbmp->WidthBytes+ (x<<1);
		c = (S_COLOR)*(S_WORD *)pImage;
		*(S_WORD *)pImage = (S_WORD)color;
		break;
	case 24:
		color &= 0xFFFFFF;
		pImage += y * pbmp->WidthBytes+ x*3;
		c = (S_COLOR)(*pImage | (*(pImage+1)<<8) | (*(pImage+2)<<16));
		*(pImage+0) = (S_BYTE)color;
		*(pImage+1) = (S_BYTE)(color>>8);
		*(pImage+1) = (S_BYTE)(color>>16);
		break;
	default:
		c = 0x00;
		break;
	}
	return c;
}

S_COLOR hai_GetPixel(SH_DC hDC, S_SHORT x, S_SHORT y)
{
	S_COLOR c;
	_SP_BITMAP pbmp;
	S_BYTE *pImage;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return 0x00;
	pbmp = (_SP_BITMAP)(((SP_DC)hDC)->hBitmap);
	pImage = (S_BYTE *)(((SP_DC)hDC)->lpImage);
	if (!pImage || !pbmp)
		return 0x00;
	if (x < 0 || x >= ((SP_DC)hDC)->cx || y < 0 || y >= ((SP_DC)hDC)->cy)
		return 0x00;

	x += ((SP_DC)hDC)->x;
	y += ((SP_DC)hDC)->y;

	
	switch (pbmp->BitsPixel)
	{
	case 4:
		c = (S_COLOR)*(pImage + y * pbmp->WidthBytes + (x>>1));
		c = (x&0x1) ? (c&0xF) : (c>>4&0xF);
		break;
	case 8:
		c = (S_COLOR)*(pImage + y * pbmp->WidthBytes + x);
		break;
	case 16:
		pImage += y * pbmp->WidthBytes+ (x<<1);
		c = (S_COLOR)*(S_WORD *)pImage;
		break;
	case 24:
		pImage += y * pbmp->WidthBytes+ x*3;
		c = (S_COLOR)(*pImage | (*(pImage+1)<<8) | (*(pImage+2)<<16));
		break;
	default:
		c = 0x00;
		break;
	}
	
	return c;
}

S_BOOL hai_MoveTo(SH_DC hDC,S_SHORT x, S_SHORT y, SP_POINT pPoint)
{
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_FALSE;
	if (x < 0 || x >= ((SP_DC)hDC)->cx || y < 0 || y >= ((SP_DC)hDC)->cy)
		return S_FALSE;

	if (pPoint)
	{
		pPoint->x = ((SP_DC)hDC)->OrgX;
		pPoint->y = ((SP_DC)hDC)->OrgY;
	}
	((SP_DC)hDC)->OrgX = x;
	((SP_DC)hDC)->OrgY = y;

	return S_TRUE;
}

#define HAI_DRAWPIXEL(pImage, x, y, bits, width, color, color4b)   \
{\
	S_BYTE *pTemp;\
	switch (bits)\
	{\
	case 4:\
		pTemp = pImage+y*width+(x>>1);\
		*pTemp = *pTemp&mask[x&0x1] | color4b[x&0x1];\
		break;\
	case 8:\
		*(pImage+y*width+x) = (S_BYTE)color;\
		break;\
	case 16:\
		*((S_WORD*)(pImage+y*width+(x<<1))) = (S_WORD)color;\
		break;\
	case 24:\
		pTemp = pImage+y*width+x*3;\
		*(pTemp+0) = (S_BYTE)color;\
		*(pTemp+1) = (S_BYTE)(color>>8);\
		*(pTemp+2) = (S_BYTE)(color>>16);\
		break;\
	default:\
		break;\
	}\
}

S_BOOL hai_LineTo(SH_DC hDC, S_SHORT EX, S_SHORT EY)
{
	S_BYTE  style;
	S_BYTE  mode;
	S_SHORT W,H,Ha,dY,f;
	S_SHORT SX, SY;
	S_BYTE  fgcolor4bit[2], bkcolor4bit[2], mask[2] = {0xF, 0xF0};
	S_WORD BitsPixel, BmpWidth;
	S_COLOR fgcolor, bkcolor;
	S_BYTE  *pDis;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_FALSE;
	if (!((SP_DC)hDC)->lpImage || !((SP_DC)hDC)->hBitmap)
		return S_FALSE;

	if (EX < 0) EX = 0;//revise coordinate
	if (EX >= ((SP_DC)hDC)->cx) EX = ((SP_DC)hDC)->cx-1;
	if (EY < 0) EY = 0;
	if (EY >= ((SP_DC)hDC)->cy) EY = ((SP_DC)hDC)->cy-1;

	SX = ((SP_DC)hDC)->OrgX + ((SP_DC)hDC)->x;//client to screen
	SY = ((SP_DC)hDC)->OrgY + ((SP_DC)hDC)->y;
	((SP_DC)hDC)->OrgX = EX;
	((SP_DC)hDC)->OrgY = EY;

	EX = EX + ((SP_DC)hDC)->x;//client to screen
	EY = EY + ((SP_DC)hDC)->y;

	style = ((_SP_PEN)(((SP_DC)hDC)->hPen))->style;
	mode  = ((SP_DC)hDC)->BkMode;
	pDis    = (S_BYTE*)(((SP_DC)hDC)->lpImage);
	BmpWidth = ((_SP_BITMAP)(((SP_DC)hDC)->hBitmap))->WidthBytes;
	BitsPixel = ((_SP_BITMAP)(((SP_DC)hDC)->hBitmap))->BitsPixel;
	fgcolor = ((_SP_PEN)(((SP_DC)hDC)->hPen))->color;
	bkcolor = ((SP_DC)hDC)->BkColor;
	fgcolor4bit[1] = (S_BYTE)fgcolor;
	fgcolor4bit[0] = fgcolor4bit[1]<<4;
	bkcolor4bit[1] = (S_BYTE)bkcolor;
	bkcolor4bit[0] = bkcolor4bit[1]<<4;

	if (SX>EX)
	{
		f=EX, EX=SX, SX=f;
		f=EY, EY=SY, SY=f;
	}

	W = (EX-SX); H = (EY-SY);
	Ha= H<0 ? -H : H;
	dY= H<0 ? -1 : 1;

	if (Ha>W)
	{
		/* Y-majored */
		f = Ha>>1;
		while (1)
		{
			switch (style)
			{
				case PS_NONE:
					break;
				case PS_DOT://broken line
					if (SY % 2)
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
					else if (mode == BKM_OPAQUE)
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, bkcolor, bkcolor4bit)
					break;
				case PS_DASH://两点实，一点虚
					if (SY % 3)
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
					else if (mode == BKM_OPAQUE)
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, bkcolor, bkcolor4bit)
					break;
				case PS_DASHDOT:
					switch (SY % 5)
					{
					case 0:
					case 1:
					case 3:
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
						break;
					default:
						if (mode == BKM_OPAQUE)
							HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, bkcolor, bkcolor4bit)
						break;
					}
					break;
				case PS_DASHDOTDOT:
					switch (SY % 7)
					{
					case 0:
					case 1:
					case 3:
					case 5:
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
						break;
					default:
						if (mode == BKM_OPAQUE)
							HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, bkcolor, bkcolor4bit)
						break;
					}
					break;
				case PS_DASHDASHDOT:
					switch (SY % 9)
					{
					case 0:
					case 1:
					case 2:
					case 3:
					case 6:
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
						break;
					default:
						if (mode == BKM_OPAQUE)
							HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, bkcolor, bkcolor4bit)
						break;
					}
					break;
				default:
					HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
					break;
			}
			if (SY==EY) break;
			if ((f+=W)>=Ha)
				f-=Ha,SX++;
			SY+=dY;
		}
	}
	else
	{
		/* X-majored */
		f = W>>1;
		while (1)
		{
			switch (style)
			{
				case PS_NONE:
					break;
				case PS_DOT:
					if (SX % 2)
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
					else if (mode == BKM_OPAQUE)
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, bkcolor, bkcolor4bit)
					break;
				case PS_DASH:
					if (SX % 3)
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
					else if (mode == BKM_OPAQUE)
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, bkcolor, bkcolor4bit)
					break;
				case PS_DASHDOT:
					switch (SX % 5)
					{
					case 0:
					case 1:
					case 3:
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
						break;
					default:
						if (mode == BKM_OPAQUE)
							HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, bkcolor, bkcolor4bit)
						break;
					}
					break;
				case PS_DASHDOTDOT:
					switch (SX % 7)
					{
					case 0:
					case 1:
					case 3:
					case 5:
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
						break;
					default:
						if (mode == BKM_OPAQUE)
							HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, bkcolor, bkcolor4bit)
						break;
					}
					break;
				case PS_DASHDASHDOT:
					switch (SX % 9)
					{
					case 0:
					case 1:
					case 2:
					case 3:
					case 6:
						HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
						break;
					default:
						if (mode == BKM_OPAQUE)
							HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, bkcolor, bkcolor4bit)
						break;
					}
					break;
				default:
					HAI_DRAWPIXEL(pDis, SX, SY, BitsPixel, BmpWidth, fgcolor, fgcolor4bit)
					break;
			}
			if (SX==EX) break;
			if ((f+=Ha)>=W)
				f-=W,SY+=dY;
			SX++;
		}
	}

	return S_TRUE;
}

S_BOOL hai_Rectangle(SH_DC hDC, S_SHORT left, S_SHORT top, S_SHORT right, S_SHORT bottom)
{
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_FALSE;
	if (!((SP_DC)hDC)->lpImage)
		return S_FALSE;
	if (left   < 0) left   = 0;
	if (top    < 0) top    = 0;
	if (right  < 0) right  = 0;
	if (bottom < 0) bottom = 0;
//	if (left   >= ((SP_DC)hDC)->cx) left   = ((SP_DC)hDC)->cx -1;
//	if (right  >= ((SP_DC)hDC)->cx) right  = ((SP_DC)hDC)->cx -1;
//	if (top    >= ((SP_DC)hDC)->cy) top    = ((SP_DC)hDC)->cy -1;
//	if (bottom >= ((SP_DC)hDC)->cy) bottom = ((SP_DC)hDC)->cy -1;

	hai_MoveTo(hDC, left, top, S_NULL);
	hai_LineTo(hDC, right, top);
	hai_LineTo(hDC, right, bottom);
	hai_LineTo(hDC, left, bottom);
	hai_LineTo(hDC, left, top);

	hai_BitBlt(hDC, (S_SHORT)(left+1), (S_SHORT)(top+1), (S_SHORT)(right-left-1), (S_SHORT)(bottom-top-1), S_NULL, 0, 0, PATCOPY);

	return S_TRUE;
}

 SH_PEN hai_CreatePen(S_WORD width, S_COLOR color, S_BYTE  style)
 {
 	_SP_PEN pPen;

	if (pPen = hai_MemAlloc(sizeof(*pPen)))
	{
		HAI_SETHDLTYPE(pPen, HT_PEN);
		pPen->width = width;
		pPen->color  = color;
		pPen->style  = style;
	}
	return (SH_PEN)pPen;
 }
 
S_VOID _hai_DrawHLine(SH_DC hDC, S_SHORT SX, S_SHORT EX, S_SHORT SY, S_COLOR c)
{
	S_BYTE  C4, color4bit[2];
	S_BYTE  mask[2] = {0xF, 0xF0};
	S_BYTE  *pDis;
	S_WORD  bits, BmpW;

	EX += ((SP_DC)hDC)->x;//client to screen
	SX += ((SP_DC)hDC)->x;
	SY += ((SP_DC)hDC)->y;
	if (SX > EX)
	{
		bits = EX; EX = SX; SX = bits;
	}

	color4bit[1] = (S_BYTE)c;
	color4bit[0] = (S_BYTE)c << 4;

	pDis = (S_BYTE*)(((SP_DC)hDC)->lpImage);
	bits = ((_SP_BITMAP)(((SP_DC)hDC)->hBitmap))->BitsPixel;
	BmpW = ((_SP_BITMAP)(((SP_DC)hDC)->hBitmap))->WidthBytes;

	switch (bits)
	{
	case 4:
		C4 = color4bit[1]|color4bit[0];
		pDis += SY*BmpW + (SX>>1);
		if (SX < EX && SX & 0x1)
		{
			*pDis = *pDis&mask[SX&0x1] | color4bit[SX&0x1];
			SX++, pDis++;
		}
		for (;SX < EX-1; SX += 2)
			*pDis++ = C4;
		if (SX == EX-1)
			*pDis = *pDis&mask[SX&0x1] | color4bit[SX&0x1];
		break;
	case 8:
		C4 = (S_BYTE)c;
		pDis += SY*BmpW + SX;
		for (;SX < EX; SX++)
			*pDis++ = C4;
		break;
	case 16:
		pDis += SY*BmpW + (SX<<1);
		for (;SX < EX; SX++)
		{
			*((S_WORD*)pDis) = (S_WORD)c;
			pDis += 2;
		}
		break;
	case 24:
		pDis += SY*BmpW + SX*3;
		for (;SX < EX; SX++)
		{
			*pDis++ = (S_BYTE)c;
			*pDis++ = (S_BYTE)(c>>8);
			*pDis++ = (S_BYTE)(c>>16);
		}
		break;
	default:
		break;
	}
}

S_VOID _hai_DrawVLine(SH_DC hDC, S_SHORT SX, S_SHORT SY, S_SHORT EY, S_COLOR c)
{
	S_BYTE  C4, color4bit[2];
	S_BYTE  mask[2] = {0xF, 0xF0};
	S_BYTE  *pDis;
	S_WORD  bits, BmpW;

	EY += ((SP_DC)hDC)->y;
	SX += ((SP_DC)hDC)->x;
	SY += ((SP_DC)hDC)->y;
	if (SY > EY)
	{
		bits = EY;EY = SY;SY = bits;
	}

	color4bit[1] = (S_BYTE)c;
	color4bit[0] = (S_BYTE)c << 4;

	pDis = (S_BYTE*)(((SP_DC)hDC)->lpImage);
	bits = ((_SP_BITMAP)(((SP_DC)hDC)->hBitmap))->BitsPixel;
	BmpW = ((_SP_BITMAP)(((SP_DC)hDC)->hBitmap))->WidthBytes;

	switch (bits)
	{
	case 4:
		pDis += SY*BmpW + (SX>>1);
		for (;SY < EY; SY++)
		{
			*pDis = *pDis&mask[SX&0x1] | color4bit[SX&0x1];
			pDis += BmpW;
		}
		break;
	case 8:
		C4 = (S_BYTE)c;
		pDis += SY*BmpW + SX;
		for (;SY < EY; SY++)
		{
			*pDis = C4;
			pDis += BmpW;
		}
		break;
	case 16:
		pDis += SY*BmpW + (SX<<1);
		for (;SY < EY; SY++)
		{
			*((S_WORD*)pDis) = (S_WORD)c;
			pDis += BmpW;
		}
		break;
	case 24:
		pDis += SY*BmpW + SX*3;
		for (;SY < EY; SY++)
		{
			*(pDis+0) = (S_BYTE)c;
			*(pDis+1) = (S_BYTE)(c>>8);
			*(pDis+2) = (S_BYTE)(c>>16);
			pDis += BmpW;
		}
		break;
	default:
		break;
	}
}

SH_BITMAP  hai_LoadBitmap(S_BYTE *filename)
{
	S_BYTE  *pRes;
	_SP_BITMAP pBitmap = S_NULL;

	if ((S_DWORD)filename & BUILTIN_RESOURCE_FLAG)
	{
		S_BYTE bits;
		S_SHORT w, h;
		pRes = resource + ((S_DWORD)filename&~BUILTIN_RESOURCE_FLAG);
		w = HAI_MAKEWORD(pRes);
		h = HAI_MAKEWORD(pRes+2);
		bits = *(pRes+4);
		pBitmap = (_SP_BITMAP)hai_MemAlloc(sizeof(_S_BITMAP) + BITMAP_WIDTHBYTES(w, bits) * h + 4);
		if (!pBitmap)
			return S_NULL;
		HAI_SETHDLTYPE(pBitmap, HT_BITMAP);
		pBitmap->BitsPixel	= bits;
		pBitmap->width		= w;
		pBitmap->height		= h;
		pBitmap->WidthBytes = BITMAP_WIDTHBYTES(w, bits);
		pBitmap->lpImage	= ((S_BYTE *)pBitmap)+sizeof(_S_BITMAP);
		hai_RleDecode(pBitmap->lpImage, pRes+5, (S_WORD)((w+1)>>1));
	}
	else
	{
		S_DWORD size;
		SH_FILE hFile = hai_fopen(filename, "rb");
		if (hFile)
		{
			hai_fseek(hFile, 0, SEEK_END);
			size = hai_ftell(hFile);
			pRes = hai_MemAlloc(size);
			if (pRes)
			{
				hai_fseek(hFile, 0, SEEK_SET);
				size = hai_fread(pRes, size, 1, hFile);
				if (*pRes=='B'&&*(pRes+1)=='M')
					pBitmap = (_SP_BITMAP)hai_BmpDecode(pRes);
				else if (*pRes==0x0A)
					pBitmap = (_SP_BITMAP)hai_PcxDecode(pRes);
				else if (HAI_MAKEWORD(pRes)==0&&(HAI_MAKEWORD(pRes+2)==1||HAI_MAKEWORD(pRes+2)==2))
				{
					pBitmap = (_SP_BITMAP)hai_IcoDecode(pRes, 1);
					HAI_SETHDLTYPE(pBitmap, HT_BITMAP);
				}
				hai_MemFree(pRes);
			}
			hai_fclose(hFile);
		}
	}

	return (SH_BITMAP)pBitmap; 
}

SH_BITMAP hai_CreateCompatibleBitmap(SH_DC hDC)
{
	_SP_BITMAP pSrc;
	_SP_BITMAP pDst;
	
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return (SH_BITMAP)S_NULL;
	pSrc = (_SP_BITMAP)(((SP_DC)hDC)->hBitmap);
	if (pSrc == S_NULL)
		return (SH_BITMAP)S_NULL;
		
	pDst = (_SP_BITMAP)hai_MemAlloc(sizeof(_S_BITMAP) + pSrc->height*pSrc->WidthBytes);
	if (!pDst)
		return (SH_BITMAP)S_NULL;

	memset(pDst, 0x00, sizeof(_S_BITMAP) + pSrc->height*pSrc->WidthBytes);
	HAI_SETHDLTYPE(pDst, HT_BITMAP);
	pDst->BitsPixel	= pSrc->BitsPixel;
	pDst->width		= pSrc->width;
	pDst->height		= pSrc->height;
	pDst->WidthBytes = pSrc->WidthBytes;
	pDst->lpImage	= ((S_BYTE *)pDst)+sizeof(_S_BITMAP);

	if (pSrc->lpImage)
		memcpy(pDst->lpImage, pSrc->lpImage, pSrc->WidthBytes*pSrc->height);

	return (SH_BITMAP)pDst;
}

SH_BITMAP hai_CreateBitmap(S_WORD  w, S_WORD  h, S_WORD  BitsPixel, S_VOID  *lpImage)
{
	_SP_BITMAP pBmp;

	if (BitsPixel != 4 && BitsPixel != 8 && BitsPixel != 16 && BitsPixel != 24)
		return (SH_BITMAP)S_NULL;

	pBmp = (_SP_BITMAP)hai_MemAlloc(sizeof(_S_BITMAP));
	if (!pBmp)
		return (SH_BITMAP)S_NULL;

	HAI_SETHDLTYPE(pBmp, HT_BITMAP);
	pBmp->width = w;
	pBmp->height = h;
	pBmp->BitsPixel = BitsPixel;
	pBmp->WidthBytes = BITMAP_WIDTHBYTES(w, BitsPixel);
	pBmp->lpImage = lpImage;
	
	return (SH_BITMAP)pBmp;
}

static S_VOID _memcpy8to4(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_WORD i;
	S_WORD odd = nPixel&0x1;

	nPixel &= 0xFFFE;
	for (i = 0; i < nPixel; i += 2)
	{
		*pDst++ = (*pSrc&0xF0)|(*(pSrc+1)>>4);
		pSrc += 2;
	}
	if (odd)
		*pDst++ = *pSrc&0xF0;
}
static S_VOID _memcpy16to4(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_BYTE by;
	S_WORD i;
	S_DWORD c, r, g, b, y;
	S_WORD odd = nPixel&0x1;

	nPixel &= 0xFFFE;
	for (i = 0; i < nPixel; i += 2)
	{
		c = *(S_WORD *)pSrc;
		pSrc += 2;
		r = (c&0x1F)<<3;
		g = (c&0x7E0)>>3;
		b = (c&0xF800)>>8;
		y = ((66*r + 129*g + 25*b + 128) >> 8) + 16;
		if (y > 255) y = 255;
		by = (S_BYTE)y&0xF0;
		
		c = *(S_WORD *)pSrc;
		pSrc += 2;
		r = (c&0x1F)<<3;
		g = (c&0x7E0)>>3;
		b = (c&0xF800)>>8;
		y = ((66*r + 129*g + 25*b + 128) >> 8) + 16;
		if (y > 255) y = 255;

		*pDst++ = (S_BYTE)(by|((y&0xF0)>>4));
	}
	if (odd)
	{
		c = *(S_WORD *)pSrc;
		pSrc += 2;
		r = (c&0x1F)<<3;
		g = (c&0x7E0)>>3;
		b = (c&0xF800)>>8;
		y = ((66*r + 129*g + 25*b + 128) >> 8) + 16;
		if (y > 255) y = 255;
		
		*pDst++ = (S_BYTE)(y&0xF0);
	}
}
static S_VOID _memcpy24to4(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_BYTE by;
	S_WORD i;
	S_DWORD r, g, b, y;
	S_WORD odd = nPixel&0x1;

	nPixel &= 0xFFFE;
	for (i = 0; i < nPixel; i += 2)
	{
		r = (S_DWORD)*pSrc++;
		g = (S_DWORD)*pSrc++;
		b = (S_DWORD)*pSrc++;
		y = ((66*r + 129*g + 25*b + 128) >> 8) + 16;
		if (y > 255) y = 255;
		by = (S_BYTE)y&0xF0;
		
		r = (S_DWORD)*pSrc++;
		g = (S_DWORD)*pSrc++;
		b = (S_DWORD)*pSrc++;
		y = ((66*r + 129*g + 25*b + 128) >> 8) + 16;
		if (y > 255) y = 255;

		*pDst++ = (S_BYTE)(by|((y&0xF0)>>4));
	}
	if (odd)
	{
		r = (S_DWORD)*pSrc++;
		g = (S_DWORD)*pSrc++;
		b = (S_DWORD)*pSrc++;
		y = ((66*r + 129*g + 25*b + 128) >> 8) + 16;
		if (y > 255) y = 255;
		
		*pDst++ = (S_BYTE)(y&0xF0);
	}
}
static S_BOOL _hai_ConverBitmapTo4Bits(_SP_BITMAP pBmpDst, _SP_BITMAP pBmpSrc)
{
	S_WORD i;
	S_BYTE * pSrc = (S_BYTE *)(pBmpSrc->lpImage);
	S_BYTE * pDst = (S_BYTE *)(pBmpDst->lpImage);
	S_WORD  WidthBytes = pBmpSrc->WidthBytes;

	if (pBmpDst->BitsPixel != 4)
		return S_FALSE;

	switch (pBmpSrc->BitsPixel)
	{
	case 4:
		for (i = 0; i < pBmpDst->height; i++)
		{
			memcpy(pDst, pSrc, WidthBytes);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 8:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy8to4(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 16:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy16to4(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 24:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy24to4(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	default:
		return S_FALSE;
		break;
	}
	return S_TRUE;
}

static S_VOID _memcpy4to8(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_WORD i;
	S_WORD odd = nPixel&0x1;

	nPixel &= 0xFFFE;
	for (i = 0; i < nPixel; i += 2)
	{
		*pDst++ = *pSrc&0xF0;
		*pDst++  = *pSrc<<4;
		pSrc++;
	}
	if (odd)
		*pDst++ = *pSrc&0xF0;
}
static S_VOID _memcpy16to8(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_WORD i;
	S_DWORD c, r, g, b, y;

	for (i = 0; i < nPixel; i++)
	{
		c = *(S_WORD *)pSrc;
		pSrc += 2;
		r = (c&0x1F)<<3;
		g = (c&0x7E0)>>3;
		b = (c&0xF800)>>8;
		y = ((66*r + 129*g + 25*b + 128) >> 8) + 16;
		if (y > 255) y = 255;

		*pDst++ = (S_BYTE)y;
	}
}
static S_VOID _memcpy24to8(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_WORD i;
	S_DWORD r, g, b, y;

	for (i = 0; i < nPixel; i++)
	{
		r = (S_DWORD)*pSrc++;
		g = (S_DWORD)*pSrc++;
		b = (S_DWORD)*pSrc++;
//		y = 0.30*r+0.59*g+0.11*b;
		y = ((66*r + 129*g + 25*b + 128) >> 8) + 16;
		if (y > 255) y = 255;

		*pDst++ = (S_BYTE)y;
	}
}

static S_BOOL _hai_ConverBitmapTo8Bits(_SP_BITMAP pBmpDst, _SP_BITMAP pBmpSrc)
{
	S_WORD i;
	S_BYTE * pSrc = (S_BYTE *)(pBmpSrc->lpImage);
	S_BYTE * pDst = (S_BYTE *)(pBmpDst->lpImage);
	S_WORD  WidthBytes = pBmpSrc->WidthBytes;

	if (pBmpDst->BitsPixel != 8)
		return S_FALSE;

	switch (pBmpSrc->BitsPixel)
	{
	case 4:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy4to8(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 8:
		for (i = 0; i < pBmpDst->height; i++)
		{
			memcpy(pDst, pSrc, WidthBytes);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 16:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy16to8(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 24:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy24to8(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	default:
		return S_FALSE;
		break;
	}
	return S_TRUE;

}
static S_VOID _memcpy4to16(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_WORD i;
	S_WORD odd = nPixel&0x1;
	S_COLOR c;

	nPixel &= 0xFFFE;
	for (i = 0; i < nPixel; i += 2)
	{
		c = gPalette4Bits[*pSrc>>4];
		*(S_WORD *)pDst = (S_WORD)(((c&0xF8)>>3)|((c&0xFC00)>>5)|((c&0xF80000)>>8));
		pDst += 2;

		c = gPalette4Bits[*pSrc&0xF];
		*(S_WORD *)pDst = (S_WORD)(((c&0xF8)>>3)|((c&0xFC00)>>5)|((c&0xF80000)>>8));
		pDst += 2;

		pSrc++;
	}
	if (odd)
	{
		c = gPalette4Bits[*pSrc>>4];
		*(S_WORD *)pDst = (S_WORD)(((c&0xF8)>>3)|((c&0xFC00)>>5)|((c&0xF80000)>>8));
		pDst += 2;
	}
}
static S_VOID _memcpy8to16(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_WORD i;
	S_COLOR c;

	for (i = 0; i < nPixel; i++)
	{
		c = gPalette8Bits[*pSrc++];
		*(S_WORD *)pDst = (S_WORD)(((c&0xF8)>>3)|((c&0xFC00)>>5)|((c&0xF80000)>>8));
		pDst += 2;
	}
}
static S_VOID _memcpy24to16(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_WORD i;
	S_WORD c;

	for (i = 0; i < nPixel; i++)
	{
		c = (*pSrc>>3)|((S_WORD)(*(pSrc+1)>>3)<<5)|((S_WORD)(*(pSrc+2)>>3)<<11);
		pSrc += 3;
		*(S_WORD *)pDst = c;
		pDst += 2;
	}
}
static S_BOOL _hai_ConverBitmapTo16Bits(_SP_BITMAP pBmpDst, _SP_BITMAP pBmpSrc)
{
	S_WORD i;
	S_BYTE * pSrc = (S_BYTE *)(pBmpSrc->lpImage);
	S_BYTE * pDst = (S_BYTE *)(pBmpDst->lpImage);
	S_WORD  WidthBytes = pBmpSrc->WidthBytes;

	if (pBmpDst->BitsPixel != 16)
		return S_FALSE;

	switch (pBmpSrc->BitsPixel)
	{
	case 4:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy4to16(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 8:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy8to16(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 16:
		for (i = 0; i < pBmpDst->height; i++)
		{
			memcpy(pDst, pSrc, WidthBytes);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 24:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy24to16(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	default:
		return S_FALSE;
		break;
	}
	return S_TRUE;

}

static S_VOID _memcpy4to24(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_WORD i;
	S_WORD odd = nPixel&0x1;
	S_COLOR c;

	nPixel &= 0xFFFE;
	for (i = 0; i < nPixel; i += 2)
	{
		c = gPalette4Bits[*pSrc>>4];
		*pDst++ = (S_BYTE)c;
		*pDst++ = (S_BYTE)(c>>8);
		*pDst++ = (S_BYTE)(c>>16);

		c = gPalette4Bits[*pSrc&0xF];
		*pDst++ = (S_BYTE)c;
		*pDst++ = (S_BYTE)(c>>8);
		*pDst++ = (S_BYTE)(c>>16);

		pSrc++;
	}
	if (odd)
	{
		c = gPalette4Bits[*pSrc>>4];
		*pDst++ = (S_BYTE)c;
		*pDst++ = (S_BYTE)(c>>8);
		*pDst++ = (S_BYTE)(c>>16);
	}
}
static S_VOID _memcpy8to24(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_WORD i;
	S_COLOR c;

	for (i = 0; i < nPixel; i++)
	{
		c = gPalette8Bits[*pSrc++];
		*pDst++ = (S_BYTE)c;
		*pDst++ = (S_BYTE)(c>>8);
		*pDst++ = (S_BYTE)(c>>16);
	}
}
static S_VOID _memcpy16to24(S_BYTE *pDst, S_BYTE *pSrc, S_WORD nPixel)
{
	S_WORD i;
	S_COLOR c;

	for (i = 0; i < nPixel; i++)
	{
		c = *(S_WORD *)pSrc;
		pSrc += 2;
		*pDst++ = (S_BYTE)(c<<3);
		*pDst++ = (S_BYTE)((c>>5)<<2);
		*pDst++ = (S_BYTE)((c>>11)<<3);
	}
}
static S_BOOL _hai_ConverBitmapTo24Bits(_SP_BITMAP pBmpDst, _SP_BITMAP pBmpSrc)
{
	S_WORD i;
	S_BYTE * pSrc = (S_BYTE *)(pBmpSrc->lpImage);
	S_BYTE * pDst = (S_BYTE *)(pBmpDst->lpImage);
	S_WORD  WidthBytes = pBmpSrc->WidthBytes;

	if (pBmpDst->BitsPixel != 24)
		return S_FALSE;

	switch (pBmpSrc->BitsPixel)
	{
	case 4:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy4to24(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 8:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy8to24(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 16:
		for (i = 0; i < pBmpDst->height; i++)
		{
			_memcpy16to24(pDst, pSrc, pBmpSrc->width);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	case 24:
		for (i = 0; i < pBmpDst->height; i++)
		{
			memcpy(pDst, pSrc, WidthBytes);
			pSrc += WidthBytes;
			pDst += pBmpDst->WidthBytes;
		}
		break;
	default:
		return S_FALSE;
		break;
	}
	return S_TRUE;

}

SH_BITMAP  hai_CreateFormatBitmap(SH_DC hDC, SH_BITMAP hBmp)
{
	S_WORD WidthBytes;
	_SP_BITMAP pBmp;
	_SP_BITMAP pBmpDst;
	_SP_BITMAP pBmpSrc = (_SP_BITMAP)hBmp;
	
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return (SH_BITMAP )S_NULL;
	if (!hBmp || HAI_GETHDLTYPE(hBmp) != HT_BITMAP || pBmpSrc->lpImage == S_NULL)
		return (SH_BITMAP )S_NULL;
	pBmp = (_SP_BITMAP)(((SP_DC)hDC)->hBitmap);
	if (pBmp == S_NULL)
		return (SH_BITMAP)S_NULL;

	WidthBytes = BITMAP_WIDTHBYTES(pBmpSrc->width, pBmp->BitsPixel);
	pBmpDst = (_SP_BITMAP)hai_MemAlloc(sizeof(_S_BITMAP)+WidthBytes*pBmpSrc->height);
	if (!pBmpDst)
		return (SH_BITMAP)S_NULL;

	HAI_SETHDLTYPE(pBmpDst , HT_BITMAP);
	pBmpDst ->width = pBmpSrc->width;
	pBmpDst ->height = pBmpSrc->height;
	pBmpDst ->BitsPixel = pBmp->BitsPixel;
	pBmpDst ->WidthBytes = WidthBytes;
	pBmpDst ->lpImage = ((S_BYTE *)pBmpDst)+sizeof(_S_BITMAP);

	switch (pBmpDst->BitsPixel)
	{
	case 4:
		_hai_ConverBitmapTo4Bits(pBmpDst, pBmpSrc);
		break;
	case 8:
		_hai_ConverBitmapTo8Bits(pBmpDst, pBmpSrc);
		break;
	case 16:
		_hai_ConverBitmapTo16Bits(pBmpDst, pBmpSrc);
		break;
	case 24:
		_hai_ConverBitmapTo24Bits(pBmpDst, pBmpSrc);
		break;
	default:
		break;
	}
	
	return (SH_BITMAP)pBmpDst ;
}

SH_ICON hai_LoadIcon(S_BYTE *filename)
{
	S_DWORD size;
	S_BYTE  *pRes;
	_SP_ICON pIco = S_NULL;

	if ((S_DWORD)filename & BUILTIN_RESOURCE_FLAG)
	{
	}
	else
	{
		SH_FILE hFile = hai_fopen(filename, "rb");
		if (hFile) {
			hai_fseek(hFile, 0, SEEK_END);
			size = hai_ftell(hFile);
			pRes = hai_MemAlloc(size);
			if (pRes)
			{
				hai_fseek(hFile, 0, SEEK_SET);
				size = hai_fread(pRes, size, 1, hFile);
				if (HAI_MAKEWORD(pRes)==0&&(HAI_MAKEWORD(pRes+2)==1||HAI_MAKEWORD(pRes+2)==2))
					pIco = (_SP_ICON)hai_IcoDecode(pRes, 2);
				hai_MemFree(pRes);
			}
			hai_fclose(hFile);
		}
	}

	return (SH_ICON)pIco; 
}

SH_ICON hai_CreateFormatIcon(SH_DC hDC, SH_ICON hIcon)
{
	S_WORD WidthBytes;
	_SP_BITMAP pBmp;
	_SP_ICON picoDst;
	_SP_ICON picoSrc = (_SP_ICON)hIcon;
	
	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return (SH_ICON )S_NULL;
	if (!hIcon || HAI_GETHDLTYPE(hIcon) != HT_ICON || picoSrc->lpImage == S_NULL)
		return (SH_ICON )S_NULL;
	pBmp = (_SP_BITMAP)(((SP_DC)hDC)->hBitmap);
	if (pBmp == S_NULL)
		return (SH_ICON)S_NULL;

	WidthBytes = BITMAP_WIDTHBYTES(picoSrc->width, pBmp->BitsPixel);
	picoDst = (_SP_ICON)hai_MemAlloc(sizeof(*picoDst)+WidthBytes*picoSrc->height+MASKWIDTH(picoSrc->width)*picoSrc->height+4);
	if (!picoDst)
		return (SH_ICON)S_NULL;

	HAI_SETHDLTYPE(picoDst , HT_ICON);
	picoDst ->width = picoSrc->width;
	picoDst ->height = picoSrc->height;
	picoDst ->BitsPixel = pBmp->BitsPixel;
	picoDst ->WidthBytes = WidthBytes;
	picoDst ->lpImage = ((S_BYTE *)picoDst)+sizeof(*picoDst);
	picoDst->lpMask = ((S_BYTE *)picoDst)+sizeof(*picoDst)+WidthBytes*picoDst->height;

	switch (picoDst->BitsPixel)
	{
	case 4:
		_hai_ConverBitmapTo4Bits((_SP_BITMAP)picoDst, (_SP_BITMAP)picoSrc);
		break;
	case 8:
		_hai_ConverBitmapTo8Bits((_SP_BITMAP)picoDst, (_SP_BITMAP)picoSrc);
		break;
	case 16:
		_hai_ConverBitmapTo16Bits((_SP_BITMAP)picoDst, (_SP_BITMAP)picoSrc);
		break;
	case 24:
		_hai_ConverBitmapTo24Bits((_SP_BITMAP)picoDst, (_SP_BITMAP)picoSrc);
		break;
	default:
		memset(picoDst->lpImage, 0x00, WidthBytes*picoDst->height);
		break;
	}
	if (picoSrc->lpMask)
		memcpy(picoDst->lpMask, picoSrc->lpMask, MASKWIDTH(picoSrc->width)*picoSrc->height);
	else
		memset(picoDst->lpMask, 0x00, MASKWIDTH(picoSrc->width)*picoSrc->height);
	
	return (SH_ICON)picoDst ;
}

S_BOOL hai_DrawIcon(SH_DC hDC, S_SHORT x, S_SHORT y, SH_ICON hIcon)
{
	S_BOOL ret;

	if (!hDC || !hIcon || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_FALSE;
	if (x >= ((SP_DC)hDC)->cx || y >= ((SP_DC)hDC)->cy)
		return S_FALSE;
	if (!((SP_DC)hDC)->lpImage || !((SP_DC)hDC)->hBitmap)
		return S_FALSE;

	if (HAI_GETHDLTYPE(hIcon) == HT_BITMAP)
	{
		SH_DC hdcMem;
		_SP_BITMAP pBitmap = (_SP_BITMAP)hIcon;
		
		hdcMem = hai_CreateCompatibleDC(hDC);
		hai_SelectObject(hdcMem, (SH_GDIOBJ)pBitmap);
		ret = hai_BitBlt(hDC, x, y, pBitmap->width, pBitmap->height, hdcMem, 0, 0, SRCCOPY);		
		hai_DeleteDC(hdcMem);
	}
	else if (HAI_GETHDLTYPE(hIcon) == HT_ICON)
	{
		S_BYTE mask[] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
		S_WORD i, j, w, h, ix=0, iy=0;
		S_BYTE *pdst, *psrc, *pmsk;
		_SP_ICON picon = (_SP_ICON)hIcon;
		_SP_BITMAP pbmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;
		if (picon->BitsPixel != pbmp->BitsPixel)
			return S_FALSE;
		if (!picon->lpImage || !picon->lpMask)
			return S_FALSE;
		if (x < 0) {ix = -x; x = 0;}
		if (y < 0) {iy = -y; y = 0;}
		if (ix >= picon->width || iy >= picon->height)
			return S_FALSE;
		w = picon->width - ix;
		h = picon->height - iy;
		if (w > ((SP_DC)hDC)->cx-x) w = ((SP_DC)hDC)->cx-x;
		if (h > ((SP_DC)hDC)->cy-y) h = ((SP_DC)hDC)->cy-y;
		x += ((SP_DC)hDC)->x;//convert absolute address
		y += ((SP_DC)hDC)->y;

		pdst = (S_BYTE *)((SP_DC)hDC)->lpImage;
		psrc = (S_BYTE *)picon->lpImage;
		switch (pbmp->BitsPixel)
		{
		case 4:
			x &= ~0x1, ix &= ~0x1, w &= ~0x1;
			pdst += y * pbmp->WidthBytes + (x>>1);
			psrc += iy * picon->WidthBytes + (ix>>1);
			for (j = 0; j < h; j++) {
				pmsk = (S_BYTE *)picon->lpMask + (iy+j) * MASKWIDTH(picon->width);
				for (i = 0; i < w; i += 2) {
					if (!(pmsk[(ix+i)>>3] & mask[(ix+i)&0x7]))
						*pdst = (*pdst&0x0F)|(*psrc&0xF0);
					if (!(pmsk[(ix+i+1)>>3] & mask[(ix+i+1)&0x7]))
						*pdst = (*pdst&0xF0)|(*psrc&0x0F);
					pdst++, psrc++;
				}
				pdst += pbmp->WidthBytes - (w>>1);
				psrc += picon->WidthBytes - (w>>1);
			}
			break;
		case 8:
			pdst += y * pbmp->WidthBytes + x;
			psrc += iy * picon->WidthBytes + ix;
			for (j = 0; j < h; j++) {
				pmsk = (S_BYTE *)picon->lpMask + (iy+j) * MASKWIDTH(picon->width);
				for (i = 0; i < w; i++) {
					if (pmsk[(ix+i)>>3] & mask[(ix+i)&0x7])
						pdst++, psrc++;
					else
						*pdst++ = *psrc++;
				}
				pdst += pbmp->WidthBytes - w;
				psrc += picon->WidthBytes - w;
			}
			break;
		case 16:
			pdst += y * pbmp->WidthBytes + (x<<1);
			psrc += iy * picon->WidthBytes + (ix<<1);
			for (j = 0; j < h; j++) {
				pmsk = (S_BYTE *)picon->lpMask + (iy+j) * MASKWIDTH(picon->width);
				for (i = 0; i < w; i++) {
					if (!(pmsk[(ix+i)>>3] & mask[(ix+i)&0x7]))
						*(S_WORD*)pdst = *(S_WORD*)psrc;
					pdst += 2, psrc += 2;
				}
				pdst += pbmp->WidthBytes - (w<<1);
				psrc += picon->WidthBytes - (w<<1);
			}
			break;
		case 24:
			pdst += y * pbmp->WidthBytes + (x*3);
			psrc += iy * picon->WidthBytes + (ix*3);
			for (j = 0; j < h; j++) {
				pmsk = (S_BYTE *)picon->lpMask + (iy+j) * MASKWIDTH(picon->width);
				for (i = 0; i < w; i++) {
					if (pmsk[(ix+i)>>3] & mask[(ix+i)&0x7])
						pdst += 3, psrc += 3;
					else
						*pdst++ = *psrc++, *pdst++ = *psrc++, *pdst++ = *psrc++;
				}
				pdst += pbmp->WidthBytes - (w*3);
				psrc += picon->WidthBytes - (w*3);
			}
			break;
		}
		ret = S_TRUE;
	}
	else
		return S_FALSE;
	
	return ret;
}

static S_BOOL __BitBlt2_Blackness(S_BYTE *pImage, S_WORD BitsPixel, S_WORD WidthBytes, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h)
{
	S_BYTE c8;
	S_SHORT wbak = w;
	
	switch (BitsPixel)
	{
	case 4:
		c8 = (S_BYTE)COLOR_GRAY00<<4 | (S_BYTE)COLOR_GRAY00;
		pImage += y * WidthBytes + (x>>1);
		while (h--)
		{
			w = wbak;
			if (w && x&0x1)
			{
				*pImage = (*pImage&0xF0) | COLOR_GRAY00;
				pImage++;
				w--;
			}
			for (;w > 1; w -= 2)
			{
				*pImage++ = c8;		
			}
			if (w == 1)
			{
				*pImage = (S_BYTE)((*pImage&0x0F) | COLOR_GRAY00<<4);
				pImage++;
			}
			pImage += WidthBytes - ((wbak+1)>>1);
			if (x&0x1 && !(wbak&0x1))
				pImage -= 1;
		}
		return S_TRUE;
	case 8:
		pImage += y * WidthBytes + x;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = COLOR_BLACK_8;
			case 7:   *pImage++ = COLOR_BLACK_8;
			case 6:   *pImage++ = COLOR_BLACK_8;
			case 5:   *pImage++ = COLOR_BLACK_8;
			case 4:   *pImage++ = COLOR_BLACK_8;
			case 3:   *pImage++ = COLOR_BLACK_8;
			case 2:   *pImage++ = COLOR_BLACK_8;
			case 1:   *pImage++ = COLOR_BLACK_8;
					}while (--w);
			}
			pImage += WidthBytes - wbak;
		}
		return S_TRUE;
	case 16:
		pImage += y * WidthBytes + (x<<1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*(S_WORD*)pImage = COLOR_BLACK_16; pImage += 2;
			case 7:   *(S_WORD*)pImage = COLOR_BLACK_16; pImage += 2;
			case 6:   *(S_WORD*)pImage = COLOR_BLACK_16; pImage += 2;
			case 5:   *(S_WORD*)pImage = COLOR_BLACK_16; pImage += 2;
			case 4:   *(S_WORD*)pImage = COLOR_BLACK_16; pImage += 2;
			case 3:   *(S_WORD*)pImage = COLOR_BLACK_16; pImage += 2;
			case 2:   *(S_WORD*)pImage = COLOR_BLACK_16; pImage += 2;
			case 1:   *(S_WORD*)pImage = COLOR_BLACK_16; pImage += 2;
					}while (--w);
			}
			pImage += WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pImage += y * WidthBytes + x*3;
		c8 = (S_BYTE)COLOR_BLACK_24;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = c8;*pImage++ = c8;*pImage++ = c8;
			case 7:   *pImage++ = c8;*pImage++ = c8;*pImage++ = c8;
			case 6:   *pImage++ = c8;*pImage++ = c8;*pImage++ = c8;
			case 5:   *pImage++ = c8;*pImage++ = c8;*pImage++ = c8;
			case 4:   *pImage++ = c8;*pImage++ = c8;*pImage++ = c8;
			case 3:   *pImage++ = c8;*pImage++ = c8;*pImage++ = c8;
			case 2:   *pImage++ = c8;*pImage++ = c8;*pImage++ = c8;
			case 1:   *pImage++ = c8;*pImage++ = c8;*pImage++ = c8;
					}while (--w);
			}
			pImage += WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt2_Whiteness(S_BYTE *pImage, S_WORD BitsPixel, S_WORD WidthBytes, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h)
{
	S_BYTE c8;
	S_SHORT wbak = w;
	
	switch (BitsPixel)
	{
	case 4:
		c8 = (S_BYTE)COLOR_GRAY15<<4 | (S_BYTE)COLOR_GRAY15;
		pImage += y * WidthBytes + (x>>1);
		while (h--)
		{
			w = wbak;
			if (w && x&0x1)
			{
				*pImage = (*pImage&0xF0) | COLOR_GRAY15;
				pImage++;
				w--;
			}
			for (;w > 1; w -= 2)
			{
				*pImage++ = c8;		
			}
			if (w == 1)
			{
				*pImage = (S_BYTE)((*pImage&0x0F) | COLOR_GRAY15<<4);
				pImage++;
			}
			pImage += WidthBytes - ((wbak+1)>>1);
			if (x&0x1 && !(wbak&0x1))
				pImage -= 1;
		}
		return S_TRUE;
	case 8:
		pImage += y * WidthBytes + x;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = COLOR_WHITE_8;
			case 7:   *pImage++ = COLOR_WHITE_8;
			case 6:   *pImage++ = COLOR_WHITE_8;
			case 5:   *pImage++ = COLOR_WHITE_8;
			case 4:   *pImage++ = COLOR_WHITE_8;
			case 3:   *pImage++ = COLOR_WHITE_8;
			case 2:   *pImage++ = COLOR_WHITE_8;
			case 1:   *pImage++ = COLOR_WHITE_8;
					}while (--w);
			}
			pImage += WidthBytes - wbak;
		}
		return S_TRUE;
	case 16:
		pImage += y * WidthBytes + (x<<1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*(S_WORD*)pImage = COLOR_WHITE_16; pImage += 2;
			case 7:   *(S_WORD*)pImage = COLOR_WHITE_16; pImage += 2;
			case 6:   *(S_WORD*)pImage = COLOR_WHITE_16; pImage += 2;
			case 5:   *(S_WORD*)pImage = COLOR_WHITE_16; pImage += 2;
			case 4:   *(S_WORD*)pImage = COLOR_WHITE_16; pImage += 2;
			case 3:   *(S_WORD*)pImage = COLOR_WHITE_16; pImage += 2;
			case 2:   *(S_WORD*)pImage = COLOR_WHITE_16; pImage += 2;
			case 1:   *(S_WORD*)pImage = COLOR_WHITE_16; pImage += 2;
					}while (--w);
			}
			pImage += WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pImage += y * WidthBytes + x*3;
		c8 = (S_BYTE)COLOR_WHITE_24;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = c8; *pImage++ = c8; *pImage++ = c8;
			case 7:   *pImage++ = c8; *pImage++ = c8; *pImage++ = c8;
			case 6:   *pImage++ = c8; *pImage++ = c8; *pImage++ = c8;
			case 5:   *pImage++ = c8; *pImage++ = c8; *pImage++ = c8;
			case 4:   *pImage++ = c8; *pImage++ = c8; *pImage++ = c8;
			case 3:   *pImage++ = c8; *pImage++ = c8; *pImage++ = c8;
			case 2:   *pImage++ = c8; *pImage++ = c8; *pImage++ = c8;
			case 1:   *pImage++ = c8; *pImage++ = c8; *pImage++ = c8;
					}while (--w);
			}
			pImage += WidthBytes - (wbak*3);
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt2_Dstinvert(S_BYTE *pImage, S_WORD BitsPixel, S_WORD WidthBytes, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h)
{
	S_SHORT wbak = w;
	
	switch (BitsPixel)
	{
	case 4:
		pImage += y * WidthBytes + (x>>1);
		while (h--)
		{
			w = wbak;
			if (w && x&0x1)
			{
				*pImage = (*pImage & 0xF0) | (~*pImage & 0xF);
				pImage++;
				w--;
			}
			for (;w > 1; w -= 2)
			{
				*pImage = ~*pImage;
				pImage++;
			}
			if (w == 1)
			{
				*pImage = (*pImage&0xF) | (~*pImage & 0xF0);
				pImage++;
			}
			pImage += WidthBytes - ((wbak+1)>>1);
			if (x&0x1 && !(wbak&0x1))
				pImage -= 1;
		}
		return S_TRUE;
	case 8:
		pImage += y * WidthBytes + x;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = ~*pImage;
			case 7:   *pImage++ = ~*pImage;
			case 6:   *pImage++ = ~*pImage;
			case 5:   *pImage++ = ~*pImage;
			case 4:   *pImage++ = ~*pImage;
			case 3:   *pImage++ = ~*pImage;
			case 2:   *pImage++ = ~*pImage;
			case 1:   *pImage++ = ~*pImage;
					}while (--w);
			}
			pImage += WidthBytes - wbak;
		}
		return S_TRUE;
	case 16:
		pImage += y * WidthBytes + (x<<1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*(S_WORD*)pImage = ~*(S_WORD*)pImage; pImage += 2;
			case 7:   *(S_WORD*)pImage = ~*(S_WORD*)pImage; pImage += 2;
			case 6:   *(S_WORD*)pImage = ~*(S_WORD*)pImage; pImage += 2;
			case 5:   *(S_WORD*)pImage = ~*(S_WORD*)pImage; pImage += 2;
			case 4:   *(S_WORD*)pImage = ~*(S_WORD*)pImage; pImage += 2;
			case 3:   *(S_WORD*)pImage = ~*(S_WORD*)pImage; pImage += 2;
			case 2:   *(S_WORD*)pImage = ~*(S_WORD*)pImage; pImage += 2;
			case 1:   *(S_WORD*)pImage = ~*(S_WORD*)pImage; pImage += 2;
					}while (--w);
			}
			pImage += WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pImage += y * WidthBytes + x*3;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = ~*pImage; *pImage++ = ~*pImage; *pImage++ = ~*pImage; 
			case 7:   *pImage++ = ~*pImage; *pImage++ = ~*pImage; *pImage++ = ~*pImage; 
			case 6:   *pImage++ = ~*pImage; *pImage++ = ~*pImage; *pImage++ = ~*pImage; 
			case 5:   *pImage++ = ~*pImage; *pImage++ = ~*pImage; *pImage++ = ~*pImage; 
			case 4:   *pImage++ = ~*pImage; *pImage++ = ~*pImage; *pImage++ = ~*pImage; 
			case 3:   *pImage++ = ~*pImage; *pImage++ = ~*pImage; *pImage++ = ~*pImage; 
			case 2:   *pImage++ = ~*pImage; *pImage++ = ~*pImage; *pImage++ = ~*pImage; 
			case 1:   *pImage++ = ~*pImage; *pImage++ = ~*pImage; *pImage++ = ~*pImage; 
					}while (--w);
			}
			pImage += WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt2_Patinvert(S_BYTE *pImage, S_WORD BitsPixel, S_WORD WidthBytes, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_COLOR PatColor)
{
	S_BYTE c8, r, g, b;
	S_WORD c16;
	S_SHORT wbak = w;
	
	switch (BitsPixel)
	{
	case 4:
		pImage += y * WidthBytes + (x>>1);
		c8 = (S_BYTE)(PatColor<<4) | (S_BYTE)(PatColor&0xF);
		while (h--)
		{
			w = wbak;
			if (w && x&0x1)
			{
				*pImage = (*pImage & 0xF0) | ((*pImage^c8)&0xF);
				pImage++;
				w--;
			}
			for (;w > 1; w -= 2)
			{
				*pImage = *pImage ^ c8;
				pImage++;
			}
			if (w == 1)
			{
				*pImage = (*pImage&0xF) | ((*pImage^c8)&0xF0);
				pImage++;
			}
			pImage += WidthBytes - ((wbak+1)>>1);
			if (x&0x1 && !(wbak&0x1))
				pImage -= 1;
		}
		return S_TRUE;
	case 8:
		pImage += y * WidthBytes + x;
		c8 = (S_BYTE)PatColor;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = *pImage ^ c8;
			case 7:   *pImage++ = *pImage ^ c8;
			case 6:   *pImage++ = *pImage ^ c8;
			case 5:   *pImage++ = *pImage ^ c8;
			case 4:   *pImage++ = *pImage ^ c8;
			case 3:   *pImage++ = *pImage ^ c8;
			case 2:   *pImage++ = *pImage ^ c8;
			case 1:   *pImage++ = *pImage ^ c8;
					}while (--w);
			}
			pImage += WidthBytes - wbak;
		}
		return S_TRUE;
	case 16:
		pImage += y * WidthBytes + (x<<1);
		c16 = (S_WORD)PatColor;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*(S_WORD*)pImage = *(S_WORD*)pImage ^ c16; pImage += 2;
			case 7:   *(S_WORD*)pImage = *(S_WORD*)pImage ^ c16; pImage += 2;
			case 6:   *(S_WORD*)pImage = *(S_WORD*)pImage ^ c16; pImage += 2;
			case 5:   *(S_WORD*)pImage = *(S_WORD*)pImage ^ c16; pImage += 2;
			case 4:   *(S_WORD*)pImage = *(S_WORD*)pImage ^ c16; pImage += 2;
			case 3:   *(S_WORD*)pImage = *(S_WORD*)pImage ^ c16; pImage += 2;
			case 2:   *(S_WORD*)pImage = *(S_WORD*)pImage ^ c16; pImage += 2;
			case 1:   *(S_WORD*)pImage = *(S_WORD*)pImage ^ c16; pImage += 2;
					}while (--w);
			}
			pImage += WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pImage += y * WidthBytes + x*3;
		r = (S_BYTE)PatColor; g = (S_BYTE)(PatColor>>8); b = (S_BYTE)(PatColor>>16);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = *pImage ^ r; *pImage++ = *pImage ^ g; *pImage++ = *pImage ^ b;
			case 7:   *pImage++ = *pImage ^ r; *pImage++ = *pImage ^ g; *pImage++ = *pImage ^ b;
			case 6:   *pImage++ = *pImage ^ r; *pImage++ = *pImage ^ g; *pImage++ = *pImage ^ b;
			case 5:   *pImage++ = *pImage ^ r; *pImage++ = *pImage ^ g; *pImage++ = *pImage ^ b;
			case 4:   *pImage++ = *pImage ^ r; *pImage++ = *pImage ^ g; *pImage++ = *pImage ^ b;
			case 3:   *pImage++ = *pImage ^ r; *pImage++ = *pImage ^ g; *pImage++ = *pImage ^ b;
			case 2:   *pImage++ = *pImage ^ r; *pImage++ = *pImage ^ g; *pImage++ = *pImage ^ b;
			case 1:   *pImage++ = *pImage ^ r; *pImage++ = *pImage ^ g; *pImage++ = *pImage ^ b;
					}while (--w);
			}
			pImage += WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt2_Patpaint(S_BYTE *pImage, S_WORD BitsPixel, S_WORD WidthBytes, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_COLOR PatColor)
{
	S_BYTE c8, r, g, b;
	S_WORD c16;
	S_SHORT wbak = w;
	
	switch (BitsPixel)
	{
	case 4:
		pImage += y * WidthBytes + (x>>1);
		c8 = (S_BYTE)(PatColor<<4) | (S_BYTE)(PatColor&0xF);
		while (h--)
		{
			w = wbak;
			if (w && x&0x1)
			{
				*pImage = (*pImage & 0xF0) | ((*pImage|c8)&0xF);
				pImage++;
				w--;
			}
			for (;w > 1; w -= 2)
			{
				*pImage = *pImage | c8;
				pImage++;
			}
			if (w == 1)
			{
				*pImage = (*pImage&0xF) | ((*pImage|c8)&0xF0);
				pImage++;
			}
			pImage += WidthBytes - ((wbak+1)>>1);
			if (x&0x1 && !(wbak&0x1))
				pImage -= 1;
		}
		return S_TRUE;
	case 8:
		pImage += y * WidthBytes + x;
		c8 = (S_BYTE)PatColor;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = *pImage | c8;
			case 7:   *pImage++ = *pImage | c8;
			case 6:   *pImage++ = *pImage | c8;
			case 5:   *pImage++ = *pImage | c8;
			case 4:   *pImage++ = *pImage | c8;
			case 3:   *pImage++ = *pImage | c8;
			case 2:   *pImage++ = *pImage | c8;
			case 1:   *pImage++ = *pImage | c8;
					}while (--w);
			}
			pImage += WidthBytes - wbak;
		}
		return S_TRUE;
	case 16:
		pImage += y * WidthBytes + (x<<1);
		c16 = (S_WORD)PatColor;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*(S_WORD*)pImage = *(S_WORD*)pImage | c16; pImage += 2;
			case 7:   *(S_WORD*)pImage = *(S_WORD*)pImage | c16; pImage += 2;
			case 6:   *(S_WORD*)pImage = *(S_WORD*)pImage | c16; pImage += 2;
			case 5:   *(S_WORD*)pImage = *(S_WORD*)pImage | c16; pImage += 2;
			case 4:   *(S_WORD*)pImage = *(S_WORD*)pImage | c16; pImage += 2;
			case 3:   *(S_WORD*)pImage = *(S_WORD*)pImage | c16; pImage += 2;
			case 2:   *(S_WORD*)pImage = *(S_WORD*)pImage | c16; pImage += 2;
			case 1:   *(S_WORD*)pImage = *(S_WORD*)pImage | c16; pImage += 2;
					}while (--w);
			}
			pImage += WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pImage += y * WidthBytes + x*3;
		r = (S_BYTE)PatColor;	g = (S_BYTE)(PatColor>>8); b = (S_BYTE)(PatColor>>16);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = *pImage | r; *pImage++ = *pImage | g; *pImage++ = *pImage | b;
			case 7:   *pImage++ = *pImage | r; *pImage++ = *pImage | g; *pImage++ = *pImage | b;
			case 6:   *pImage++ = *pImage | r; *pImage++ = *pImage | g; *pImage++ = *pImage | b;
			case 5:   *pImage++ = *pImage | r; *pImage++ = *pImage | g; *pImage++ = *pImage | b;
			case 4:   *pImage++ = *pImage | r; *pImage++ = *pImage | g; *pImage++ = *pImage | b;
			case 3:   *pImage++ = *pImage | r; *pImage++ = *pImage | g; *pImage++ = *pImage | b;
			case 2:   *pImage++ = *pImage | r; *pImage++ = *pImage | g; *pImage++ = *pImage | b;
			case 1:   *pImage++ = *pImage | r; *pImage++ = *pImage | g; *pImage++ = *pImage | b;
					}while (--w);
			}
			pImage += WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt2_Patcopy(S_BYTE *pImage, S_WORD BitsPixel, S_WORD WidthBytes, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_COLOR PatColor)
{
	S_BYTE c8, r, g, b;
	S_WORD c16;
	S_SHORT wbak = w;
	
	switch (BitsPixel)
	{
	case 4:
		pImage += y * WidthBytes + (x>>1);
		c8 = (S_BYTE)(PatColor<<4) | (S_BYTE)(PatColor&0xF);
		while (h--)
		{
			w = wbak;
			if (w && x&0x1)
			{
				*pImage = (*pImage & 0xF0) | (c8 & 0xF);
				pImage++;
				w--;
			}
			for (;w > 1; w -= 2)
			{
				*pImage++ = c8;
			}
			if (w == 1)
			{
				*pImage = (*pImage&0xF) | (c8 & 0xF0);
				pImage++;
			}
			pImage += WidthBytes - ((wbak+1)>>1);
			if (x&0x1 && !(wbak&0x1))
				pImage -= 1;
		}
		return S_TRUE;
	case 8:
		pImage += y * WidthBytes + x;
		c8 = (S_BYTE)PatColor;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = c8;
			case 7:   *pImage++ = c8;
			case 6:   *pImage++ = c8;
			case 5:   *pImage++ = c8;
			case 4:   *pImage++ = c8;
			case 3:   *pImage++ = c8;
			case 2:   *pImage++ = c8;
			case 1:   *pImage++ = c8;
					}while (--w);
			}
			pImage += WidthBytes - wbak;
		}
		return S_TRUE;
	case 16:
		pImage += y * WidthBytes + (x<<1);
		c16 = (S_WORD)PatColor;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*(S_WORD*)pImage = c16; pImage += 2;
			case 7:   *(S_WORD*)pImage = c16; pImage += 2;
			case 6:   *(S_WORD*)pImage = c16; pImage += 2;
			case 5:   *(S_WORD*)pImage = c16; pImage += 2;
			case 4:   *(S_WORD*)pImage = c16; pImage += 2;
			case 3:   *(S_WORD*)pImage = c16; pImage += 2;
			case 2:   *(S_WORD*)pImage = c16; pImage += 2;
			case 1:   *(S_WORD*)pImage = c16; pImage += 2;
					}while (--w);
			}
			pImage += WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pImage += y * WidthBytes + x*3;
		r = (S_BYTE)PatColor; g = (S_BYTE)(PatColor>>8); b = (S_BYTE)(PatColor>>16);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pImage++ = r; *pImage++ = g; *pImage++ = b;
			case 7:   *pImage++ = r; *pImage++ = g; *pImage++ = b;
			case 6:   *pImage++ = r; *pImage++ = g; *pImage++ = b;
			case 5:   *pImage++ = r; *pImage++ = g; *pImage++ = b;
			case 4:   *pImage++ = r; *pImage++ = g; *pImage++ = b;
			case 3:   *pImage++ = r; *pImage++ = g; *pImage++ = b;
			case 2:   *pImage++ = r; *pImage++ = g; *pImage++ = b;
			case 1:   *pImage++ = r; *pImage++ = g; *pImage++ = b;
					}while (--w);
			}
			pImage += WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt2(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, S_DWORD dwRop)
{
	S_BYTE *pDst;
	_SP_BITMAP pBmp;

	if (!hdcDst || HAI_GETHDLTYPE(hdcDst) != HT_DC)
		return S_FALSE;
	if (xDst >= ((SP_DC)hdcDst)->cx || yDst >= ((SP_DC)hdcDst)->cy)
		return S_FALSE;
	if (!((SP_DC)hdcDst)->lpImage || !((SP_DC)hdcDst)->hBitmap)
		return S_FALSE;

	if (xDst < 0) xDst = 0;
	if (yDst < 0) yDst = 0;
	if (w <= 0) w = ((SP_DC)hdcDst)->cx;
	if (h <= 0) h = ((SP_DC)hdcDst)->cy;
	w = HAI_MIN(w, ((SP_DC)hdcDst)->cx-xDst);
	h = HAI_MIN(h, ((SP_DC)hdcDst)->cy-yDst);
	xDst += ((SP_DC)hdcDst)->x;
	yDst += ((SP_DC)hdcDst)->y;
	pBmp = (_SP_BITMAP)((SP_DC)hdcDst)->hBitmap;
	pDst  = (S_BYTE*)((SP_DC)hdcDst)->lpImage;

	switch (dwRop)
	{
	case BLACKNESS:
		return __BitBlt2_Blackness(pDst, pBmp->BitsPixel, pBmp->WidthBytes, xDst, yDst, w, h);
	case WHITENESS:
		return __BitBlt2_Whiteness(pDst, pBmp->BitsPixel, pBmp->WidthBytes, xDst, yDst, w, h);
	case DSTINVERT:
		return __BitBlt2_Dstinvert(pDst, pBmp->BitsPixel, pBmp->WidthBytes, xDst, yDst, w, h);

	case PATINVERT:
		if ( !((SP_DC)hdcDst)->hBrush || ((_SP_BRUSH)(((SP_DC)hdcDst)->hBrush))->style == BS_NULL )
			return S_FALSE;
		return __BitBlt2_Patinvert(pDst, pBmp->BitsPixel, pBmp->WidthBytes, xDst, yDst, w, h, ((_SP_BRUSH)(((SP_DC)hdcDst)->hBrush))->color);

	case PATPAINT:
		if ( !((SP_DC)hdcDst)->hBrush || ((_SP_BRUSH)(((SP_DC)hdcDst)->hBrush))->style == BS_NULL )
			return S_FALSE;
		return __BitBlt2_Patpaint(pDst, pBmp->BitsPixel, pBmp->WidthBytes, xDst, yDst, w, h, ((_SP_BRUSH)(((SP_DC)hdcDst)->hBrush))->color);

	case PATCOPY:
		if ( !((SP_DC)hdcDst)->hBrush || ((_SP_BRUSH)(((SP_DC)hdcDst)->hBrush))->style == BS_NULL )
			return S_FALSE;
		return __BitBlt2_Patcopy(pDst, pBmp->BitsPixel, pBmp->WidthBytes, xDst, yDst, w, h, ((_SP_BRUSH)(((SP_DC)hdcDst)->hBrush))->color);

	default:
		break;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt1_NotSrcCopy(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, 
										SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc)
{
	S_SHORT wbak = w;
	S_BYTE *pDst = ((SP_DC)hdcDst)->lpImage;
	S_BYTE *pSrc = ((SP_DC)hdcSrc)->lpImage;
	_SP_BITMAP pBmpDst = (_SP_BITMAP)((SP_DC)hdcDst)->hBitmap;
	_SP_BITMAP pBmpSrc = (_SP_BITMAP)((SP_DC)hdcSrc)->hBitmap;

	xDst += ((SP_DC)hdcDst)->x;
	yDst += ((SP_DC)hdcDst)->y;
	xSrc += ((SP_DC)hdcSrc)->x;
	ySrc += ((SP_DC)hdcSrc)->y;
	
	switch (pBmpDst->BitsPixel)
	{
	case 4:
		xDst &= ~0x1; xSrc &= ~0x1;
		wbak &= ~0x1; wbak >>= 1;
		pDst += yDst * pBmpDst->WidthBytes + (xDst>>1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc>>1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ = ~*pSrc++;
			case 7:   *pDst++ = ~*pSrc++;
			case 6:   *pDst++ = ~*pSrc++;
			case 5:   *pDst++ = ~*pSrc++;
			case 4:   *pDst++ = ~*pSrc++;
			case 3:   *pDst++ = ~*pSrc++;
			case 2:   *pDst++ = ~*pSrc++;
			case 1:   *pDst++ = ~*pSrc++;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak;
			pSrc += pBmpSrc->WidthBytes - wbak;
		}
		return S_TRUE;
	case 8:
		pDst += yDst * pBmpDst->WidthBytes + xDst;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ = ~*pSrc++;
			case 7:   *pDst++ = ~*pSrc++;
			case 6:   *pDst++ = ~*pSrc++;
			case 5:   *pDst++ = ~*pSrc++;
			case 4:   *pDst++ = ~*pSrc++;
			case 3:   *pDst++ = ~*pSrc++;
			case 2:   *pDst++ = ~*pSrc++;
			case 1:   *pDst++ = ~*pSrc++;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak;
			pSrc += pBmpSrc->WidthBytes - wbak;
		}
		return S_TRUE;
	case 16:
		pDst += yDst * pBmpDst->WidthBytes + (xDst<<1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc<<1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*(S_WORD*)pDst = ~*(S_WORD*)pSrc; pDst += 2;pSrc += 2;
			case 7:   *(S_WORD*)pDst = ~*(S_WORD*)pSrc; pDst += 2;pSrc += 2;
			case 6:   *(S_WORD*)pDst = ~*(S_WORD*)pSrc; pDst += 2;pSrc += 2;
			case 5:   *(S_WORD*)pDst = ~*(S_WORD*)pSrc; pDst += 2;pSrc += 2;
			case 4:   *(S_WORD*)pDst = ~*(S_WORD*)pSrc; pDst += 2;pSrc += 2;
			case 3:   *(S_WORD*)pDst = ~*(S_WORD*)pSrc; pDst += 2;pSrc += 2;
			case 2:   *(S_WORD*)pDst = ~*(S_WORD*)pSrc; pDst += 2;pSrc += 2;
			case 1:   *(S_WORD*)pDst = ~*(S_WORD*)pSrc; pDst += 2;pSrc += 2;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - (wbak<<1);
			pSrc += pBmpSrc->WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pDst += yDst * pBmpDst->WidthBytes + xDst*3;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc*3;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++;
			case 7:   *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++;
			case 6:   *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++;
			case 5:   *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++;
			case 4:   *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++;
			case 3:   *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++;
			case 2:   *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++;
			case 1:   *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++; *pDst++ = ~*pSrc++;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak*3;
			pSrc += pBmpSrc->WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt1_SrcAnd(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, 
										SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc)
{
	S_SHORT wbak = w;
	S_BYTE *pDst = ((SP_DC)hdcDst)->lpImage;
	S_BYTE *pSrc = ((SP_DC)hdcSrc)->lpImage;
	_SP_BITMAP pBmpDst = (_SP_BITMAP)((SP_DC)hdcDst)->hBitmap;
	_SP_BITMAP pBmpSrc = (_SP_BITMAP)((SP_DC)hdcSrc)->hBitmap;

	xDst += ((SP_DC)hdcDst)->x;
	yDst += ((SP_DC)hdcDst)->y;
	xSrc += ((SP_DC)hdcSrc)->x;
	ySrc += ((SP_DC)hdcSrc)->y;
	
	switch (pBmpDst->BitsPixel)
	{
	case 4:
		xDst &= ~0x1; xSrc &= ~0x1;
		wbak &= ~0x1; wbak >>= 1;
		pDst += yDst * pBmpDst->WidthBytes + (xDst>>1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc>>1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ &= *pSrc++;
			case 7:   *pDst++ &= *pSrc++;
			case 6:   *pDst++ &= *pSrc++;
			case 5:   *pDst++ &= *pSrc++;
			case 4:   *pDst++ &= *pSrc++;
			case 3:   *pDst++ &= *pSrc++;
			case 2:   *pDst++ &= *pSrc++;
			case 1:   *pDst++ &= *pSrc++;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak;
			pSrc += pBmpSrc->WidthBytes - wbak;
		}
		return S_TRUE;
	case 8:
		pDst += yDst * pBmpDst->WidthBytes + xDst;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ &= *pSrc++;
			case 7:   *pDst++ &= *pSrc++;
			case 6:   *pDst++ &= *pSrc++;
			case 5:   *pDst++ &= *pSrc++;
			case 4:   *pDst++ &= *pSrc++;
			case 3:   *pDst++ &= *pSrc++;
			case 2:   *pDst++ &= *pSrc++;
			case 1:   *pDst++ &= *pSrc++;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak;
			pSrc += pBmpSrc->WidthBytes - wbak;
		}
		return S_TRUE;
	case 16:
		pDst += yDst * pBmpDst->WidthBytes + (xDst<<1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc<<1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*(S_WORD*)pDst &= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 7:   *(S_WORD*)pDst &= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 6:   *(S_WORD*)pDst &= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 5:   *(S_WORD*)pDst &= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 4:   *(S_WORD*)pDst &= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 3:   *(S_WORD*)pDst &= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 2:   *(S_WORD*)pDst &= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 1:   *(S_WORD*)pDst &= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - (wbak<<1);
			pSrc += pBmpSrc->WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pDst += yDst * pBmpDst->WidthBytes + xDst*3;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc*3;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ &= *pSrc++; *pDst++ &= *pSrc++; *pDst++ &= *pSrc++;
			case 7:   *pDst++ &= *pSrc++; *pDst++ &= *pSrc++; *pDst++ &= *pSrc++;
			case 6:   *pDst++ &= *pSrc++; *pDst++ &= *pSrc++; *pDst++ &= *pSrc++;
			case 5:   *pDst++ &= *pSrc++; *pDst++ &= *pSrc++; *pDst++ &= *pSrc++;
			case 4:   *pDst++ &= *pSrc++; *pDst++ &= *pSrc++; *pDst++ &= *pSrc++;
			case 3:   *pDst++ &= *pSrc++; *pDst++ &= *pSrc++; *pDst++ &= *pSrc++;
			case 2:   *pDst++ &= *pSrc++; *pDst++ &= *pSrc++; *pDst++ &= *pSrc++;
			case 1:   *pDst++ &= *pSrc++; *pDst++ &= *pSrc++; *pDst++ &= *pSrc++;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak*3;
			pSrc += pBmpSrc->WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt1_SrcCopy(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, 
										SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc)
{
	S_SHORT wbak = w;
	S_BYTE *pDst = ((SP_DC)hdcDst)->lpImage;
	S_BYTE *pSrc = ((SP_DC)hdcSrc)->lpImage;
	_SP_BITMAP pBmpDst = (_SP_BITMAP)((SP_DC)hdcDst)->hBitmap;
	_SP_BITMAP pBmpSrc = (_SP_BITMAP)((SP_DC)hdcSrc)->hBitmap;

	xDst += ((SP_DC)hdcDst)->x;
	yDst += ((SP_DC)hdcDst)->y;
	xSrc += ((SP_DC)hdcSrc)->x;
	ySrc += ((SP_DC)hdcSrc)->y;
	
	switch (pBmpDst->BitsPixel)
	{
	case 4:
		xDst &= ~0x1; xSrc &= ~0x1;
		wbak &= ~0x1; wbak >>= 1;
		pDst += yDst * pBmpDst->WidthBytes + (xDst>>1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc>>1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ = *pSrc++;
			case 7:   *pDst++ = *pSrc++;
			case 6:   *pDst++ = *pSrc++;
			case 5:   *pDst++ = *pSrc++;
			case 4:   *pDst++ = *pSrc++;
			case 3:   *pDst++ = *pSrc++;
			case 2:   *pDst++ = *pSrc++;
			case 1:   *pDst++ = *pSrc++;
					}while (--w);
			}
			
			pDst += pBmpDst->WidthBytes - wbak;
			pSrc += pBmpSrc->WidthBytes - wbak;
		}
		return S_TRUE;
	case 8:
		pDst += yDst * pBmpDst->WidthBytes + xDst;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ = *pSrc++;
			case 7:   *pDst++ = *pSrc++;
			case 6:   *pDst++ = *pSrc++;
			case 5:   *pDst++ = *pSrc++;
			case 4:   *pDst++ = *pSrc++;
			case 3:   *pDst++ = *pSrc++;
			case 2:   *pDst++ = *pSrc++;
			case 1:   *pDst++ = *pSrc++;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak;
			pSrc += pBmpSrc->WidthBytes - wbak;
		}
		return S_TRUE;
	case 16:
		pDst += yDst * pBmpDst->WidthBytes + (xDst<<1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc<<1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 7:   *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 6:   *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 5:   *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 4:   *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 3:   *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 2:   *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 1:   *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - (wbak<<1);
			pSrc += pBmpSrc->WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pDst += yDst * pBmpDst->WidthBytes + xDst*3;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc*3;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ = *pSrc++; *pDst++ = *pSrc++; *pDst++ = *pSrc++;
			case 7:   *pDst++ = *pSrc++; *pDst++ = *pSrc++; *pDst++ = *pSrc++;
			case 6:   *pDst++ = *pSrc++; *pDst++ = *pSrc++; *pDst++ = *pSrc++;
			case 5:   *pDst++ = *pSrc++; *pDst++ = *pSrc++; *pDst++ = *pSrc++;
			case 4:   *pDst++ = *pSrc++; *pDst++ = *pSrc++; *pDst++ = *pSrc++;
			case 3:   *pDst++ = *pSrc++; *pDst++ = *pSrc++; *pDst++ = *pSrc++;
			case 2:   *pDst++ = *pSrc++; *pDst++ = *pSrc++; *pDst++ = *pSrc++;
			case 1:   *pDst++ = *pSrc++; *pDst++ = *pSrc++; *pDst++ = *pSrc++;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak*3;
			pSrc += pBmpSrc->WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt1_SrcInvert(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, 
										SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc)
{
	S_SHORT wbak = w;
	S_BYTE *pDst = ((SP_DC)hdcDst)->lpImage;
	S_BYTE *pSrc = ((SP_DC)hdcSrc)->lpImage;
	_SP_BITMAP pBmpDst = (_SP_BITMAP)((SP_DC)hdcDst)->hBitmap;
	_SP_BITMAP pBmpSrc = (_SP_BITMAP)((SP_DC)hdcSrc)->hBitmap;

	xDst += ((SP_DC)hdcDst)->x;
	yDst += ((SP_DC)hdcDst)->y;
	xSrc += ((SP_DC)hdcSrc)->x;
	ySrc += ((SP_DC)hdcSrc)->y;
	
	switch (pBmpDst->BitsPixel)
	{
	case 4:
		xDst &= ~0x1; xSrc &= ~0x1;
		wbak &= ~0x1; wbak >>= 1;
		pDst += yDst * pBmpDst->WidthBytes + (xDst>>1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc>>1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ ^= *pSrc++;
			case 7:   *pDst++ ^= *pSrc++;
			case 6:   *pDst++ ^= *pSrc++;
			case 5:   *pDst++ ^= *pSrc++;
			case 4:   *pDst++ ^= *pSrc++;
			case 3:   *pDst++ ^= *pSrc++;
			case 2:   *pDst++ ^= *pSrc++;
			case 1:   *pDst++ ^= *pSrc++;
					}while (--w);
			}

			pDst += pBmpDst->WidthBytes - wbak;
			pSrc += pBmpSrc->WidthBytes - wbak;
		}
		return S_TRUE;
	case 8:
		pDst += yDst * pBmpDst->WidthBytes + xDst;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ ^= *pSrc++;
			case 7:   *pDst++ ^= *pSrc++;
			case 6:   *pDst++ ^= *pSrc++;
			case 5:   *pDst++ ^= *pSrc++;
			case 4:   *pDst++ ^= *pSrc++;
			case 3:   *pDst++ ^= *pSrc++;
			case 2:   *pDst++ ^= *pSrc++;
			case 1:   *pDst++ ^= *pSrc++;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak;
			pSrc += pBmpSrc->WidthBytes - wbak;
		}
		return S_TRUE;
	case 16:
		pDst += yDst * pBmpDst->WidthBytes + (xDst<<1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc<<1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*(S_WORD*)pDst ^= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 7:   *(S_WORD*)pDst ^= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 6:   *(S_WORD*)pDst ^= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 5:   *(S_WORD*)pDst ^= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 4:   *(S_WORD*)pDst ^= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 3:   *(S_WORD*)pDst ^= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 2:   *(S_WORD*)pDst ^= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 1:   *(S_WORD*)pDst ^= *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - (wbak<<1);
			pSrc += pBmpSrc->WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pDst += yDst * pBmpDst->WidthBytes + xDst*3;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc*3;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{*pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++;
			case 7:   *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++;
			case 6:   *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++;
			case 5:   *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++;
			case 4:   *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++;
			case 3:   *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++;
			case 2:   *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++;
			case 1:   *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++; *pDst++ ^= *pSrc++;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak*3;
			pSrc += pBmpSrc->WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt1(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, 
				SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc, S_DWORD dwRop)
{
	if (!hdcDst || HAI_GETHDLTYPE(hdcDst) != HT_DC || !hdcSrc || HAI_GETHDLTYPE(hdcSrc) != HT_DC)
		return S_FALSE;
	if (((_SP_BITMAP)(((SP_DC)hdcSrc)->hBitmap))->BitsPixel != ((_SP_BITMAP)(((SP_DC)hdcSrc)->hBitmap))->BitsPixel)
		return S_FALSE;
	if (xDst >= ((SP_DC)hdcDst)->cx || yDst >= ((SP_DC)hdcDst)->cy)
		return S_FALSE;
	if (xSrc >= ((SP_DC)hdcSrc)->cx || ySrc>= ((SP_DC)hdcSrc)->cy)
		return S_FALSE;
	if (!((SP_DC)hdcDst)->lpImage || !((SP_DC)hdcSrc)->lpImage)
		return S_FALSE;

	if (xSrc < 0) xSrc = 0;
	if (ySrc < 0) ySrc = 0;
	if (xDst < 0) xDst = 0;
	if (yDst < 0) yDst = 0;
	if (w <= 0) w = ((SP_DC)hdcSrc)->cx;
	w = HAI_MIN(w, ((SP_DC)hdcSrc)->cx-xSrc);
	w = HAI_MIN(w, ((SP_DC)hdcDst)->cx-xDst);
	if (h <= 0) h = ((SP_DC)hdcSrc)->cy;
	h = HAI_MIN(h, ((SP_DC)hdcSrc)->cy-ySrc);
	h = HAI_MIN(h, ((SP_DC)hdcDst)->cy-yDst);

	switch (dwRop)
	{
	case NOTSRCCOPY:
		return __BitBlt1_NotSrcCopy(hdcDst, xDst, yDst, w, h, hdcSrc, xSrc, ySrc);
	case SRCAND:
		return __BitBlt1_SrcAnd(hdcDst, xDst, yDst, w, h, hdcSrc, xSrc, ySrc);
	case SRCCOPY:
		return __BitBlt1_SrcCopy(hdcDst, xDst, yDst, w, h, hdcSrc, xSrc, ySrc);
	case SRCINVERT:
		return __BitBlt1_SrcInvert(hdcDst, xDst, yDst, w, h, hdcSrc, xSrc, ySrc);
	default:
		break;
	}

	return S_FALSE;
}

#define BRIGHT_BLEND_16_565(sc, dc, val)		\
	do{		\
		S_SHORT r = (S_SHORT)((sc) & 0x1F);		\
		S_SHORT g = (S_SHORT)((sc)>>5 & 0x3F);	\
		S_SHORT b = (S_SHORT)((sc)>>11 & 0x1F);	\
		r += (val), g += (val), b += (val);		\
		if (r>0x1F) r=0x1F;		\
		else if (r<0) r = 0;			\
		if (g>0x3F) g = 0x3F;		\
		else if (g<0) g = 0;		\
		if (b>0x1F) b = 0x1F;		\
		else if (b<0) b = 0;		\
		(dc) = (S_WORD)(r|g<<5|b<<11);		\
	}while (0)

#define BRIGHT_BLEND_24(ps, pd, val)		\
	do{		\
		S_SHORT r = (S_SHORT)(*((ps)+0));		\
		S_SHORT g = (S_SHORT)(*((ps)+1));		\
		S_SHORT b = (S_SHORT)(*((ps)+2));	\
		r += (val), g += (val), b += (val);		\
		if (r>0xFF) r=0xFF;		\
		else if (r<0) r = 0;			\
		if (g>0xFF) g = 0xFF;		\
		else if (g<0) g = 0;		\
		if (b>0xFF) b = 0xFF;		\
		else if (b<0) b = 0;		\
		*((pd)+0) = (S_BYTE)r;		\
		*((pd)+1) = (S_BYTE)g;		\
		*((pd)+2) = (S_BYTE)b;		\
	}while (0)

static S_BOOL __BitBlt3_BrightCopy(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, 
										SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc)
{
	S_SHORT wbak = w;
	S_SHORT bright = (S_SHORT)((SP_DC)hdcSrc)->ColorKey;
	S_BYTE *pDst = ((SP_DC)hdcDst)->lpImage;
	S_BYTE *pSrc = ((SP_DC)hdcSrc)->lpImage;
	_SP_BITMAP pBmpDst = (_SP_BITMAP)((SP_DC)hdcDst)->hBitmap;
	_SP_BITMAP pBmpSrc = (_SP_BITMAP)((SP_DC)hdcSrc)->hBitmap;

	xDst += ((SP_DC)hdcDst)->x;
	yDst += ((SP_DC)hdcDst)->y;
	xSrc += ((SP_DC)hdcSrc)->x;
	ySrc += ((SP_DC)hdcSrc)->y;
	
	switch (pBmpDst->BitsPixel)
	{
	case 16:
		bright >>= 3;
		pDst += yDst * pBmpDst->WidthBytes + (xDst<<1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc<<1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{BRIGHT_BLEND_16_565(*(S_WORD*)pSrc, *(S_WORD*)pDst, bright); pDst += 2; pSrc += 2;
			case 7:   BRIGHT_BLEND_16_565(*(S_WORD*)pSrc, *(S_WORD*)pDst, bright); pDst += 2; pSrc += 2;
			case 6:   BRIGHT_BLEND_16_565(*(S_WORD*)pSrc, *(S_WORD*)pDst, bright); pDst += 2; pSrc += 2;
			case 5:   BRIGHT_BLEND_16_565(*(S_WORD*)pSrc, *(S_WORD*)pDst, bright); pDst += 2; pSrc += 2;
			case 4:   BRIGHT_BLEND_16_565(*(S_WORD*)pSrc, *(S_WORD*)pDst, bright); pDst += 2; pSrc += 2;
			case 3:   BRIGHT_BLEND_16_565(*(S_WORD*)pSrc, *(S_WORD*)pDst, bright); pDst += 2; pSrc += 2;
			case 2:   BRIGHT_BLEND_16_565(*(S_WORD*)pSrc, *(S_WORD*)pDst, bright); pDst += 2; pSrc += 2;
			case 1:   BRIGHT_BLEND_16_565(*(S_WORD*)pSrc, *(S_WORD*)pDst, bright); pDst += 2; pSrc += 2;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - (wbak<<1);
			pSrc += pBmpSrc->WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pDst += yDst * pBmpDst->WidthBytes + xDst*3;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc*3;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{BRIGHT_BLEND_24(pSrc, pDst, bright); pSrc += 3; pDst += 3;
			case 7:   BRIGHT_BLEND_24(pSrc, pDst, bright); pSrc += 3; pDst += 3;
			case 6:   BRIGHT_BLEND_24(pSrc, pDst, bright); pSrc += 3; pDst += 3;
			case 5:   BRIGHT_BLEND_24(pSrc, pDst, bright); pSrc += 3; pDst += 3;
			case 4:   BRIGHT_BLEND_24(pSrc, pDst, bright); pSrc += 3; pDst += 3;
			case 3:   BRIGHT_BLEND_24(pSrc, pDst, bright); pSrc += 3; pDst += 3;
			case 2:   BRIGHT_BLEND_24(pSrc, pDst, bright); pSrc += 3; pDst += 3;
			case 1:   BRIGHT_BLEND_24(pSrc, pDst, bright); pSrc += 3; pDst += 3;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak*3;
			pSrc += pBmpSrc->WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

#define ALPHA_BLEND_16_565(ps, pd, alpha)	\
	do {						\
		S_DWORD src = *(S_WORD*)(ps);			\
		S_DWORD dst = *(S_WORD*)(pd);			\
		src = (src | src << 16) & 0x07e0f81f;		\
		dst = (dst | dst << 16) & 0x07e0f81f;		\
		dst += (src - dst) * (alpha) >> 5;			\
		dst &= 0x07e0f81f;				\
		*(S_WORD*)(pd) = (S_WORD)(dst | dst >> 16);			\
		} while(0)

#define ALPHA_BLEND_24(ps, pd, alpha)	\
	do {						\
		S_WORD sr = *((ps)+0);			\
		S_WORD sg = *((ps)+1);			\
		S_WORD sb = *((ps)+2);			\
		S_WORD dr = *((pd)+0);			\
		S_WORD dg = *((pd)+1);			\
		S_WORD db = *((pd)+2);			\
		dr += (sr-dr)*(alpha) >> 8;			\
		dg += (sg-dg)*(alpha) >> 8;			\
		db += (sb-db)*(alpha) >> 8;			\
		*((pd)+0) = (S_BYTE)dr;			\
		*((pd)+1) = (S_BYTE)dg;			\
		*((pd)+2) = (S_BYTE)db;			\
		} while(0)

static S_BOOL __BitBlt3_AlphaCopy(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, 
										SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc)
{
	S_BYTE a = ((SP_DC)hdcSrc)->Alpha;
	S_SHORT wbak = w;
	S_BYTE *pDst = ((SP_DC)hdcDst)->lpImage;
	S_BYTE *pSrc = ((SP_DC)hdcSrc)->lpImage;
	_SP_BITMAP pBmpDst = (_SP_BITMAP)((SP_DC)hdcDst)->hBitmap;
	_SP_BITMAP pBmpSrc = (_SP_BITMAP)((SP_DC)hdcSrc)->hBitmap;

	xDst += ((SP_DC)hdcDst)->x;
	yDst += ((SP_DC)hdcDst)->y;
	xSrc += ((SP_DC)hdcSrc)->x;
	ySrc += ((SP_DC)hdcSrc)->y;
	
	switch (pBmpDst->BitsPixel)
	{
	case 16:
		pDst += yDst * pBmpDst->WidthBytes + (xDst<<1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc<<1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 7:   ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 6:   ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 5:   ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 4:   ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 3:   ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 2:   ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 1:   ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - (wbak<<1);
			pSrc += pBmpSrc->WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pDst += yDst * pBmpDst->WidthBytes + xDst*3;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc*3;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 7:   ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 6:   ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 5:   ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 4:   ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 3:   ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 2:   ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 1:   ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak*3;
			pSrc += pBmpSrc->WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

#define TRANS_COPY_24(pSrc, pDst, ColorKey)		\
	do{		\
		S_DWORD CurColor = ((S_DWORD)(*((pSrc)+2))<<16)|((S_DWORD)(*((pSrc)+1))<<8)|((S_DWORD)(*(pSrc)));	\
		if (CurColor != (ColorKey))	\
		{	\
			*((pDst)+0) = *((pSrc)+0);	\
			*((pDst)+1) = *((pSrc)+1);	\
			*((pDst)+2) = *((pSrc)+2);	\
		}	\
	}while (0)

static S_BOOL __BitBlt3_TransCopy(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, 
										SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc)
{
	S_SHORT wbak = w;
	S_BYTE ColorKey8 = (S_BYTE)((SP_DC)hdcSrc)->ColorKey;
	S_WORD ColorKey16 = (S_WORD)((SP_DC)hdcSrc)->ColorKey;
	S_DWORD ColorKey24 = (S_DWORD)((SP_DC)hdcSrc)->ColorKey;
	S_BYTE *pDst = ((SP_DC)hdcDst)->lpImage;
	S_BYTE *pSrc = ((SP_DC)hdcSrc)->lpImage;
	_SP_BITMAP pBmpDst = (_SP_BITMAP)((SP_DC)hdcDst)->hBitmap;
	_SP_BITMAP pBmpSrc = (_SP_BITMAP)((SP_DC)hdcSrc)->hBitmap;

	xDst += ((SP_DC)hdcDst)->x;
	yDst += ((SP_DC)hdcDst)->y;
	xSrc += ((SP_DC)hdcSrc)->x;
	ySrc += ((SP_DC)hdcSrc)->y;
	
	switch (pBmpDst->BitsPixel)
	{
	case 8:
		pDst += yDst * pBmpDst->WidthBytes + xDst;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{if (*pSrc != ColorKey8) *pDst = *pSrc; pDst++; pSrc++;
			case 7:   if (*pSrc != ColorKey8) *pDst = *pSrc; pDst++; pSrc++;
			case 6:   if (*pSrc != ColorKey8) *pDst = *pSrc; pDst++; pSrc++;
			case 5:   if (*pSrc != ColorKey8) *pDst = *pSrc; pDst++; pSrc++;
			case 4:   if (*pSrc != ColorKey8) *pDst = *pSrc; pDst++; pSrc++;
			case 3:   if (*pSrc != ColorKey8) *pDst = *pSrc; pDst++; pSrc++;
			case 2:   if (*pSrc != ColorKey8) *pDst = *pSrc; pDst++; pSrc++;
			case 1:   if (*pSrc != ColorKey8) *pDst = *pSrc; pDst++; pSrc++;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak;
			pSrc += pBmpSrc->WidthBytes - wbak;
		}
		return S_TRUE;
	case 16:
		pDst += yDst * pBmpDst->WidthBytes + (xDst<<1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc<<1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{if (*(S_WORD*)pSrc != ColorKey16) *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 7:   if (*(S_WORD*)pSrc != ColorKey16) *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 6:   if (*(S_WORD*)pSrc != ColorKey16) *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 5:   if (*(S_WORD*)pSrc != ColorKey16) *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 4:   if (*(S_WORD*)pSrc != ColorKey16) *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 3:   if (*(S_WORD*)pSrc != ColorKey16) *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 2:   if (*(S_WORD*)pSrc != ColorKey16) *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
			case 1:   if (*(S_WORD*)pSrc != ColorKey16) *(S_WORD*)pDst = *(S_WORD*)pSrc; pDst += 2; pSrc += 2;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - (wbak<<1);
			pSrc += pBmpSrc->WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pDst += yDst * pBmpDst->WidthBytes + xDst*3;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc*3;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{TRANS_COPY_24(pSrc, pDst, ColorKey24); pSrc += 3; pDst += 3;
			case 7:   TRANS_COPY_24(pSrc, pDst, ColorKey24); pSrc += 3; pDst += 3;
			case 6:   TRANS_COPY_24(pSrc, pDst, ColorKey24); pSrc += 3; pDst += 3;
			case 5:   TRANS_COPY_24(pSrc, pDst, ColorKey24); pSrc += 3; pDst += 3;
			case 4:   TRANS_COPY_24(pSrc, pDst, ColorKey24); pSrc += 3; pDst += 3;
			case 3:   TRANS_COPY_24(pSrc, pDst, ColorKey24); pSrc += 3; pDst += 3;
			case 2:   TRANS_COPY_24(pSrc, pDst, ColorKey24); pSrc += 3; pDst += 3;
			case 1:   TRANS_COPY_24(pSrc, pDst, ColorKey24); pSrc += 3; pDst += 3;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak*3;
			pSrc += pBmpSrc->WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt3_TransAlphaCpy(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, 
										SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc)
{
	S_BYTE a = ((SP_DC)hdcSrc)->Alpha;
	S_SHORT wbak = w;
	S_WORD ColorKey16 = (S_WORD)((SP_DC)hdcSrc)->ColorKey;
	S_DWORD ColorKey24 = (S_DWORD)((SP_DC)hdcSrc)->ColorKey;
	S_BYTE *pDst = ((SP_DC)hdcDst)->lpImage;
	S_BYTE *pSrc = ((SP_DC)hdcSrc)->lpImage;
	_SP_BITMAP pBmpDst = (_SP_BITMAP)((SP_DC)hdcDst)->hBitmap;
	_SP_BITMAP pBmpSrc = (_SP_BITMAP)((SP_DC)hdcSrc)->hBitmap;

	xDst += ((SP_DC)hdcDst)->x;
	yDst += ((SP_DC)hdcDst)->y;
	xSrc += ((SP_DC)hdcSrc)->x;
	ySrc += ((SP_DC)hdcSrc)->y;
	
	switch (pBmpDst->BitsPixel)
	{
	case 16:
		pDst += yDst * pBmpDst->WidthBytes + (xDst<<1);
		pSrc += ySrc * pBmpSrc->WidthBytes + (xSrc<<1);
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{if (*(S_WORD*)pSrc!=ColorKey16) ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 7:   if (*(S_WORD*)pSrc!=ColorKey16) ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 6:   if (*(S_WORD*)pSrc!=ColorKey16) ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 5:   if (*(S_WORD*)pSrc!=ColorKey16) ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 4:   if (*(S_WORD*)pSrc!=ColorKey16) ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 3:   if (*(S_WORD*)pSrc!=ColorKey16) ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 2:   if (*(S_WORD*)pSrc!=ColorKey16) ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
			case 1:   if (*(S_WORD*)pSrc!=ColorKey16) ALPHA_BLEND_16_565(pSrc, pDst, a); pDst += 2; pSrc += 2;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - (wbak<<1);
			pSrc += pBmpSrc->WidthBytes - (wbak<<1);
		}
		return S_TRUE;
	case 24:
		pDst += yDst * pBmpDst->WidthBytes + xDst*3;
		pSrc += ySrc * pBmpSrc->WidthBytes + xSrc*3;
		while (h--)
		{
			w = (wbak+7)>>3;
			switch (wbak%8)
			{
			case 0:do{if (HAI_MAKEDWORD3(pSrc)!=ColorKey24) ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 7:   if (HAI_MAKEDWORD3(pSrc)!=ColorKey24) ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 6:   if (HAI_MAKEDWORD3(pSrc)!=ColorKey24) ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 5:   if (HAI_MAKEDWORD3(pSrc)!=ColorKey24) ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 4:   if (HAI_MAKEDWORD3(pSrc)!=ColorKey24) ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 3:   if (HAI_MAKEDWORD3(pSrc)!=ColorKey24) ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 2:   if (HAI_MAKEDWORD3(pSrc)!=ColorKey24) ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
			case 1:   if (HAI_MAKEDWORD3(pSrc)!=ColorKey24) ALPHA_BLEND_24(pSrc, pDst, a); pSrc += 3; pDst += 3;
					}while (--w);
			}
			pDst += pBmpDst->WidthBytes - wbak*3;
			pSrc += pBmpSrc->WidthBytes - wbak*3;
		}
		return S_TRUE;
	default:
		return S_FALSE;
	}
	return S_FALSE;
}

static S_BOOL __BitBlt3(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, 
				SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc, S_DWORD dwRop)
{
	if (!hdcDst || HAI_GETHDLTYPE(hdcDst) != HT_DC || !hdcSrc || HAI_GETHDLTYPE(hdcSrc) != HT_DC)
		return S_FALSE;
	if (((_SP_BITMAP)(((SP_DC)hdcSrc)->hBitmap))->BitsPixel != ((_SP_BITMAP)(((SP_DC)hdcSrc)->hBitmap))->BitsPixel)
		return S_FALSE;
	if (xDst >= ((SP_DC)hdcDst)->cx || yDst >= ((SP_DC)hdcDst)->cy)
		return S_FALSE;
	if (xSrc >= ((SP_DC)hdcSrc)->cx || ySrc>= ((SP_DC)hdcSrc)->cy)
		return S_FALSE;
	if (!((SP_DC)hdcDst)->lpImage || !((SP_DC)hdcSrc)->lpImage)
		return S_FALSE;

	if (xSrc < 0) xSrc = 0;
	if (ySrc < 0) ySrc = 0;
	if (xDst < 0) xDst = 0;
	if (yDst < 0) yDst = 0;
	if (w <= 0) w = ((SP_DC)hdcSrc)->cx;
	w = HAI_MIN(w, ((SP_DC)hdcSrc)->cx-xSrc);
	w = HAI_MIN(w, ((SP_DC)hdcDst)->cx-xDst);
	if (h <= 0) h = ((SP_DC)hdcSrc)->cy;
	h = HAI_MIN(h, ((SP_DC)hdcSrc)->cy-ySrc);
	h = HAI_MIN(h, ((SP_DC)hdcDst)->cy-yDst);

	switch (dwRop)
	{
	case BRIGHTCOPY:
		return __BitBlt3_BrightCopy(hdcDst, xDst, yDst, w, h, hdcSrc, xSrc, ySrc);
	case ALPHACOPY:
		return __BitBlt3_AlphaCopy(hdcDst, xDst, yDst, w, h, hdcSrc, xSrc, ySrc);
	case TRANSCOPY:
		return __BitBlt3_TransCopy(hdcDst, xDst, yDst, w, h, hdcSrc, xSrc, ySrc);
	case TRANSALPHACPY:
		return __BitBlt3_TransAlphaCpy(hdcDst, xDst, yDst, w, h, hdcSrc, xSrc, ySrc);
	default:
		break;
	}

	return S_FALSE;
}

S_BOOL hai_BitBlt(SH_DC hdcDst, S_SHORT xDst, S_SHORT yDst, S_SHORT w, S_SHORT h, 
				SH_DC hdcSrc, S_SHORT xSrc, S_SHORT ySrc, S_DWORD dwRop)
{
	switch (dwRop)
	{
	case NOTSRCCOPY:
	case SRCAND:
	case SRCCOPY:
	case SRCINVERT:
		return __BitBlt1(hdcDst, xDst, yDst, w, h, hdcSrc, xSrc, ySrc, dwRop);

	case BLACKNESS:
	case DSTINVERT:
	case WHITENESS:	
	case PATINVERT:
	case PATCOPY:
	case PATPAINT:
		return __BitBlt2(hdcDst, xDst, yDst, w, h, dwRop);

	case BRIGHTCOPY:
	case ALPHACOPY:
	case TRANSCOPY:
	case TRANSALPHACPY:
		return __BitBlt3(hdcDst, xDst, yDst, w, h, hdcSrc, xSrc, ySrc, dwRop);
	
	default:
		break;
	}
	return S_FALSE;
}

static S_VOID _hai_StretchBlitLine(S_BYTE *pDstImage, S_WORD dw, S_BYTE *pSrcImage, S_WORD sw, S_WORD BitsPixel)
{
	S_WORD flag = 0xFFFF;
	S_WORD i, WidthCnt = 0;
	S_WORD *pwDst, *pwSrc;

	switch (BitsPixel)
	{
	case 4:
		for (i = 0; i < sw; i++)
		{
			for (WidthCnt += dw; WidthCnt >= sw; WidthCnt -= sw)
			{
				flag++;
				if (flag&0x1)// dst 2th pixel
				{
					if (i&0x1)//src 2th pixel
						*pDstImage = (*pDstImage&0xF) | (*pSrcImage&0xF0);
					else
						*pDstImage = (*pDstImage&0xF) | (*pSrcImage<<4);
					pDstImage++;
				}
				else // dst 1st pixel
				{
					if (i&0x1)//src 2th pixel
						*pDstImage = (*pDstImage&0xF0) | (*pSrcImage>>4);
					else
						*pDstImage = (*pDstImage&0xF0) | (*pSrcImage&0xF);
				}
			}
			if (i&0x1)
				pSrcImage++;
		}
		break;

	case 8:
		for (i = 0; i < sw; i++)
		{
			for (WidthCnt += dw; WidthCnt >= sw; WidthCnt -= sw)
			{
				*pDstImage++ = *pSrcImage;
			}
			pSrcImage++;
		}
		break;

	case 16:
		pwDst = (S_WORD *)pDstImage;
		pwSrc = (S_WORD *)pSrcImage;
		for (i = 0; i < sw; i++)
		{
			for (WidthCnt += dw; WidthCnt >= sw; WidthCnt -= sw)
			{
				*pwDst++ = *pwSrc;
			}
			pwSrc++;
		}
		break;

	case 24:
		for (i = 0; i < sw; i++)
		{
			for (WidthCnt += dw; WidthCnt >= sw; WidthCnt -= sw)
			{
				*pDstImage++ = *(pSrcImage+0);
				*pDstImage++ = *(pSrcImage+1);
				*pDstImage++ = *(pSrcImage+2);
			}
			pSrcImage += 3;
		}
		break;

	default:
		//todo;
		break;
	}
}

S_BOOL hai_StretchBlit(SH_DC hDst, S_WORD dx, S_WORD dy, S_WORD dw, S_WORD dh, SH_DC hSrc, S_WORD sx, S_WORD sy, S_WORD sw, S_WORD sh)
{
	S_WORD i, HeightCnt = 0;
	_SP_BITMAP pDstBmp, pSrcBmp;
	S_BYTE *pDstImage, *pSrcImage;

	if (!hDst || HAI_GETHDLTYPE(hDst) != HT_DC)
		return S_FALSE;
	if (!hSrc || HAI_GETHDLTYPE(hSrc) != HT_DC)
		return S_FALSE;

	pDstBmp = (_SP_BITMAP)(((SP_DC)hDst)->hBitmap);
	pSrcBmp = (_SP_BITMAP)(((SP_DC)hSrc)->hBitmap);
	if (!pDstBmp || !pSrcBmp)
		return S_FALSE;

	pDstImage = (S_BYTE *)(((SP_DC)hDst)->lpImage);
	pSrcImage = (S_BYTE *)(((SP_DC)hSrc)->lpImage);
	if (!pDstImage || !pSrcImage)
		return S_FALSE;

	if (pDstBmp->BitsPixel != pSrcBmp->BitsPixel)
		return S_FALSE;

	if (dx >= ((SP_DC)hDst)->cx || dy >= ((SP_DC)hDst)->cy)
		return S_FALSE;
	if (sx >= ((SP_DC)hSrc)->cx || sy >= ((SP_DC)hSrc)->cy)
		return S_FALSE;
	
	if (dw == 0) dw = ((SP_DC)hDst)->cx;
	if (dh == 0) dh = ((SP_DC)hDst)->cy;
	if (sw == 0) sw = ((SP_DC)hSrc)->cx;
	if (sh == 0) sh = ((SP_DC)hSrc)->cy;

	if (dx+dw > ((SP_DC)hDst)->cx)
		dw = ((SP_DC)hDst)->cx - dx;
	if (dy+dh > ((SP_DC)hDst)->cy)
		dh = ((SP_DC)hDst)->cy - dy;
	if (sx+sw > ((SP_DC)hSrc)->cx)
		sw = ((SP_DC)hSrc)->cx - sx;
	if (sy+sh > ((SP_DC)hSrc)->cy)
		sh = ((SP_DC)hSrc)->cy - sy;

	dx += ((SP_DC)hDst)->x;
	dy += ((SP_DC)hDst)->y;
	sx += ((SP_DC)hSrc)->x;
	sy += ((SP_DC)hSrc)->y;
	
	pDstImage += dy*pDstBmp->WidthBytes + dx*pDstBmp->BitsPixel/8;
	pSrcImage += sy*pSrcBmp->WidthBytes + sx*pSrcBmp->BitsPixel/8;

	for (i = 0; i < sh; i++)
	{
		for (HeightCnt += dh; HeightCnt >= sh; HeightCnt -= sh)
		{
			_hai_StretchBlitLine(pDstImage, dw, pSrcImage, sw, pSrcBmp->BitsPixel);
			pDstImage += pDstBmp->WidthBytes;
		}
		pSrcImage += pSrcBmp->WidthBytes;
	}

	return S_TRUE;
}

static S_BOOL _DrawChar6x12StyleFor4(SH_DC hDC, S_SHORT x, S_SHORT y, const S_BYTE *pMatAsc)
{
	S_BOOL  bkmode;
	S_CHAR  index, bits = 6;
	S_SHORT j, h = 12;
	S_BYTE  *pDst;
	const S_BYTE *p = pMatAsc;
	_SP_BITMAP pBmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;

	x += ((SP_DC)hDC)->x;
	y += ((SP_DC)hDC)->y;
	bkmode = (((SP_DC)hDC)->BkMode)? 1:0;
	pDst = ((S_BYTE*)((SP_DC)hDC)->lpImage) + y*pBmp->WidthBytes+ (x>>1);

	for (j = 0; j < h; j++)
	{
		index =  (*p >> bits) & 0x3;
		*pDst  =  *pDst & gFontMask[bkmode][index] | gFontMat[bkmode][index];
		bits  -= 2;
		pDst++;
		if (bits < 0)
		{
			p++;
			bits = 6;
		}
		index =  (*p >> bits) & 0x3;
		*pDst  =  *pDst & gFontMask[bkmode][index] | gFontMat[bkmode][index];
		bits  -= 2;
		pDst++;
		if (bits < 0)
		{
			p++;
			bits = 6;
		}
		index =  (*p >> bits) & 0x3;
		*pDst  =  *pDst & gFontMask[bkmode][index] | gFontMat[bkmode][index];
		bits  -= 2;
		pDst++;
		if (bits < 0)
		{
			p++;
			bits = 6;
		}
	
		pDst += pBmp->WidthBytes-3;
	}
	return S_TRUE;
}

static S_BOOL _DrawChar6x12StyleFor8(SH_DC hDC, S_SHORT x, S_SHORT y, const S_BYTE *pMatAsc)
{
	S_BOOL  bkmode;
	S_CHAR  bits = 7;
	S_BYTE  tColor, bColor;
	S_SHORT j, h = 12;
	S_BYTE  *pDst;
	const S_BYTE *p = pMatAsc;
	_SP_BITMAP pBmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;

	x += ((SP_DC)hDC)->x;
	y += ((SP_DC)hDC)->y;
	tColor = (S_BYTE)((SP_DC)hDC)->TextColor;
	bColor = (S_BYTE)((SP_DC)hDC)->BkColor;
	bkmode = (((SP_DC)hDC)->BkMode)? BKM_TRANSPARENT:BKM_OPAQUE;
	pDst = ((S_BYTE*)((SP_DC)hDC)->lpImage) + y*pBmp->WidthBytes + x;

	for (j = 0; j < h; j++)
	{
		if ((*p >> bits--) & 0x1) *pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if (bits < 0)
			p++, bits = 7;
		
		if ((*p >> bits--) & 0x1) *pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if (bits < 0)
			p++, bits = 7;
		
		if ((*p >> bits--) & 0x1) *pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if (bits < 0)
			p++, bits = 7;
	
		pDst += pBmp->WidthBytes-6;
	}
	return S_TRUE;
}

static S_BOOL _DrawChar6x12StyleFor16(SH_DC hDC, S_SHORT x, S_SHORT y, const S_BYTE *pMatAsc)
{
	S_BOOL  bkmode;
	S_CHAR  bits = 7;
	S_WORD  tColor, bColor;
	S_SHORT j, h = 12;
	S_BYTE  *pDst;
	const S_BYTE *p = pMatAsc;
	_SP_BITMAP pBmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;

	x += ((SP_DC)hDC)->x;
	y += ((SP_DC)hDC)->y;
	tColor = (S_WORD)((SP_DC)hDC)->TextColor;
	bColor = (S_WORD)((SP_DC)hDC)->BkColor;
	bkmode = (((SP_DC)hDC)->BkMode)? BKM_TRANSPARENT:BKM_OPAQUE;
	pDst = ((S_BYTE*)((SP_DC)hDC)->lpImage) + y*pBmp->WidthBytes + (x<<1);

	for (j = 0; j < h; j++)
	{
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if (bits < 0)
			p++, bits = 7;
		
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if (bits < 0)
			p++, bits = 7;
		
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  = tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if (bits < 0)
			p++, bits = 7;
	
		pDst += pBmp->WidthBytes-12;
	}
	return S_TRUE;
}

static S_BOOL _DrawChar6x12StyleFor24(SH_DC hDC, S_SHORT x, S_SHORT y, const S_BYTE *pMatAsc)
{
	S_BOOL  bkmode;
	S_CHAR  bits = 7;
	S_DWORD  tColor, bColor;
	S_SHORT j, h = 12;
	S_BYTE  *pDst;
	const S_BYTE *p = pMatAsc;
	_SP_BITMAP pBmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;

	x += ((SP_DC)hDC)->x;
	y += ((SP_DC)hDC)->y;
	tColor = (S_DWORD)((SP_DC)hDC)->TextColor;
	bColor = (S_DWORD)((SP_DC)hDC)->BkColor;
	bkmode = (((SP_DC)hDC)->BkMode)? BKM_TRANSPARENT:BKM_OPAQUE;
	pDst = ((S_BYTE*)((SP_DC)hDC)->lpImage) + y*pBmp->WidthBytes + x*3;

	for (j = 0; j < h; j++)
	{
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if (bits < 0)
			p++, bits = 7;
		
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if (bits < 0)
			p++, bits = 7;
		
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if (bits < 0)
			p++, bits = 7;
	
		pDst += pBmp->WidthBytes-18;
	}
	return S_TRUE;
}

static S_BOOL _DrawChar6x12_Style(SH_DC hDC, S_SHORT x, S_SHORT y, const S_BYTE *pMatAsc)
{
	S_BOOL ret;
	_SP_BITMAP pBmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;

	switch (pBmp->BitsPixel)
	{
	case 4:
		ret = _DrawChar6x12StyleFor4(hDC, x, y, pMatAsc);
		break;
	case 8:
		ret = _DrawChar6x12StyleFor8(hDC, x, y, pMatAsc);
		break;
	case 16:
		ret = _DrawChar6x12StyleFor16(hDC, x, y, pMatAsc);
		break;
	case 24:
		ret = _DrawChar6x12StyleFor24(hDC, x, y, pMatAsc);
		break;
	default:
		ret = S_FALSE;
		break;
	}
	return ret;
}

static S_BOOL _hai_DrawChar(SH_DC hDC, S_SHORT x, S_SHORT y, S_BYTE asc)
{
	S_BOOL ret;
	_SP_FONT pFont;

	pFont = (_SP_FONT)(((SP_DC)hDC)->hFont);
	if (x+pFont->width >= ((SP_DC)hDC)->cx || y+pFont->height >= ((SP_DC)hDC)->cy)
		return S_FALSE;

	switch (pFont->FontID)
	{
	case 1:
		ret = _DrawChar6x12_Style(hDC, x, y, &(Hai_MatAsc[asc][0]));
		break;
	case 2:
	case 3:
	default:
		ret = _DrawChar6x12_Style(hDC, x, y, &(Hai_MatAsc[asc][0]));
		break;
	}
	return ret;
}

static S_BOOL _hai_DrawFont12x12StyleFor4(SH_DC hDC, S_SHORT x, S_SHORT y, const S_BYTE *pMatHz)
{
	S_BOOL  bkmode;
	S_CHAR  index, bits = 6;
	S_SHORT j, h = 12;
	S_BYTE  *pDst;
	const S_BYTE *p = pMatHz;
	_SP_BITMAP pBmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;
	
	x += ((SP_DC)hDC)->x;
	y += ((SP_DC)hDC)->y;
	bkmode = (((SP_DC)hDC)->BkMode)? 1:0;
	pDst = ((S_BYTE*)((SP_DC)hDC)->lpImage) + y*pBmp->WidthBytes + (x>>1);

	for (j = 0; j < h; j++)
	{
		index =  (*p >> bits) & 0x3;
		*pDst  =  *pDst & gFontMask[bkmode][index] | gFontMat[bkmode][index];
		bits  -= 2;
		pDst++;
		index =  (*p >> bits) & 0x3;
		*pDst  =  *pDst & gFontMask[bkmode][index] | gFontMat[bkmode][index];
		bits  -= 2;
		pDst++;
		if (bits < 0)
		{
			p++;
			bits = 6;
		}
		index =  (*p >> bits) & 0x3;
		*pDst  =  *pDst & gFontMask[bkmode][index] | gFontMat[bkmode][index];
		bits  -= 2;
		pDst++;
		index =  (*p >> bits) & 0x3;
		*pDst  =  *pDst & gFontMask[bkmode][index] | gFontMat[bkmode][index];
		bits  -= 2;
		pDst++;
		if (bits < 0)
		{
			p++;
			bits = 6;
		}
		index =  (*p >> bits) & 0x3;
		*pDst  =  *pDst & gFontMask[bkmode][index] | gFontMat[bkmode][index];
		bits  -= 2;
		pDst++;
		index =  (*p >> bits) & 0x3;
		*pDst  =  *pDst & gFontMask[bkmode][index] | gFontMat[bkmode][index];
		bits  -= 2;
		pDst++;
		if (bits < 0)
		{
			p++;
			bits = 6;
		}
		pDst += pBmp->WidthBytes-6;
	}
	return S_TRUE;
}

static S_BOOL _hai_DrawFont12x12StyleFor8(SH_DC hDC, S_SHORT x, S_SHORT y, const S_BYTE *pMatHz)
{
	S_BOOL  bkmode;
	S_CHAR  bits = 7;
	S_BYTE  tColor, bColor;
	S_SHORT j, h = 12;
	S_BYTE  *pDst;
	const S_BYTE *p = pMatHz;
	_SP_BITMAP pBmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;

	x += (S_BYTE)((SP_DC)hDC)->x;
	y += (S_BYTE)((SP_DC)hDC)->y;
	tColor = (S_BYTE)((SP_DC)hDC)->TextColor;
	bColor = (S_BYTE)((SP_DC)hDC)->BkColor;
	bkmode = (((SP_DC)hDC)->BkMode)? BKM_TRANSPARENT:BKM_OPAQUE;
	pDst = ((S_BYTE*)((SP_DC)hDC)->lpImage) + y*pBmp->WidthBytes + x;

	for (j = 0; j < h; j++)
	{
		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if (bits < 0)
			p++, bits = 7;

		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if (bits < 0)
			p++, bits = 7;

		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if ((*p >> bits--) & 0x1) *pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *pDst = bColor;
		pDst++;
		if (bits < 0)
			p++, bits = 7;

		pDst += pBmp->WidthBytes-12;
	}
	return S_TRUE;
}

static S_BOOL _hai_DrawFont12x12StyleFor16(SH_DC hDC, S_SHORT x, S_SHORT y, const S_BYTE *pMatHz)
{
	S_BOOL  bkmode;
	S_CHAR  bits = 7;
	S_WORD  tColor, bColor;
	S_SHORT j, h = 12;
	S_BYTE  *pDst;
	const S_BYTE *p = pMatHz;
	_SP_BITMAP pBmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;

	x += ((SP_DC)hDC)->x;
	y += ((SP_DC)hDC)->y;
	tColor = (S_WORD)((SP_DC)hDC)->TextColor;
	bColor = (S_WORD)((SP_DC)hDC)->BkColor;
	bkmode = (((SP_DC)hDC)->BkMode)? BKM_TRANSPARENT:BKM_OPAQUE;
	pDst = ((S_BYTE*)((SP_DC)hDC)->lpImage) + y*pBmp->WidthBytes + (x<<1);

	for (j = 0; j < h; j++)
	{
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if (bits < 0)
			p++, bits = 7;

		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if (bits < 0)
			p++, bits = 7;

		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if ((*p >> bits--) & 0x1) *(S_WORD*)pDst  =  tColor;
		else if (bkmode == BKM_OPAQUE) *(S_WORD*)pDst = bColor;
		pDst += 2;
		if (bits < 0)
			p++, bits = 7;

		pDst += pBmp->WidthBytes-24;
	}
	return S_TRUE;
}

static S_BOOL _hai_DrawFont12x12StyleFor24(SH_DC hDC, S_SHORT x, S_SHORT y, const S_BYTE *pMatHz)
{
	S_BOOL  bkmode;
	S_CHAR  bits = 7;
	S_DWORD  tColor, bColor;
	S_SHORT j, h = 12;
	S_BYTE  *pDst;
	const S_BYTE *p = pMatHz;
	_SP_BITMAP pBmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;

	x += ((SP_DC)hDC)->x;
	y += ((SP_DC)hDC)->y;
	tColor = (S_DWORD)((SP_DC)hDC)->TextColor;
	bColor = (S_DWORD)((SP_DC)hDC)->BkColor;
	bkmode = (((SP_DC)hDC)->BkMode)? BKM_TRANSPARENT:BKM_OPAQUE;
	pDst = ((S_BYTE*)((SP_DC)hDC)->lpImage) + y*pBmp->WidthBytes + x*3;

	for (j = 0; j < h; j++)
	{
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if (bits < 0)
			p++, bits = 7;
		
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if (bits < 0)
			p++, bits = 7;
		
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if ((*p >> bits--) & 0x1) COPYPIXEL_24(pDst, tColor);
		else if (bkmode == BKM_OPAQUE) COPYPIXEL_24(pDst, bColor);
		pDst += 3;
		if (bits < 0)
			p++, bits = 7;
	
		pDst += pBmp->WidthBytes-36;
	}
	return S_TRUE;
}

static S_BOOL _hai_DrawFont12x12_Style(SH_DC hDC, S_SHORT x, S_SHORT y, const S_BYTE *pMatHz)
{
	S_BOOL ret;
	_SP_BITMAP pBmp = (_SP_BITMAP)((SP_DC)hDC)->hBitmap;

	switch (pBmp->BitsPixel)
	{
	case 4:
		ret = _hai_DrawFont12x12StyleFor4(hDC, x, y, pMatHz);
		break;
	case 8:
		ret = _hai_DrawFont12x12StyleFor8(hDC, x, y, pMatHz);
		break;
	case 16:
		ret = _hai_DrawFont12x12StyleFor16(hDC, x, y, pMatHz);
		break;
	case 24:
		ret = _hai_DrawFont12x12StyleFor24(hDC, x, y, pMatHz);
		break;
	default:
		ret = S_FALSE;
		break;
	}
	return ret;
}

static S_BOOL _hai_DrawHz(SH_DC hDC, S_SHORT x, S_SHORT y, S_WORD hz)
{
	_SP_FONT pFont;
	const S_BYTE (*p)[18];

	pFont = (_SP_FONT)(((SP_DC)hDC)->hFont);
	if (x+pFont->width*2 >= ((SP_DC)hDC)->cx || y+pFont->height >= ((SP_DC)hDC)->cy)
		return S_FALSE;

	switch (pFont->FontID)
	{
	case 1:
		if((hz&0xff) < 0xa1)
		{
			if(hz>=0xed40u)  hz-=0xed40u, p = Hai_MatHz_0xED40_0xFEA0;
			else if(hz>=0xdb40u)  hz-=0xdb40u, p = Hai_MatHz_0xDB40_0xECA0;
			else if(hz>=0xc940u)  hz-=0xc940u, p = Hai_MatHz_0xC940_0xDAA0;
			else if(hz>=0xb740u)  hz-=0xb740u, p = Hai_MatHz_0xB740_0xC8A0;
			else if(hz>=0xa540u)  hz-=0xa540u, p = Hai_MatHz_0xA540_0xB6A0;
			else if(hz>=0x9340u)  hz-=0x9340u, p = Hai_MatHz_0x9340_0xA4A0;
			else if(hz>=0x8140u)  hz-=0x8140u, p = Hai_MatHz_0x8140_0x92A0;
			else return S_FALSE;
			return _hai_DrawFont12x12_Style(hDC, x, y, p[((hz>>8)&0xff)*97+(hz&0xff)]);
		} 
		else 
		{
			if(hz>=0xeda1u)  hz-=0xeda1u, p = Hai_MatHz_0xEDA1_0xFEFE;
			else if(hz>=0xdba1u)  hz-=0xdba1u, p = Hai_MatHz_0xDBA1_0xECFE;
			else if(hz>=0xc9a1u)  hz-=0xc9a1u, p = Hai_MatHz_0xC9A1_0xDAFE;
			else if(hz>=0xb7a1u)  hz-=0xb7a1u, p = Hai_MatHz_0xB7A1_0xC8FE;
			else if(hz>=0xa5a1u)  hz-=0xa5a1u, p = Hai_MatHz_0xA5A1_0xB6FE;
			else if(hz>=0x93a1u)  hz-=0x93a1u, p = Hai_MatHz_0x93A1_0xA4FE;
			else if(hz>=0x81a1u)  hz-=0x81a1u, p = Hai_MatHz_0x81A1_0x92FE;
			else return S_FALSE;
			return _hai_DrawFont12x12_Style(hDC, x, y, p[((hz>>8)&0xff)*94+(hz&0xff)]);
		}
	case 2:
	case 3:
	default:
		break;
	}
	return S_FALSE;
}

S_BOOL hai_TextOut(SH_DC hDC, S_SHORT x, S_SHORT y, S_BYTE *lpString, S_SHORT cbstring)
{
	S_SHORT X;
	S_BYTE  c0, c1;
	S_BYTE  offset = 0;
	_SP_FONT pFont;
	S_BOOL  flag;

	if (!hDC || HAI_GETHDLTYPE(hDC) != HT_DC)
		return S_FALSE;
	if (!((SP_DC)hDC)->lpImage || !((SP_DC)hDC)->hBitmap)
		return S_FALSE;
	if (!lpString || !*lpString)
		return S_FALSE;
	if (!((SP_DC)hDC)->hFont)
		return S_FALSE;

	pFont = (_SP_FONT)(((SP_DC)hDC)->hFont);
	if ((pFont->style & FS_BOLD) || (pFont->style & FS_ITALIC))
		offset = 1;
	flag = cbstring? 1:0;

	for (X = x; *lpString; lpString++)
	{
		if (flag)
		{
			if (!(cbstring--))
				break;
		}
		c0 = *lpString;
		if (c0 == '\n')
		{
			y += pFont->height+2;
			X  = x;
		}
		else if(c0 & 0x80)
		{//是一个双字节字
			c1 = lpString[1] ;
			if(!c1) break;
			_hai_DrawHz(hDC, X, y, (S_WORD)((c0<<8)|c1));
			X += pFont->width * 2 + offset;			
			lpString++;
		}
		else
		{//是一个字符
			_hai_DrawChar(hDC, X, y, c0);
			X += pFont->width + offset;
		}
	}
	return S_TRUE;
}

S_SHORT hai_DrawText(SH_DC hDC, S_BYTE *lpString, S_SHORT nCount, /*LPRECT lpRect,*/S_DWORD uFormat)
{
	return 0;
}


