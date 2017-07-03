//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatpath.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-19  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __HFATPATH_H__
#define __HFATPATH_H__

#include "hfstype.h"

#define CWDOPERATE_CLRPATH		0
#define CWDOPERATE_INITPATH		1
#define CWDOPERATE_ADDPATH		2
#define CWDOPERATE_COPYFROM		3
#define CWDOPERATE_COPYTO		4
#define CWDOPERATE_SETCLUS		5
#define CWDOPERATE_GETCLUS		6
#define CWDOPERATE_GETLENG		7


S_DWORD _hai_CwdOperate(int operate, S_BYTE *pname);


//return a address of first char that not space
const S_BYTE *hai_DelFrontSpace(const S_BYTE *pszString);
const S_BYTE *hai_DelFrontChar(const S_BYTE *pszString, int c);
//return a address of '\0':
S_BYTE *hai_DelTailSpace(S_BYTE *pszString);
S_BYTE *hai_DelTailChar(S_BYTE *pszString, int c);

S_BYTE *hai_FirstStriStr(const S_BYTE *pdststr, const S_BYTE *psrcstr);
S_BYTE *hai_FindTailChar(const S_BYTE *pszString, int c);
S_BYTE *hai_GetOneDirName(const S_BYTE *dirpath, S_BYTE *OutName);
S_BYTE *hai_MakeLegalFatFileName(S_BYTE *filename);
S_BYTE *hai_MakeNumericTailName(S_BYTE *Name83);
S_BYTE *hai_WriteShortName2Dent(S_BYTE *dentbuf, S_BYTE *Name83);
int hai_Make83Format(const S_BYTE *LongName, S_BYTE *pName83);
int hai_GetFileNameFromDent(const S_BYTE *dent, S_BYTE *DstName);
int hai_GetFileNameFromLDent(const S_BYTE *dent, S_BYTE *DstName);
int hai_CountChar(const S_BYTE *pStr);
int hai_Unicode2OtherCode(S_BYTE *pBuffer);
int hai_OtherCode2Unicode(S_BYTE *UniStr, const S_BYTE *OtherStr);

S_WORD hai_CountByteUni(const S_BYTE *UniStr);

#endif //__HFATPATH_H__
