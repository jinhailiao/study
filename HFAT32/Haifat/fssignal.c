//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			ftsignal.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"

SIGNAL_T FileBlockSignal;

int hai_FileSysSignalInit(void)
{
	//TODO:
	FileBlockSignal = 0;
	
	return 0;
}

int hai_FileBlockLock(void)
{
	//TODO:
	FileBlockSignal++;
	//if (dev in use)
	//AppSchedule();
	return 0;
}

int hai_FileBlockUnlock(void)
{
	//TODO:
	FileBlockSignal--;
	return 0;
}

int hai_DiskOperateLock(int dev)
{
	//TODO:
	switch (dev)
	{
	case 0:
		//if (dev in use)
		//AppSchedule();
		break;
	case 1:
		//if (dev in use)
		//AppSchedule();
		break;
	case 2:
		//if (dev in use)
		//AppSchedule();
		break;
	default:
		break;
	}
	return 0;
}

int hai_DiskOperateUnlock(int dev)
{
	//TODO:
	
	switch (dev)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}
	return 0;
}


