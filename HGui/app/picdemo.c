//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			picdemo.c
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
#include "dialog.h"


DEFINESCENE(SID_PICALPHA)
MSGMAPBEGIN
	ONCREAT(PicAlphaCreat)
	ONMSG(EVT_PAINT, PicAlphaPaint)
	ONUSERMSG(EVT_TIMER, PicAlphaTimer)
	ONKEYUP(PicAlphaKey)
	ONMSG(EVT_EXIT, PicAlphaExit)
	ONDESTROY(PicAlphaDestroy)
MSGMAPEND

//
//
typedef struct tagPicAData
{
	S_SHORT valA, valX1, valY1, valX2, valY2;
	S_SHORT alpha, x1, y1, x2, y2;
	SH_BITMAP hBK, hPic1, hPic2;
}
S_PICADATA, *SP_PICADATA;

#define PICA_PIC_W	86
#define PICA_PIC_H	176


static S_VOID PicAlphaCreat(SP_EVT pEvent)
{
	SP_PICADATA pSceneDat = hai_GetSceneData(sizeof(*pSceneDat));

	if (!pSceneDat)
	{
		hai_PostEvent(EVT_DESTROY, 0, 0);
		return;
	}
	memset(pSceneDat, 0x00, sizeof(*pSceneDat));
	pSceneDat->hBK   = hai_LoadBitmap(WINOS_RES_PATH"backgrund.bmp");
	pSceneDat->hPic1 = hai_LoadBitmap(WINOS_RES_PATH"dict3.bmp");
	pSceneDat->hPic2 = hai_LoadBitmap(WINOS_RES_PATH"tool.bmp");
	if (!pSceneDat->hBK || !pSceneDat->hPic1 || !pSceneDat->hPic2)
	{
		hai_PostEvent(EVT_DESTROY, 0, 0);
		return;
	}
	pSceneDat->alpha = 255;
	pSceneDat->x1 = 0;
	pSceneDat->y1 = 0;
	pSceneDat->x2 = LCD_WIDTH-PICA_PIC_W;
	pSceneDat->y2 = LCD_HEIGHT-PICA_PIC_H;
	pSceneDat->valA = -2;
	pSceneDat->valX1 = 2;
	pSceneDat->valY1 = 1;
	pSceneDat->valX2 = -2;
	pSceneDat->valY2 = -1;
}

static S_VOID PicAlphaPaint(SP_EVT pEvent)
{
	SP_PICADATA pSceneDat = hai_GetSceneData(sizeof(*pSceneDat));
	S_PAINTSTRUCT Paint;
	SH_DC hDC, hDCmem;

	hDC = hai_BeginPaint(&Paint);
	hDCmem = hai_CreateCompatibleDC(hDC);
	hai_SelectObject(hDCmem, (SH_GDIOBJ)pSceneDat->hBK);
	hai_BitBlt(hDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, hDCmem, 0, 0, SRCCOPY);
	hai_SetAlphaVal(hDCmem, (S_BYTE)pSceneDat->alpha);
	hai_SelectObject(hDCmem, (SH_GDIOBJ)pSceneDat->hPic1);
	hai_BitBlt(hDC, pSceneDat->x1, pSceneDat->y1, PICA_PIC_W, PICA_PIC_H, hDCmem, 0, 0, ALPHACOPY);
	hai_SelectObject(hDCmem, (SH_GDIOBJ)pSceneDat->hPic2);
	hai_BitBlt(hDC, pSceneDat->x2, pSceneDat->y2, PICA_PIC_W, PICA_PIC_H, hDCmem, 0, 0, ALPHACOPY);
	hai_DeleteDC(hDCmem);
	hai_EndPaint(&Paint);
}

