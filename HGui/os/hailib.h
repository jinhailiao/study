//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			hailib.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2006-09-06  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#ifndef _HAILIB_H_
#define _HAILIB_H_

#include "haitype.h"




S_BOOL hai_RleDecode(S_BYTE *poutbuf, S_BYTE *pinbuf, S_WORD WidthBytes);
S_BYTE *hai_strchr(const S_BYTE *pszString, S_BYTE c);
S_WORD hai_strlen(const S_BYTE *pszString);
S_BYTE *hai_strcpy(S_BYTE *pszDst, const S_BYTE *pszSrc);
S_VOID hai_CutString(S_BYTE *pszText, S_SHORT AscWidth, S_SHORT DisWidth);
S_VOID *HG_memcpy(S_VOID *dest, const S_VOID *src, S_UINT count);
S_INT HG_memcmp(const S_VOID *buf1, const S_VOID *buf2, S_UINT count);
S_VOID *HG_memset(S_VOID *dest, S_INT c, S_UINT count);





#endif //_HAILIB_H_