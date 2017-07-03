//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			ftdevice.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"
#include "ftdevice.h"
#include "haiclib.h"
#include "hfatutil.h"
#include "hfatsys.h"


//function prototype
//
S_BYTE hai_GetIdleDiskChar(S_VOID);



#ifdef ADD_RAM_DISK_DRIVER
extern const S_FATDEVDRV RamDiskDrv;
#endif
#ifdef ADD_RAM_DISK2_DRIVER
extern const S_FATDEVDRV RamDisk2Drv;
#endif
#ifdef ADD_RAM_DISK3_DRIVER
extern const S_FATDEVDRV RamDisk3Drv;
#endif
#ifdef ADD_S3C2410_NAND
extern const S_FATDEVDRV s3c2410NandDrv;
#endif
#ifdef ADD_S3C2410_SDI
extern const S_FATDEVDRV s3c2410sdiDrv;
#endif


//please add device driver to here
//
const SP_FATDEVDRV AllDevDrv[DEVICE_SUPPORT_MAX] =
{
#ifdef ADD_RAM_DISK3_DRIVER
	(const SP_FATDEVDRV)&RamDisk3Drv,
#else
	S_NULL,
#endif
#ifdef ADD_RAM_DISK_DRIVER
	(const SP_FATDEVDRV)&RamDiskDrv,
#else
	S_NULL,
#endif
#ifdef ADD_RAM_DISK_DRIVER
	(const SP_FATDEVDRV)&RamDisk2Drv,
#else
	S_NULL,
#endif
#ifdef ADD_S3C2410_NAND
	(const SP_FATDEVDRV)&s3c2410NandDrv,
#else
	S_NULL,
#endif
#ifdef ADD_S3C2410_SDI
	(const SP_FATDEVDRV)&s3c2410sdiDrv,
#else
	S_NULL,
#endif
	S_NULL,
	S_NULL,
	S_NULL,
	S_NULL,
	S_NULL
};


//Please do not fill following data
//
S_FATDEVINFO DevInfo[DEVICE_SUPPORT_MAX];
SP_FATDEVINFO AllDevInfo[DEVICE_SUPPORT_MAX] =
{
	S_NULL,
	S_NULL,
	S_NULL,
	S_NULL,
	S_NULL,
	S_NULL,
	S_NULL,
	S_NULL,
	S_NULL,
	S_NULL	
};



//function define
//
#ifdef HAVE_INLINE

#ifndef _INLINE_
#error	_INLINE_ not defined!!! 
#endif

_INLINE_ S_BYTE  DevInf_DiskName(int dev)
{
	return AllDevInfo[dev]->DiskName;
}
_INLINE_ S_BYTE  DevInf_FatType(int dev)
{
	return AllDevInfo[dev]->FatType;	
}
_INLINE_ S_WORD  DevInf_flag(int dev)
{
	return AllDevInfo[dev]->flag;
}
_INLINE_ S_BYTE  DevInf_SecPerClust(int dev)
{
	return AllDevInfo[dev]->SecPerClust;
}
_INLINE_ S_BYTE  DevInf_NumFats(int dev)
{
	return AllDevInfo[dev]->NumFats;
}
_INLINE_ S_WORD  DevInf_RootEntCnt(int dev)
{
	return AllDevInfo[dev]->RootEntCnt;
}
_INLINE_ S_WORD  DevInf_RsvdSec(int dev)
{
	return AllDevInfo[dev]->RsvdSec;
}
_INLINE_ S_WORD  DevInf_BytsPerSec(int dev)
{
	return AllDevInfo[dev]->BytsPerSec;
}
_INLINE_ S_WORD  DevInf_FSInfo(int dev)
{
	return AllDevInfo[dev]->FSInfo;
}
_INLINE_ S_WORD  DevInf_BkBootSec(int dev)
{
	return AllDevInfo[dev]->BkBootSec;
}
_INLINE_ S_DWORD DevInf_FatSize(int dev)
{
	return AllDevInfo[dev]->FatSize;
}
_INLINE_ S_DWORD DevInf_RootClus(int dev)
{
	return AllDevInfo[dev]->RootClus;
}
_INLINE_ S_DWORD DevInf_TotSec(int dev)
{
	return AllDevInfo[dev]->TotSec;
}
_INLINE_ S_DWORD DevInf_FirstDatSec(int dev)
{
	return AllDevInfo[dev]->FirstDatSec;
}

