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



void  hai_InitMemHeap(void *pMem, unsigned long size);
void  *hai_MemAlloc(unsigned long size);
unsigned char hai_MemFree(void *pMem);
unsigned long hai_MemStatus(unsigned long *pBlkNum, unsigned long *pMaxBlkSize);


#endif //_HAIMEM_H_

