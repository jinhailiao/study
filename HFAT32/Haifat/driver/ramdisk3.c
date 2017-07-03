//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			ramdisk3.c
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
#include "cfg2410.h"


#ifdef ADD_RAM_DISK3_DRIVER

//macro define
//
#define RAMDISK3_SECTOR_BYTES			512
#define RAMDISK3_SECTOR_MAX				(RAMDISK_SIZE/RAMDISK3_SECTOR_BYTES)/*32MB*/
#define RAMDISK3_SECT_PER_CLST			4

//function prototype
//
int RAMDISK3DevInit(void);
int RAMDISK3DevStatus(void);
int RAMDISK3DevRead(S_DWORD sector, S_BYTE *OutBuf);
int RAMDISK3DevWrite(S_DWORD sector, S_BYTE *InBuf);
int RAMDISK3DevCmd(S_DWORD cmd, S_DWORD arg, S_DWORD *OutVal);
int RAMDISK3DevFlush(S_DWORD sector, S_BOOL FreeBuffer);

//assert driver struct
//
const S_FATDEVDRV RamDisk3Drv =
{
	"RAMDISK3",
	RAMDISK3DevInit,
	RAMDISK3DevStatus,
	RAMDISK3DevRead,
	RAMDISK3DevWrite,
	RAMDISK3DevCmd,
	RAMDISK3DevFlush,
};

//RAMDISK3 Driver Buffer
//
#ifdef RAMDISK_SIMULATOR
static S_BYTE RamDisk3[RAMDISK3_SECTOR_MAX*512];
#endif
static S_BYTE *RAMDISK3StartAddr;

int RAMDISK3DevInit(void)
{
#ifdef RAMDISK_SIMULATOR
	RAMDISK3StartAddr = RamDisk3;
#else
	RAMDISK3StartAddr = (S_BYTE *)RAMDISK_START;//note: This addr must align 4-bytes;
#endif
	return 0;	
}

int RAMDISK3DevStatus(void)
{
	return 0;
}

int RAMDISK3DevRead(S_DWORD sector, S_BYTE *OutBuf)
{
	S_BYTE *pDat = RAMDISK3StartAddr + sector*RAMDISK3_SECTOR_BYTES;

	hai_memcpy(OutBuf, pDat, RAMDISK3_SECTOR_BYTES);
	
	return 0;	
}

int RAMDISK3DevWrite(S_DWORD sector, S_BYTE *InBuf)
{
	S_BYTE *pDat = RAMDISK3StartAddr + sector*RAMDISK3_SECTOR_BYTES;

	hai_memcpy(pDat, InBuf, RAMDISK3_SECTOR_BYTES);

	return 0;
}

int RAMDISK3DevCmd(S_DWORD cmd, S_DWORD arg, S_DWORD *OutVal)
{
	int err = 0;
	switch (cmd)
	{
	case DEVCMD_WRITE_PROTECT:
		*OutVal = 0;
		break;
	case DEVCMD_SECTOR_SIZE:
		*OutVal = RAMDISK3_SECTOR_BYTES;//this is a recommand val, otherwise:1024, 2048, 4096.
		break;
	case DEVCMD_TOTAL_SECTOR:
		*OutVal = RAMDISK3_SECTOR_MAX;
		break;
	case DEVCMD_SEC_PER_CLUS:
		*OutVal = RAMDISK3_SECT_PER_CLST; //or 1, 2, 4, 8, 16, 32, 64, 128;
		break;
	default:
		*OutVal = 0;
		break;
	}
	return err;
}

int RAMDISK3DevFlush(S_DWORD sector, S_BOOL FreeBuffer)
{
	return 0;
}

#endif //#ifdef ADD_RAM_DISK_DRIVER