static S_VOID PicAlphaTimer(SP_EVT pEvent)
{
	SP_PICADATA pSceneDat = hai_GetSceneData(sizeof(*pSceneDat));

	pSceneDat->alpha += pSceneDat->valA;
	if (pSceneDat->alpha < 0)
	{
		pSceneDat->alpha = 0;
		pSceneDat->valA = -pSceneDat->valA;
	}
	else if (pSceneDat->alpha > 255)
	{
		pSceneDat->alpha = 255;
		pSceneDat->valA = -pSceneDat->valA;
	}
	pSceneDat->x1 += pSceneDat->valX1;
	if (pSceneDat->x1 < 0)
	{
		pSceneDat->x1 = 0;
		pSceneDat->valX1 = -pSceneDat->valX1;
	}
	else if (pSceneDat->x1 > LCD_WIDTH-PICA_PIC_W)
	{
		pSceneDat->x1 = LCD_WIDTH-PICA_PIC_W;
		pSceneDat->valX1 = -pSceneDat->valX1;
	}
	pSceneDat->x2 += pSceneDat->valX2;
	if (pSceneDat->x2 < 0)
	{
		pSceneDat->x2 = 0;
		pSceneDat->valX2 = -pSceneDat->valX2;
	}
	else if (pSceneDat->x2 > LCD_WIDTH-PICA_PIC_W)
	{
		pSceneDat->x2 = LCD_WIDTH-PICA_PIC_W;
		pSceneDat->valX2 = -pSceneDat->valX2;
	}
	pSceneDat->y1 += pSceneDat->valY1;
	if (pSceneDat->y1 < 0)
	{
		pSceneDat->y1 = 0;
		pSceneDat->valY1 = -pSceneDat->valY1;
	}
	else if (pSceneDat->y1 > LCD_HEIGHT-PICA_PIC_H)
	{
		pSceneDat->y1 = LCD_HEIGHT-PICA_PIC_H;
		pSceneDat->valY1 = -pSceneDat->valY1;
	}
	pSceneDat->y2 += pSceneDat->valY2;
	if (pSceneDat->y2 < 0)
	{
		pSceneDat->y2 = 0;
		pSceneDat->valY2 = -pSceneDat->valY2;
	}
	else if (pSceneDat->y2 > LCD_HEIGHT-PICA_PIC_H)
	{
		pSceneDat->y2 = LCD_HEIGHT-PICA_PIC_H;
		pSceneDat->valY2 = -pSceneDat->valY2;
	}
	hai_InvalidRect(S_NULL);
}

S_VOID GetNameDlgProc(SP_EVT pEvt);

static S_VOID PicAlphaKey(SP_EVT pEvent)
{
	if (pEvent->wParam == VK_ESCAPE)
		hai_PostEvent(EVT_DESTROY, 0, 0);
	else if (pEvent->wParam == '3')
		HG_DialogBox(140, 90, 180, 90, GetNameDlgProc);
}

static S_VOID PicAlphaExit(SP_EVT pEvent)
{
	SP_PICADATA pSceneDat = hai_GetSceneData(sizeof(*pSceneDat));

	if (pSceneDat->hBK)
		hai_DeleteObject((SH_GDIOBJ)pSceneDat->hBK);
	if (pSceneDat->hPic1)
		hai_DeleteObject((SH_GDIOBJ)pSceneDat->hPic1);
	if (pSceneDat->hPic2)
		hai_DeleteObject((SH_GDIOBJ)pSceneDat->hPic2);

	hai_ReleaseSceneData();
}

static S_VOID PicAlphaDestroy(SP_EVT pEvent)
{
	PicAlphaExit(pEvent);
	RETURNSCENE(0, 0);
}

DEFINESCENE(SID_PICPARALL)
MSGMAPBEGIN
	ONCREAT(ParallCreat)
	ONMSG(EVT_PAINT, ParallPaint)
	ONUSERMSG(EVT_TIMER, ParallTimer)
	ONKEYUP(ParallKey)
	ONMSG(EVT_EXIT, ParallExit)
	ONDESTROY(ParallDestroy)
MSGMAPEND

#define	TILE_W		48
#define	TILE_H		48
#define	MAP_W		16
#define	MAP_H		16

static const S_BYTE fgmap[MAP_H][MAP_W] = {
//	 0123456789ABCDEF
	"3333333333333333",
	"3   2   3      3",
	"3   222 3  222 3",
	"3333 22     22 3",
	"3       222    3",
	"3   222 2 2  333",
	"3   2 2 222    3",
	"3   222      223",
	"3        333   3",
	"3  22 23 323  23",
	"3  22 32 333  23",
	"3            333",
	"3 3  22 33     3",
	"3    222  2  3 3",
	"3  3     3   3 3",
	"3333333333333333"
};

