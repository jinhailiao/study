//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatfind.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-26  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __HFATFIND_H__
#define __HFATFIND_H__

#include "hfstype.h"
#include "hfileapi.h"

#define FINDINFO_FILEFILTER_MAX		32

typedef struct _tagFindInfo
{
	int dev;
	S_WORD DentNum;
	S_DWORD SenNum;
	S_BYTE FileFilter[FINDINFO_FILEFILTER_MAX];
}
_S_FINEINFO, *_SP_FINEINFO;

long _hai_findfirstOnFat(S_BYTE disk, S_BYTE *filespec, struct finddata *fileinfo);
int  _hai_findnextOnFat(long handle, struct finddata *fileinfo);


#endif //__HFATFIND_H__