#endif //HAVE_INLINE

int _hai_FatDevReset(int dev)
{
	if (AllDevDrv[dev])
		return AllDevDrv[dev]->FatDevInit();
	return -1;
}

int _hai_FatDevInit(int dev)//complete device reset
{
	return AllDevInfo[dev]->drv->FatDevInit();
}
int _hai_FatDevStatus(int dev)// return device ready or busy, or other......
{
	int i = 0, err;
	
	do{
		err = AllDevInfo[dev]->drv->FatDevStatus();
		if (err)
			AppSchedule();
	}while (err && (i++ < DEVICE_QUERY_MAX));

	return err;
}
int _hai_FatDevRead(int dev, S_DWORD sector, S_BYTE *OutBuf)
{
	if (_hai_FatDevStatus(dev))
		return -1;
	return AllDevInfo[dev]->drv->FatDevRead(sector, OutBuf);
}
int _hai_FatDevWrite(int dev, S_DWORD sector, S_BYTE *InBuf)
{
	if (AllDevInfo[dev]->flag & DEVINFO_FLAG_WRITE_PROTECT)
		return -1;
	if (_hai_FatDevStatus(dev))
		return -1;
	return AllDevInfo[dev]->drv->FatDevWrite(sector, InBuf);
}
int _hai_FatDevCmd(int dev, S_DWORD cmd, S_DWORD arg, S_DWORD *OutVal)
{
	if (_hai_FatDevStatus(dev))
		return -1;
	return AllDevInfo[dev]->drv->FatDevCmd(cmd, arg, OutVal);
}
int _hai_FatDevFlush(int dev, S_DWORD sector, S_BOOL FreeBuffer)
{
	if (_hai_FatDevStatus(dev))
		return -1;
	return AllDevInfo[dev]->drv->FatDevFlush(sector, FreeBuffer);
}

void hai_InitFatDeviceData(void)
{
	hai_memset(AllDevInfo, 0x00, sizeof(AllDevInfo));
	hai_memset(DevInfo, 0x00, sizeof(DevInfo));
}

int hai_FatDevExist(int dev)
{
	if (dev < 0 || dev >= sizeof(AllDevInfo)/sizeof(AllDevInfo[0]))                                    
		return S_FALSE;

	return (AllDevInfo[dev] != S_NULL);
}

int hai_GetDeviceNum(S_BYTE disk)
{
	int DevNo;

	disk &= ~0x20;//upper
	
	for (DevNo = 0; DevNo < sizeof(AllDevInfo)/sizeof(AllDevInfo[0]); DevNo++)
		if (AllDevInfo[DevNo]!=S_NULL && disk == AllDevInfo[DevNo]->DiskName)
			break;
	if (DevNo == sizeof(AllDevInfo)/sizeof(AllDevInfo[0]))
		DevNo = -1;
	return DevNo;
}

int _hai_InitFatDevice(int DevDrvNo)
{
	int err = -1;
	S_DWORD WritePrect;
	SP_FATDEVINFO pDevInfo;
	
	if (DevDrvNo < 0 || DevDrvNo >= sizeof(AllDevDrv)/sizeof(AllDevDrv[0]))                                    
		return FATERR_INVALID_DISK;
		
	if (_hai_FatDevReset(DevDrvNo))
		return FATERR_DEVINIT_FAILED;
	
	pDevInfo = AllDevInfo[DevDrvNo] = &DevInfo[DevDrvNo];
	hai_memset(pDevInfo, 0x00, sizeof(*pDevInfo));
	pDevInfo->drv = AllDevDrv[DevDrvNo];
	pDevInfo->DiskName = hai_GetIdleDiskChar();//must successful
	err = _hai_FatDevCmd(DevDrvNo, DEVCMD_WRITE_PROTECT, 0, &WritePrect);
	if (!err && WritePrect)
		pDevInfo->flag |= DEVINFO_FLAG_WRITE_PROTECT;
	
	return err;
}