static const S_BYTE midmap[MAP_H][MAP_W] = {
//	 0123456789ABCDEF
	"   1    1       ",
	"           1   1",
	"  1             ",
	"     1  1    1  ",
	"   1            ",
	"         1      ",
	" 1            1 ",
	"    1   1       ",
	"          1     ",
	"   1            ",
	"        1    1  ",
	" 1          1   ",
	"     1          ",
	"        1       ",
	"  1        1    ",
	"                "
};

static const S_BYTE bgmap[MAP_H][MAP_W] = {
//	 0123456789ABCDEF
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000",
	"0000000000000000"
};

typedef struct tagParallData
{
	S_SHORT fx, fy, mx, my, bx, by;
	S_SHORT fxval, fyval, mxval, myval, bxval, byval;
	SH_BITMAP htile;
}
S_PARALLDATA, *SP_PARALLDATA;

S_VOID hai_DrawTiles(SH_DC hDst, SH_DC hSrc, S_SHORT x, S_SHORT y, S_BYTE (*map)[MAP_W], S_DWORD rop)
{
	S_BYTE flag;
	S_SHORT m, n;
	S_SHORT w, h;
	S_SHORT i, j;
	S_SHORT xb;

	for (i = 0; i < LCD_HEIGHT; i += h)
	{
		xb = x;
		n = y % TILE_H;
		h = TILE_H - n;
		for (j = 0; j < LCD_WIDTH; j += w)
		{
			flag = map[y/TILE_H][xb/TILE_W];
			m = xb % TILE_W;
			w = TILE_W - m;
			xb += w;
			if (flag != ' ')
				hai_BitBlt(hDst, j, i, w, h, hSrc, m, (S_SHORT)(((flag-'0')*TILE_H)+n), rop);
		}
		y += h;
	}
}


static S_VOID ParallCreat(SP_EVT pEvent)
{
	SP_PARALLDATA pDat = hai_GetSceneData(sizeof(*pDat));
	
	if (!pDat)
	{
		hai_PostEvent(EVT_DESTROY, 0, 0);
		return;
	}
	memset(pDat, 0x00, sizeof(*pDat));
	pDat->htile = hai_LoadBitmap(WINOS_RES_PATH"tiles.bmp");
	if (pDat->htile == S_NULL)
	{
		hai_PostEvent(EVT_DESTROY, 0, 0);
		return;
	}
	pDat->bxval = 1;
	pDat->byval = 1;
	pDat->mxval = 2;
	pDat->myval = 1;
	pDat->fxval = 3;
	pDat->fyval = 2;
}

static S_VOID ParallPaint(SP_EVT pEvent)
{
	SP_PARALLDATA pSceneDat = hai_GetSceneData(sizeof(*pSceneDat));
	S_PAINTSTRUCT Paint;
	SH_DC hDC, hDCmem;

	hDC = hai_BeginPaint(&Paint);
	hDCmem = hai_CreateCompatibleDC(hDC);
	hai_SelectObject(hDCmem, (SH_GDIOBJ)pSceneDat->htile);
	hai_SetColorKey(hDCmem, 0xFF00FF);
	hai_DrawTiles(hDC, hDCmem, pSceneDat->bx, pSceneDat->by, (S_BYTE(*)[MAP_W])bgmap, SRCCOPY);
	hai_DrawTiles(hDC, hDCmem, pSceneDat->mx, pSceneDat->my, (S_BYTE(*)[MAP_W])midmap, TRANSCOPY);
	hai_DrawTiles(hDC, hDCmem, pSceneDat->fx, pSceneDat->fy, (S_BYTE(*)[MAP_W])fgmap, SRCCOPY);
	hai_BitBlt(hDC, 0, 0, TILE_W, TILE_H, hDCmem, 0, (S_SHORT)(4*TILE_H), TRANSCOPY);
	hai_DeleteDC(hDCmem);
	hai_EndPaint(&Paint);
}

