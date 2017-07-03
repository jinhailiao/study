//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			fattype.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __FAT_TYPE_H__
#define __FAT_TYPE_H__

#include "fsconfig.h"
#include "hfserr.h"
#include "fssignal.h"


typedef char  S_CHAR;
typedef short S_SHORT;
typedef long  S_LONG;
typedef void  S_VOID;
typedef int   S_INT;
typedef unsigned char  S_BYTE;
typedef unsigned short S_WORD;
typedef unsigned long  S_DWORD;
typedef unsigned int   S_UINT;
typedef unsigned int   size_t;


#define S_BOOL	S_BYTE
#define S_TRUE	((S_BOOL)1)
#define S_FALSE	((S_BOOL)0)
#define S_NULL	((S_VOID*)0)


#define HAI_PATH_MAX		(260*2)/*based-byte*/
#define HAI_FILENAME_MAX	(255*2)/*based-byte*/


//
//general macro define
//
#define HAI_MAKEWORD(p)		((((S_WORD)(*(p)))&0xFF)|((((S_WORD)(*((p)+1)))<<8)&0xFF00))
#define HAI_MAKEDWORD(p)	((((S_DWORD)(*(p)))&0xFF)|((((S_DWORD)(*((p)+1)))<<8)&0xFF00)|((((S_DWORD)(*((p)+2)))<<16)&0xFF0000)|((((S_DWORD)(*((p)+3)))<<24)&0xFF000000))
#define HAI_MAKEDWORD3(p)	((((S_DWORD)(*(p)))&0xFF)|((((S_DWORD)(*((p)+1)))<<8)&0xFF00)|((((S_DWORD)(*((p)+2)))<<16)&0xFF0000))
#define HAI_WRITEWORD(p,w)	*(p)=(S_BYTE)((w)&0xFF),*((p)+1)=(S_BYTE)(((w)>>8)&0xFF)
#define HAI_WRITEDWORD(p,dw)  *(p)=(S_BYTE)((dw)&0xFF),*((p)+1)=(S_BYTE)(((dw)>>8)&0xFF),*((p)+2)=(S_BYTE)(((dw)>>16)&0xFF),*((p)+3)=(S_BYTE)(((dw)>>24)&0xFF) 
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



#endif //__FAT_TYPE_H__
