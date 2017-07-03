//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			ftdevice.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __FT_DEVICE_H__
#define __FT_DEVICE_H__

#include "hfstype.h"



//fat device cmd define
//
#define DEVCMD_WRITE_PROTECT		1
#define DEVCMD_SECTOR_SIZE			2
#define DEVCMD_TOTAL_SECTOR		3
#define DEVCMD_SEC_PER_CLUS		4
#define DEVCMD_ROOTENTCNT_F16	5
#define DEVCMD_FAT_SIZE				6
#define DEVCMD_ERASE_ALL_SECTOR		7


//device flag
//
#define DEVINFO_FLAG_WRITE_PROTECT		0x0001U
//#define DEVINFO_FLAG_FORMATED			0x0002U



//fat device driver
//
//note: the sector size must is 512 byte.
typedef int (*pfnFatDevInit)(void);//complete device reset
typedef int (*pfnFatDevStatus)(void);// return device ready or busy, or other......
typedef int (*pfnFatDevRead)(S_DWORD sector, S_BYTE *OutBuf);
typedef int (*pfnFatDevWrite)(S_DWORD sector, S_BYTE *InBuf);
typedef int (*pfnFatDevCmd)(S_DWORD cmd, S_DWORD arg, S_DWORD *OutVal);
//if sector = 0xFFFFFFFF, that means all driver buffer need flush;otherwise, that only flush sector specified driver buffer
//FreeBuffer determine whether specified driver buffer need flag invalid;
//if you don't use driver buffer,then return 0 directly.
typedef int (*pfnFatDevFlush)(S_DWORD sector, S_BOOL FreeBuffer);

typedef struct tagFatDeviceDrv
{
	S_BYTE *DevName;
	pfnFatDevInit   FatDevInit;
	pfnFatDevStatus FatDevStatus;
	pfnFatDevRead   FatDevRead;
	pfnFatDevWrite  FatDevWrite;
	pfnFatDevCmd    FatDevCmd;
	pfnFatDevFlush  FatDevFlush;
}
S_FATDEVDRV, *SP_FATDEVDRV;



//fat devict info
//
typedef struct tagFatDevInfo
{
	S_BYTE  DiskName; // C, D, E, .....
	S_BYTE  FatType;
	S_WORD  flag;
	S_BYTE  SecPerClust;
	S_BYTE  NumFats;
	S_WORD  RootEntCnt;
	S_WORD  RsvdSec;
	S_WORD  BytsPerSec;
	S_WORD  FSInfo;
	S_WORD  BkBootSec;
	S_DWORD FatSize;
	S_DWORD RootClus;
	S_DWORD TotSec;
	S_DWORD FirstDatSec;
	SP_FATDEVDRV drv;
}
S_FATDEVINFO, *SP_FATDEVINFO;


#ifdef HAVE_INLINE

#ifndef _INLINE_
#error	_INLINE_ not defined!!! 
#endif

_INLINE_ S_BYTE  DevInf_DiskName(int dev);
_INLINE_ S_BYTE  DevInf_FatType(int dev);
_INLINE_ S_WORD  DevInf_flag(int dev);
_INLINE_ S_BYTE  DevInf_SecPerClust(int dev);
_INLINE_ S_BYTE  DevInf_NumFats(int dev);
_INLINE_ S_WORD  DevInf_RootEntCnt(int dev);
_INLINE_ S_WORD  DevInf_RsvdSec(int dev);
_INLINE_ S_WORD  DevInf_BytsPerSec(int dev);
_INLINE_ S_WORD  DevInf_FSInfo(int dev);
_INLINE_ S_WORD  DevInf_BkBootSec(int dev);
_INLINE_ S_DWORD DevInf_FatSize(int dev);
_INLINE_ S_DWORD DevInf_RootClus(int dev);
_INLINE_ S_DWORD DevInf_TotSec(int dev);
_INLINE_ S_DWORD DevInf_FirstDatSec(int dev);


#else

extern SP_FATDEVINFO AllDevInfo[];

#define DevInf_DiskName(dev)		AllDevInfo[dev]->DiskName
#define DevInf_FatType(dev)			AllDevInfo[dev]->FatType
#define DevInf_flag(dev)			AllDevInfo[dev]->flag
#define DevInf_SecPerClust(dev)		AllDevInfo[dev]->SecPerClust
#define DevInf_NumFats(dev)			AllDevInfo[dev]->NumFats
#define DevInf_RootEntCnt(dev)		AllDevInfo[dev]->RootEntCnt
#define DevInf_RsvdSec(dev)			AllDevInfo[dev]->RsvdSec
#define DevInf_BytsPerSec(dev)		AllDevInfo[dev]->BytsPerSec
#define DevInf_FSInfo(dev)			AllDevInfo[dev]->FSInfo
#define DevInf_BkBootSec(dev)		AllDevInfo[dev]->BkBootSec
#define DevInf_FatSize(dev)			AllDevInfo[dev]->FatSize
#define DevInf_RootClus(dev)		AllDevInfo[dev]->RootClus
#define DevInf_TotSec(dev)			AllDevInfo[dev]->TotSec
#define DevInf_FirstDatSec(dev)		AllDevInfo[dev]->FirstDatSec


#endif //HAVE_INLINE



void hai_InitFatDeviceData(void);
int hai_FatDevExist(int dev);
int _hai_InitFatDevice(int DevDrvNo);
int hai_GetDeviceNum(S_BYTE disk);
int hai_UpdateDevFatInfo(int DevNo);

int _hai_FatDevReset(int dev);//complete device reset on boot
int _hai_FatDevInit(int dev);//complete device reset on run
int _hai_FatDevStatus(int dev);// return device ready or busy, or other......
int _hai_FatDevRead(int dev, S_DWORD sector, S_BYTE *OutBuf);
int _hai_FatDevWrite(int dev, S_DWORD sector, S_BYTE *InBuf);
int _hai_FatDevCmd(int dev, S_DWORD cmd, S_DWORD arg, S_DWORD *OutVal);
int _hai_FatDevFlush(int dev, S_DWORD sector, S_BOOL FreeBuffer);


#endif //__FT_DEVICE_H__