static S_VOID ParallTimer(SP_EVT pEvent)
{
	SP_PARALLDATA pSceneDat = hai_GetSceneData(sizeof(*pSceneDat));
	S_SHORT totalw = TILE_W*MAP_W, totalh = TILE_H*MAP_H;

	pSceneDat->bx += pSceneDat->bxval;
	if (pSceneDat->bx < 0)
	{
		pSceneDat->bx = 1;
		pSceneDat->bxval = -pSceneDat->bxval;
	}
	else if (pSceneDat->bx > totalw-LCD_WIDTH)
	{
		pSceneDat->bx = totalw-LCD_WIDTH-1;
		pSceneDat->bxval = -pSceneDat->bxval;
	}
	pSceneDat->by += pSceneDat->byval;
	if (pSceneDat->by < 0)
	{
		pSceneDat->by = 1;
		pSceneDat->byval = -pSceneDat->byval;
	}
	else if (pSceneDat->by > totalh-LCD_HEIGHT)
	{
		pSceneDat->by = totalh-LCD_HEIGHT-1;
		pSceneDat->byval = -pSceneDat->byval;
	}
	pSceneDat->mx += pSceneDat->mxval;
	if (pSceneDat->mx < 0)
	{
		pSceneDat->mx = 1;
		pSceneDat->mxval = -pSceneDat->mxval;
	}
	else if (pSceneDat->mx > totalw-LCD_WIDTH)
	{
		pSceneDat->mx = totalw-LCD_WIDTH-1;
		pSceneDat->mxval = -pSceneDat->mxval;
	}
	pSceneDat->my += pSceneDat->myval;
	if (pSceneDat->my < 0)
	{
		pSceneDat->my = 1;
		pSceneDat->myval = -pSceneDat->myval;
	}
	else if (pSceneDat->my > totalh-LCD_HEIGHT)
	{
		pSceneDat->my = totalh-LCD_HEIGHT-1;
		pSceneDat->myval = -pSceneDat->myval;
	}
	pSceneDat->fx += pSceneDat->fxval;
	if (pSceneDat->fx < 0)
	{
		pSceneDat->fx = 1;
		pSceneDat->fxval = -pSceneDat->fxval;
	}
	else if (pSceneDat->fx > totalw-LCD_WIDTH)
	{
		pSceneDat->fx = totalw-LCD_WIDTH-1;
		pSceneDat->fxval = -pSceneDat->fxval;
	}
	pSceneDat->fy += pSceneDat->fyval;
	if (pSceneDat->fy < 0)
	{
		pSceneDat->fy = 1;
		pSceneDat->fyval = -pSceneDat->fyval;
	}
	else if (pSceneDat->fy > totalh-LCD_HEIGHT)
	{
		pSceneDat->fy = totalh-LCD_HEIGHT-1;
		pSceneDat->fyval = -pSceneDat->fyval;
	}
	hai_InvalidRect(S_NULL);
}

static S_VOID ParallKey(SP_EVT pEvent)
{
	if (pEvent->wParam == VK_ESCAPE)
		hai_PostEvent(EVT_DESTROY, 0, 0);
}

static S_VOID ParallExit(SP_EVT pEvent)
{
	SP_PARALLDATA pDat = hai_GetSceneData(sizeof(*pDat));

	hai_DeleteObject((SH_GDIOBJ)pDat->htile);
	hai_ReleaseSceneData();
}

static S_VOID ParallDestroy(SP_EVT pEvent)
{
	ParallExit(pEvent);
	RETURNSCENE(0, 0);
}


DEFINESCENE(SID_PICSTRETCH)
MSGMAPBEGIN
	ONCREAT(stretchCreat)
	ONMSG(EVT_PAINT, stretchPaint)
	ONUSERMSG(EVT_TIMER, stretchTimer)
	ONKEYUP(stretchKey)
	ONMSG(EVT_EXIT, stretchExit)
	ONDESTROY(stretchDestroy)
MSGMAPEND

#define PS_MSGBOX_W		200
#define PS_MSGBOX_H		100

typedef struct tagPicStretch
{
	S_BYTE FirstPaint;
	S_BYTE ArtBox;
	SH_BITMAP hbk, hbox;
}
S_PICSTRETCH, *SP_PICSTRETCH;

static S_VOID stretchCreat(SP_EVT pEvent)
{
	SP_PICSTRETCH pDat = hai_GetSceneData(sizeof(*pDat));
	
	if (!pDat)
	{
		hai_PostEvent(EVT_DESTROY, 0, 0);
		return;
	}
	memset(pDat, 0x00, sizeof(*pDat));
	pDat->hbk  = hai_LoadBitmap(WINOS_RES_PATH"aquaria.bmp");
	pDat->hbox = hai_LoadBitmap(WINOS_RES_PATH"artbox.bmp");
	if (pDat->hbk == S_NULL || pDat->hbox == S_NULL)
	{
		hai_PostEvent(EVT_DESTROY, 0, 0);
		return;
	}
	pDat->FirstPaint = 1;
	pDat->ArtBox = 0;
}