int hai_UpdateDevFatInfo(int DevNo)
{
	S_DWORD RootSiz;
	S_BYTE bpb[FAT_SEC_BUF_MAX];
	SP_FATDEVINFO pDevInfo;

	if (!hai_FatDevExist(DevNo))
		return FATERR_INVALID_DISK;

	pDevInfo = AllDevInfo[DevNo];
	if (_hai_FatDevRead(DevNo, 0, bpb))
		return FATERR_DEV_OPERATE_ERR;

	pDevInfo->FatType = (S_BYTE)hai_GetFatType(bpb);
	if (pDevInfo->FatType == FAT_TYPE_NO)
		return FATERR_UNKNOWN_FILESYS;

	pDevInfo->SecPerClust = *pBPB_SecPerClus(bpb);
	pDevInfo->BytsPerSec  = HAI_MAKEWORD(pBPB_BytsPerSec(bpb));
	if (pDevInfo->BytsPerSec != 512)
		return FATERR_UNKNOWN_FILESYS;
	
	pDevInfo->RsvdSec = HAI_MAKEWORD(pBPB_RsvdSecCnt(bpb));
	pDevInfo->NumFats = *pBPB_NumFATs(bpb);
	if (pDevInfo->NumFats > 2)
		return FATERR_UNKNOWN_FILESYS;

	pDevInfo->FatSize = (S_DWORD)HAI_MAKEWORD(pBPB_FATSz16(bpb));
	if (pDevInfo->FatSize == 0)
		pDevInfo->FatSize = HAI_MAKEDWORD(pBPB_FATSz32(bpb));
	pDevInfo->TotSec = (S_DWORD)HAI_MAKEWORD(pBPB_TotSec16(bpb));
	if (pDevInfo->TotSec == 0)
		pDevInfo->TotSec = HAI_MAKEDWORD(pBPB_TotSec32(bpb));
	if (pDevInfo->FatType == FAT_TYPE_32)
	{
		pDevInfo->RootEntCnt = 0;
		pDevInfo->FSInfo   = HAI_MAKEWORD(pBPB_FSInfo(bpb));
		pDevInfo->BkBootSec = HAI_MAKEWORD(pBPB_BkBootSec(bpb));
		pDevInfo->RootClus  = HAI_MAKEDWORD(pBPB_RootClus(bpb));
	}
	else
	{
		pDevInfo->RootEntCnt = HAI_MAKEWORD(pBPB_RootEntCnt(bpb));
		pDevInfo->FSInfo   = 0;
		pDevInfo->BkBootSec = 0;
		pDevInfo->RootClus  = 0;
	}
	RootSiz = ((pDevInfo->RootEntCnt*FAT_DIRENT_SIZE)+(pDevInfo->BytsPerSec-1))/pDevInfo->BytsPerSec;
	pDevInfo->FirstDatSec  = pDevInfo->RsvdSec+pDevInfo->NumFats*pDevInfo->FatSize+RootSiz;
	
	return 0;
}


S_BYTE hai_GetIdleDiskChar(S_VOID)
{
	S_BYTE ch;
	int i;

	for (ch = 'C'; ch <= 'Z'; ch++)
	{
		for (i = 0; i < sizeof(AllDevInfo)/sizeof(AllDevInfo[0]); i++)
			if (AllDevInfo[i]!=S_NULL && ch == AllDevInfo[i]->DiskName)
				break;
		if (i == sizeof(AllDevInfo)/sizeof(AllDevInfo[0]))
			return ch;
	}
	return 0;
}




