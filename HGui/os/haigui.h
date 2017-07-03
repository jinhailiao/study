//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haigui.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef  _HAIGUI_H_
#define  _HAIGUI_H_

#include "haitype.h"
#include "haihandle.h"

#define BLOCK_3D_DN	0
#define BLOCK_3D_UP		1


#define  MB_MULTILINE	0x000F
#define  MB_OK			0x0001
#define  MB_CANCEL		0x0002
#define  MB_RETRY		0x0004

#if (LCD_BITSPERPIX == 4)

#define  COLOR_WHITE		COLOR_GRAY15
#define  COLOR_LIGHT		COLOR_GRAY12
#define  COLOR_LIGRAY	COLOR_GRAY10
#define  COLOR_GRAY		COLOR_GRAY08
#define  COLOR_LIDARK	COLOR_GRAY06
#define  COLOR_DARK		COLOR_GRAY03
#define  COLOR_BLACK		COLOR_GRAY00

#elif (LCD_BITSPERPIX == 8)

#define  COLOR_WHITE	0xFF
#define  COLOR_LIGHT	0xD2
#define  COLOR_LIGRAY	0xA8
#define  COLOR_GRAY		0x7E
#define  COLOR_LIDARK	0x54
#define  COLOR_DARK		0x2A
#define  COLOR_BLACK	0x00

#elif (LCD_BITSPERPIX == 16)

#define  COLOR_WHITE	0xFFFF
#define  COLOR_LIGHT	0xCE59
#define  COLOR_LIGRAY	0xA514
#define  COLOR_GRAY	0x7BCF
#define  COLOR_LIDARK	0x528A
#define  COLOR_DARK	0x2945
#define  COLOR_BLACK	0x0000

#elif (LCD_BITSPERPIX == 24)

#define  COLOR_WHITE	0xFFFFFF
#define  COLOR_LIGHT	0xD2D2D2
#define  COLOR_LIGRAY	0xA8A8A8
#define  COLOR_GRAY		0x7E7E7E
#define  COLOR_LIDARK	0x545454
#define  COLOR_DARK		0x2A2A2A
#define  COLOR_BLACK	0x000000

#else
#error Unsupport color depth!
#endif



S_BOOL hai_DrawWndFrame(SH_DC hDC, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BYTE *pszTitle);
S_BOOL hai_Draw3DBlock(SH_DC hDC, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BYTE status, S_BYTE SysBrushType);
S_BOOL hai_DrawButton(SH_DC hDC, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_BOOL IsUp, S_BYTE *pszText);
S_VOID hai_GetStringExtent(SH_DC hDC, S_BYTE *str, S_WORD *width, S_WORD *height);
S_VOID hai_DrawBoxByDashed(SH_DC hDC, S_SHORT x, S_SHORT y, S_SHORT w, S_SHORT h, S_COLOR color);
S_BOOL hai_IsRectIntersect(SP_RECT pR1, SP_RECT pR2);
S_BOOL hai_RectAdd(SP_RECT pInR1, SP_RECT pInR2, SP_RECT pOutR);
S_BOOL hai_IsPointInRect(S_WORD x, S_WORD y, SP_RECT pRect);
S_BOOL hai_IsRectCovered(SP_RECT prt1, SP_RECT prt2);


#endif