static S_VOID stretchPaint(SP_EVT pEvent)
{
	S_WORD i;
	SP_PICSTRETCH pSceneDat = hai_GetSceneData(sizeof(*pSceneDat));
	S_PAINTSTRUCT Paint;
	SH_DC hDC, hDCmem;

	hDC = hai_BeginPaint(&Paint);
	hDCmem = hai_CreateCompatibleDC(hDC);
	hai_SelectObject(hDCmem, (SH_GDIOBJ)pSceneDat->hbk);
	if (pSceneDat->FirstPaint)
	{
		pSceneDat->FirstPaint = 0;
		for (i = 15; i < 256; i += 20)
		{
			hai_BitBlt(hDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, S_NULL, 0, 0, BLACKNESS);
			hai_SetAlphaVal(hDCmem, (S_BYTE)i);
			hai_BitBlt(hDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, hDCmem, 0, 0, ALPHACOPY);
			hai_UpdateWindow(0, 0, LCD_WIDTH, LCD_HEIGHT);
			hai_Delay(1);
		}
	}
	else
	{
		hai_BitBlt(hDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, hDCmem, 0, 0, SRCCOPY);
		if (pSceneDat->ArtBox)
		{
			hai_SelectObject(hDCmem, (SH_GDIOBJ)pSceneDat->hbox);
			hai_SetColorKey(hDCmem, 0xFFFFFF);
			hai_BitBlt(hDC, (LCD_WIDTH-260)>>1, ((LCD_HEIGHT-110)>>1)-20, 260, 110, hDCmem, 0, 0, TRANSCOPY);
		}
	}
	hai_DeleteDC(hDCmem);
	hai_EndPaint(&Paint);
}

static S_VOID stretchTimer(SP_EVT pEvent)
{
}

