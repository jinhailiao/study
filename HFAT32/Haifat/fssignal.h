//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			ftsignal.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __FTSIGNAL_H__
#define __FTSIGNAL_H__


#define SIGNAL_T	int


int hai_FileSysSignalInit(void);
int hai_FileBlockLock(void);
int hai_FileBlockUnlock(void);
int hai_DiskOperateLock(int dev);
int hai_DiskOperateUnlock(int dev);


#endif //__FTSIGNAL_H__
