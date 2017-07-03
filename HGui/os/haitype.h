//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haitype.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef _HAITYPEDEF_H_
#define _HAITYPEDEF_H_

#include "stdio.h"
#include "ctype.h"
#include "string.h"


//
//configuration informoration
//

#define DEBUG_MEMHEAP

#define  LCD_BITSPERPIX		24
#define USE_16GRAY_PALETTE
//#define HAI_16BITS_COLOR_REVERSION
#define USE_256GRAY_PALETTE

#define __HAVE_STDIO__

#define _ENTER_CRITICAL_		S_NULL
#define _EXIT_CRITICAL_			S_NULL

#define WINOS_RES_PATH		"..\\data\\extended\\bmp\\"


//
//type define
//

typedef unsigned char   S_BYTE;
typedef unsigned short  S_WORD;
typedef unsigned long   S_DWORD;
typedef char            S_CHAR;
typedef short           S_SHORT;
typedef long            S_LONG;
typedef void            S_VOID;

typedef unsigned int	S_UINT;
typedef int			S_INT;

#define S_CONST		const
#define WPARAM		S_WORD
#define LPARAM		S_DWORD


#define S_BOOL            S_BYTE
#define S_FALSE           ((S_BOOL)0)
#define S_TRUE            ((S_BOOL)1)
#define S_NULL            ((void *)0)
#define S_ROMDATA	static const
#define S_COLOR		S_DWORD
#define S_REGISTER	register


// Make sure the types really have the right sizes
#define HGui_SIZE_ASSERT(name, x)               \
       typedef int HGui_dummy_ ## name[(x) * 2 - 1]

HGui_SIZE_ASSERT(S_BYTE, sizeof(S_BYTE)==1);
HGui_SIZE_ASSERT(S_WORD, sizeof(S_WORD)==2);
HGui_SIZE_ASSERT(S_DWORD, sizeof(S_DWORD)==4);
HGui_SIZE_ASSERT(S_CHAR, sizeof(S_CHAR)==1);
HGui_SIZE_ASSERT(S_SHORT, sizeof(S_SHORT)==2);
HGui_SIZE_ASSERT(S_LONG, sizeof(S_LONG)==4);
HGui_SIZE_ASSERT(WPARAM, sizeof(WPARAM)==2);
HGui_SIZE_ASSERT(LPARAM, sizeof(LPARAM)==4);


//
//general macro define
//
#define HAI_MAKEWORD(p)		((((S_WORD)(*(p)))&0xFF)|((((S_WORD)(*((p)+1)))<<8)&0xFF00))
#define HAI_MAKEDWORD(p)	((((S_DWORD)(*(p)))&0xFF)|((((S_DWORD)(*((p)+1)))<<8)&0xFF00)|((((S_DWORD)(*((p)+2)))<<16)&0xFF0000)|((((S_DWORD)(*((p)+3)))<<24)&0xFF000000))
#define HAI_MAKEDWORD3(p)	((((S_DWORD)(*(p)))&0xFF)|((((S_DWORD)(*((p)+1)))<<8)&0xFF00)|((((S_DWORD)(*((p)+2)))<<16)&0xFF0000))
#define HAI_WRITEWORD(p,w)	*(p)=(w)&0xFF,*((p)+1)=((w)>>8)&0xFF
#define HAI_WRITEDWORD(p,dw)  *(p)=(dw)&0xFF,*((p)+1)=((dw)>>8)&0xFF,*((p)+2)=((dw)>>16)&0xFF,*((p)+3)=((dw)>>24)&0xFF 
#define HAI_SWAPWORD(w)		((((w)>>8)&0xFF)|(((w)<<8)&0xFF00))
#define HAI_SWAPDWORD(dw)	((((dw)>>24)&0xFF)|(((dw)>>8)&0xFF00)|(((dw)<<8)&0xFF0000)|(((dw)<<24)&0xFF000000))
#define HAI_COMBWORD(h,l)	((((S_WORD)(h))<<8)|((S_WORD)(l)))
#define HAI_COMBDWORD(h,l)	((((S_DWORD)(h))<<16)|((S_DWORD)(l)))
#define HAI_GETHIBYTE(w)	((S_BYTE)(((w)>>8)&0xFF))
#define HAI_GETLOBYTE(w)	((S_BYTE)((w)&0xFF))
#define HAI_GETHIWORD(dw)	((S_WORD)(((dw)>>16)&0xFFFF))
#define HAI_GETLOWORD(dw)	((S_WORD)((dw)&0xFFFF))
#define HAI_ABS(n)			((n) < 0 ? (-(n)) : (n))
#define HAI_MIN(a, b)		((a) < (b) ? (a) : (b))
#define HAI_MAX(a, b)		((a) > (b) ? (a) : (b))


#define HAI_DECLARE_HANDLE(h)     typedef struct tag##h##hdl{S_WORD unused;}* h
#define HAI_MEMBER_OFFSET(s,m)    ((S_DWORD)(&(((s*)0)->##m)))
#define BITMAP_WIDTHBYTES(w, bits) (((w*bits+31)>>5)<<2)


#define BUILTIN_RESOURCE_FLAG			0x80000000U
#define MAKEINTRESOURCE(ID)			((S_VOID *)((S_DWORD)(ID)|BUILTIN_RESOURCE_FLAG))

//
//base struct define
//
typedef struct tagPoint
{
	S_SHORT  x;
	S_SHORT  y;
}
S_POINT, *SP_POINT;

typedef struct tagRect
{
	S_WORD  x;
	S_WORD  y;
	S_WORD  w;
	S_WORD  h;
}
S_RECT, *SP_RECT;

typedef struct tagRgn
{
	S_SHORT  left;
	S_SHORT  top;
	S_SHORT  right;
	S_SHORT  bottom;
}
S_RGN, *SP_RGN;

typedef struct tagLine
{
	S_SHORT  x0;
	S_SHORT  y0;
	S_SHORT  x1;
	S_SHORT  y1;
}
S_LINE, *SP_LINE;

typedef struct tagArea
{
	S_SHORT  x;
	S_SHORT  y;
	S_SHORT  cx;
	S_SHORT  cy;
}
S_AREA, *SP_AREA;


#endif//_HAITYPEDEF_H_