static S_VOID stretchKey(SP_EVT pEvent)
{
	S_WORD i;
	SH_DC hDC, hDst, hSrc;
	SH_BITMAP hBmp;
	SP_PICSTRETCH pSceneDat = hai_GetSceneData(sizeof(*pSceneDat));

	switch (pEvent->wParam)
	{
	case VK_ESCAPE:
		if (pSceneDat->ArtBox)
			hai_SendEvent(EVT_KEYUP, 'n', 0);
		else
		{
			hDC = hai_GetSceneDC();
			hDst = hai_CreateCompatibleDC(hDC);
			hSrc = hai_CreateCompatibleDC(hDC);
			hBmp = hai_CreateCompatibleBitmap(hDC);
			hai_SelectObject(hSrc, (SH_GDIOBJ)pSceneDat->hbox);
			hai_SelectObject(hDst, (SH_GDIOBJ)hBmp);
			hai_SetColorKey(hDst, 0xFFFFFF);
			for (i = 1; i < 6; i++)
			{
				hai_StretchBlit(hDst, 0, 0, (S_SHORT)(i*52), (S_SHORT)(i*22), hSrc, 0, 0, 0, 0);
				hai_BitBlt(hDC, (S_SHORT)((LCD_WIDTH-i*52)>>1), (S_SHORT)(((LCD_HEIGHT-i*22)>>1)-20), (S_SHORT)(i*52), (S_SHORT)(i*22), hDst, 0, 0, TRANSCOPY);
				hai_UpdateWindow((S_SHORT)((LCD_WIDTH-i*52)>>1), (S_SHORT)(((LCD_HEIGHT-i*22)>>1)-20), (S_SHORT)(i*52), (S_SHORT)(i*22));
				hai_Delay(1);
			}
			hai_ReleaseDC(hDC);
			hai_DeleteDC(hDst);
			hai_DeleteDC(hSrc);
			hai_DeleteObject((SH_GDIOBJ)hBmp);
			pSceneDat->ArtBox = 1;
		}
		break;
	case 'y':
	case 'Y':
		if (pSceneDat->ArtBox)
			hai_PostEvent(EVT_DESTROY, 0, 0);
		break;
	case 'n':
	case 'N':
		if (pSceneDat->ArtBox)
		{
			hDC = hai_GetSceneDC();
			hDst = hai_CreateCompatibleDC(hDC);
			hSrc = hai_CreateCompatibleDC(hDC);
			hBmp = hai_CreateCompatibleBitmap(hDC);
			hai_SelectObject(hDst, (SH_GDIOBJ)hBmp);
			hai_SetColorKey(hDst, 0xFFFFFF);
			for (i = 4; i > 0; i--)
			{
				hai_SelectObject(hSrc, (SH_GDIOBJ)pSceneDat->hbk);
				hai_BitBlt(hDC, (S_SHORT)((LCD_WIDTH-(i+1)*52)>>1), (S_SHORT)(((LCD_HEIGHT-(i+1)*22)>>1)-20), (S_SHORT)((i+1)*52), (S_SHORT)((i+1)*22), hSrc, (S_SHORT)((LCD_WIDTH-(i+1)*52)>>1), (S_SHORT)(((LCD_HEIGHT-(i+1)*22)>>1)-20), SRCCOPY);
				hai_SelectObject(hSrc, (SH_GDIOBJ)pSceneDat->hbox);
				hai_StretchBlit(hDst, 0, 0, (S_SHORT)(i*52), (S_SHORT)(i*22), hSrc, 0, 0, 0, 0);
				hai_BitBlt(hDC, (S_SHORT)((LCD_WIDTH-i*52)>>1), (S_SHORT)(((LCD_HEIGHT-i*22)>>1)-20), (S_SHORT)(i*52), (S_SHORT)(i*22), hDst, 0, 0, TRANSCOPY);
				hai_UpdateWindow((S_SHORT)((LCD_WIDTH-(i+1)*52)>>1), (S_SHORT)(((LCD_HEIGHT-(i+1)*22)>>1)-20), (S_SHORT)((i+1)*52), (S_SHORT)((i+1)*22));
				hai_Delay(1);
			}
			hai_SelectObject(hSrc, (SH_GDIOBJ)pSceneDat->hbk);
			hai_BitBlt(hDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, hSrc, 0, 0, SRCCOPY);
			hai_UpdateWindow((LCD_WIDTH-52)>>1, ((LCD_HEIGHT-22)>>1)-20, 52, 22);
			hai_ReleaseDC(hDC);
			hai_DeleteDC(hDst);
			hai_DeleteDC(hSrc);
			hai_DeleteObject((SH_GDIOBJ)hBmp);
			pSceneDat->ArtBox = 0;
		}
		break;
	default:
		break;
	}
}

static S_VOID stretchExit(SP_EVT pEvent)
{
	SP_PICSTRETCH pSceneDat = hai_GetSceneData(sizeof(*pSceneDat));

	hai_DeleteObject((SH_GDIOBJ)pSceneDat->hbk);
	hai_DeleteObject((SH_GDIOBJ)pSceneDat->hbox);
	hai_ReleaseSceneData();
}

static S_VOID stretchDestroy(SP_EVT pEvent)
{
	S_WORD i;
	SH_DC hDC, hDCmem;
	SP_PICSTRETCH pSceneDat = hai_GetSceneData(sizeof(*pSceneDat));

	hai_SendEvent(EVT_KEYUP, 'n', 0);//msgbox
	hDC = hai_GetSceneDC();
	hDCmem = hai_CreateCompatibleDC(hDC);
	hai_SelectObject(hDCmem, (SH_GDIOBJ)pSceneDat->hbk);
	for (i = 255; i > 20; i -= 20)
	{
		hai_BitBlt(hDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, S_NULL, 0, 0, BLACKNESS);
		hai_SetAlphaVal(hDCmem, (S_BYTE)i);
		hai_BitBlt(hDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, hDCmem, 0, 0, ALPHACOPY);
		hai_UpdateWindow(0, 0, LCD_WIDTH, LCD_HEIGHT);
		hai_Delay(1);
	}
	hai_ReleaseDC(hDC);
	hai_DeleteDC(hDCmem);
	hai_DeleteObject((SH_GDIOBJ)pSceneDat->hbk);
	hai_DeleteObject((SH_GDIOBJ)pSceneDat->hbox);
	hai_ReleaseSceneData();
	RETURNSCENE(0, 0);
}



