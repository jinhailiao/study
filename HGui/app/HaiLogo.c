//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haitime.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#include "haitype.h"
#include "schedule.h"
#include "haievent.h"
#include "stdio.h"
#include "haigdi.h"
#include "haihandle.h"
#include "bmpicon.h"
#include "AppIDs.h"
#include "haiscene.h"
#include "haictrl.h"

DEFINESCENE(SID_HAILOGO)
MSGMAPBEGIN
	ONCREAT(LogoOnCreat)
	ONMSG(EVT_PAINT, LogoOnPaint)
	ONUSERMSG(EVT_TIMER, LogoOnTimer)
	ONDESTROY(LogoOnDestroy)
	ONKEYDN(LogoOnKey)
MSGMAPEND


static void  LogoOnCreat(SP_EVT pEvent)
{
	S_WORD *TimeCnt = hai_GetSceneData(sizeof(S_WORD));

	*TimeCnt = 0;
}

static void  LogoOnPaint(SP_EVT pEvent)
{
	S_PAINTSTRUCT Paint;
	SH_DC hDC, hDCmem;
	SH_BITMAP hbitmap;
	S_BITMAP bitmap;

	hDC = hai_BeginPaint(&Paint);
	hbitmap = hai_LoadBitmap(WINOS_RES_PATH"hailogo.bmp");
	hai_GetObject((SH_GDIOBJ)hbitmap, sizeof(bitmap), &bitmap);
	hDCmem = hai_CreateCompatibleDC(hDC);
	hai_SelectObject(hDCmem, (SH_GDIOBJ)hbitmap);
	hai_BitBlt(hDC, 0, 0, 480, 272, S_NULL, 0, 0, WHITENESS);
	hai_BitBlt(hDC, (S_SHORT)((LCD_WIDTH-bitmap.width)>>1), (S_SHORT)((LCD_HEIGHT-bitmap.height)>>1), bitmap.width, bitmap.height, hDCmem, 0, 0, SRCCOPY);
	hai_DeleteObject((SH_GDIOBJ)hbitmap);
	hai_DeleteDC(hDCmem);
	hai_EndPaint(&Paint);
}

static void  LogoOnTimer(SP_EVT pEvent)
{
	S_WORD *TimeCnt = hai_GetSceneData(sizeof(S_WORD));

	(*TimeCnt)++;
	
	if (*TimeCnt >= 100)
		hai_PostEvent(EVT_DESTROY, 0, 0);
}

static void  LogoOnDestroy(SP_EVT pEvent)
{
	hai_ReleaseSceneData();
	SWITCHAPP(APPID_DESKTOP, SID_DESKTOP, 0, 0);
}

static void  LogoOnKey(SP_EVT pEvent)
{
	hai_PostEvent(EVT_DESTROY, 0, 0);
}


