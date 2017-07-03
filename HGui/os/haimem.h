//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haimem.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef  _HAIMEM_H_
#define  _HAIMEM_H_

#include  "haitype.h"


S_VOID  hai_InitMemHeap(S_VOID *pMem, S_DWORD size);
S_VOID  *hai_MemAlloc(S_DWORD size);
S_BOOL  hai_MemFree(S_VOID *pMem);
S_DWORD hai_MemStatus(S_DWORD *pBlkNum, S_DWORD *pMaxBlkSize);


#endif //_HAIMEM_H_

