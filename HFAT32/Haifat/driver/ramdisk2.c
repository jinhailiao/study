//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			ramdisk2.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-24  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"
#include "ftdevice.h"
#include "haiclib.h"


#ifdef ADD_RAM_DISK2_DRIVER

#ifdef RAMDISK_SIMULATOR
#include <stdio.h>
#endif

//macro define
//
#define RAMDISK2_SECTOR_BYTES			512
#define RAMDISK2_SECTOR_MAX				0x40000/*128MB*/
#define RAMDISK2_SECT_PER_CLST			2
#define RAMDISK2_BUFFER_CLST_MAX			6

#define RAMDISK2_BUFFER_NEEDFLUSH		0x01000000


//function prototype
//
static int RamDiskReadCluster(S_DWORD cluster, S_BYTE *OutBuf);
static int RamDiskWriteCluster(S_DWORD cluster, S_BYTE *InBuf);
static void IncreaseUseCount(int BufNo);
static int FindLeastUseCluster(void);

int RamDisk2DevInit(void);
int RamDisk2DevStatus(void);
int RamDisk2DevRead(S_DWORD sector, S_BYTE *OutBuf);
int RamDisk2DevWrite(S_DWORD sector, S_BYTE *InBuf);
int RamDisk2DevCmd(S_DWORD cmd, S_DWORD arg, S_DWORD *OutVal);
int RamDisk2DevFlush(S_DWORD sector, S_BOOL FreeBuffer);

//assert driver struct
//
const S_FATDEVDRV RamDisk2Drv =
{
	"ramdisk32",
	RamDisk2DevInit,
	RamDisk2DevStatus,
	RamDisk2DevRead,
	RamDisk2DevWrite,
	RamDisk2DevCmd,
	RamDisk2DevFlush,
};

//RamDisk Driver Buffer
//
static S_BYTE *RamDiskStartAddr;
static S_BYTE RamDiskBuf[RAMDISK2_BUFFER_CLST_MAX][RAMDISK2_SECTOR_BYTES*RAMDISK2_SECT_PER_CLST];
static S_DWORD RamDiskFlag[RAMDISK2_BUFFER_CLST_MAX];
static S_DWORD RamDiskClst[RAMDISK2_BUFFER_CLST_MAX];

int RamDisk2DevInit(void)
{
	int i;
#ifdef RAMDISK_SIMULATOR
	FILE *fp = fopen(".\\ramdisk32.dat", "rb+");
	if (fp == S_NULL)
		return -1;
	RamDiskStartAddr = (S_BYTE*)fp;
#else
	RamDiskStartAddr = 0x????????;//note: This addr must align 4-bytes;
#endif
	for (i = 0; i < RAMDISK2_BUFFER_CLST_MAX; i++)
		RamDiskFlag[i] = 0;
	for (i = 0; i < RAMDISK2_BUFFER_CLST_MAX; i++)
		RamDiskClst[i] = 0xFFFFFFFF;
	return 0;	
}

int RamDisk2DevStatus(void)
{
	return 0;
}

int RamDisk2DevRead(S_DWORD sector, S_BYTE *OutBuf)
{
	int i, m = RAMDISK2_BUFFER_CLST_MAX-1;
	S_DWORD cluster = sector/RAMDISK2_SECT_PER_CLST;
	S_DWORD current = sector%RAMDISK2_SECT_PER_CLST;
	
	//find if in driver buffer
	for (i = 0; i < RAMDISK2_BUFFER_CLST_MAX; i++)
	{
		if (RamDiskClst[i] == 0xFFFFFFFF)
			m = i;
		else if (RamDiskClst[i] == cluster)
			break;
	}
	
	if (i == RAMDISK2_BUFFER_CLST_MAX)// not found out
	{
		if (RamDiskClst[m] != 0xFFFFFFFF) // have not idle cluster, make one idle cluster
		{
			m = FindLeastUseCluster();
			if ((RamDiskFlag[m]&RAMDISK2_BUFFER_NEEDFLUSH) == RAMDISK2_BUFFER_NEEDFLUSH)
				if (RamDiskWriteCluster(RamDiskClst[m], RamDiskBuf[m]))
				{
					return -1;
				}
		}
		RamDiskFlag[m] = 0;
		RamDiskClst[m] = cluster;
		if (RamDiskReadCluster(RamDiskClst[m], RamDiskBuf[m]))//ok, have one idle cluster
		{
			RamDiskClst[m] = 0xFFFFFFFF;//Oh, shit! fail on read disk
			return -1;
		}
		i = m;//make it as found successful....
	}

	if (RamDiskClst[i] == cluster)//OK, found out
	{
		S_BYTE *pDat = &RamDiskBuf[i][current*RAMDISK2_SECTOR_BYTES];
#if 0		
		for (m = 0; m < RAMDISK_SECTOR_BYTES; m++)
			OutBuf[m]  = pDat[m];
#else
		hai_memcpy(OutBuf, pDat, RAMDISK2_SECTOR_BYTES);
#endif
		IncreaseUseCount(i);
	}
	
	return 0;	
}

int RamDisk2DevWrite(S_DWORD sector, S_BYTE *InBuf)
{
	int i, m = RAMDISK2_BUFFER_CLST_MAX-1;
	S_DWORD cluster = sector/RAMDISK2_SECT_PER_CLST;
	S_DWORD current = sector%RAMDISK2_SECT_PER_CLST;
	
	//find if in driver buffer
	for (i = 0; i < RAMDISK2_BUFFER_CLST_MAX; i++)
	{
		if (RamDiskClst[i] == 0xFFFFFFFF)
			m = i;
		else if (RamDiskClst[i] == cluster)
			break;
	}
	
	if (i == RAMDISK2_BUFFER_CLST_MAX)// not found out
	{
		if (RamDiskClst[m] != 0xFFFFFFFF) // have not idle cluster, make one idle cluster
		{
			m = FindLeastUseCluster();
			if ((RamDiskFlag[m]&RAMDISK2_BUFFER_NEEDFLUSH) == RAMDISK2_BUFFER_NEEDFLUSH)
				if (RamDiskWriteCluster(RamDiskClst[m], RamDiskBuf[m]))
				{
					return -1;
				}
		}
		RamDiskFlag[m] = 0;
		RamDiskClst[m] = cluster;
		if (RamDiskReadCluster(RamDiskClst[m], RamDiskBuf[m]))//ok, have one idle cluster
		{
			RamDiskClst[m] = 0xFFFFFFFF;//Oh, shit! fail on read disk
			return -1;
		}
		i = m;//make it as found successful....
	}

	if (RamDiskClst[i] == cluster)//OK, found out
	{
		S_BYTE *pDat = &RamDiskBuf[i][current*RAMDISK2_SECTOR_BYTES];
#if 0		
		for (i = 0; i < RAMDISK_SECTOR_BYTES; i++)
			pDat[i] = InBuf[i];
#else
		hai_memcpy(pDat, InBuf, RAMDISK2_SECTOR_BYTES);
#endif
		RamDiskFlag[i] |= RAMDISK2_BUFFER_NEEDFLUSH;
		IncreaseUseCount(i);
	}
		
	return 0;
}

int RamDisk2DevCmd(S_DWORD cmd, S_DWORD arg, S_DWORD *OutVal)
{
	int err = 0;
	switch (cmd)
	{
	case DEVCMD_WRITE_PROTECT:
		//*OutVal = DEVINFO_FLAG_WRITE_DETECT; //if have write protect 
		*OutVal = 0;
		break;
	case DEVCMD_SECTOR_SIZE:
		*OutVal = RAMDISK2_SECTOR_BYTES;//this is a recommand val, otherwise:1024, 2048, 4096.
		break;
	case DEVCMD_TOTAL_SECTOR:
		*OutVal = RAMDISK2_SECTOR_MAX;
		break;
	case DEVCMD_SEC_PER_CLUS:
		*OutVal = RAMDISK2_SECT_PER_CLST; //or 1, 2, 4, 8, 16, 32, 64, 128;
		break;
	default:
		*OutVal = 0;
		break;
	}
	return err;
}

int RamDisk2DevFlush(S_DWORD sector, S_BOOL FreeBuffer)
{
	int i, start, end;
	
	if (sector == 0xFFFFFFFF)
	{
		start = 0; end = RAMDISK2_BUFFER_CLST_MAX;
	}
	else
	{
		S_DWORD cluster = sector/RAMDISK2_SECT_PER_CLST;
		for (i = 0; i < RAMDISK2_BUFFER_CLST_MAX; i++)
			if (RamDiskClst[i] == cluster)
				break;
		if (i == RAMDISK2_BUFFER_CLST_MAX)
		{
			start = 0; end = 0;
		}
		else
		{
			start = i; end = i+1;
		}
	}
	
	for (i = start; i < end; i++)
	{
		if ((RamDiskFlag[i]&RAMDISK2_BUFFER_NEEDFLUSH) == RAMDISK2_BUFFER_NEEDFLUSH)
		{
			if (RamDiskWriteCluster(RamDiskClst[i], RamDiskBuf[i]))
			{
				return -1;
			}
			RamDiskFlag[i] &= ~RAMDISK2_BUFFER_NEEDFLUSH;
		}
		if (FreeBuffer)
		{
			RamDiskFlag[i] = 0x00;
			RamDiskClst[i] = 0xFFFFFFFF;
		}
	}
	return 0;
}



static void IncreaseUseCount(int BufNo)
{
	S_DWORD UseCnt = RamDiskFlag[BufNo]&0x0000FFFF;
	
	UseCnt = (UseCnt<0xFFFF)? (UseCnt+1):(UseCnt);//increase use count
	RamDiskFlag[BufNo] = (RamDiskFlag[BufNo]&0xFFFF0000)|(UseCnt&0x0000FFFF);
}

static int RamDiskReadCluster(S_DWORD cluster, S_BYTE *OutBuf)
{
	S_DWORD offset = cluster * RAMDISK2_SECTOR_BYTES * RAMDISK2_SECT_PER_CLST;
	
#ifdef RAMDISK_SIMULATOR
	if (!fseek((FILE*)RamDiskStartAddr, offset, SEEK_SET))
	{
		if (fread(OutBuf, RAMDISK2_SECTOR_BYTES, RAMDISK2_SECT_PER_CLST, (FILE*)RamDiskStartAddr) != RAMDISK2_SECT_PER_CLST)
			return -1;
		return 0;
	}
	return -1;
#else
	int i;
	S_BYTE *pDat = RamDiskStartAddr + offset;
	for (i = 0; i < RAMDISK2_SECTOR_BYTES * RAMDISK2_SECT_PER_CLST; i++)
		OutBuf[i] = pDat[i];
	return 0;
#endif
}

static int RamDiskWriteCluster(S_DWORD cluster, S_BYTE *InBuf)
{
	S_DWORD offset = cluster * RAMDISK2_SECTOR_BYTES * RAMDISK2_SECT_PER_CLST;
	
#ifdef RAMDISK_SIMULATOR
	if (!fseek((FILE*)RamDiskStartAddr, offset, SEEK_SET))
	{
		if (fwrite(InBuf, RAMDISK2_SECTOR_BYTES, RAMDISK2_SECT_PER_CLST, (FILE*)RamDiskStartAddr) != RAMDISK2_SECT_PER_CLST)
			return -1;
		fflush((FILE*)RamDiskStartAddr);
		return 0;
	}
	return -1;
#else
	int i;
	S_BYTE *pDat = RamDiskStartAddr + offset;
	for (i = 0; i < RAMDISK2_SECTOR_BYTES * RAMDISK2_SECT_PER_CLST; i++)
		pDat[i] = InBuf[i];
	return 0;
#endif
}

static int FindLeastUseCluster(void)
{
	int i = 0, m = 0;
	S_DWORD UseCnt = RamDiskFlag[i]&0x0000FFFF;
	
	for (i = 1; i < RAMDISK2_BUFFER_CLST_MAX; i++)
	{
		if (UseCnt > (RamDiskFlag[i]&0x0000FFFF))
		{
			UseCnt = RamDiskFlag[i]&0x0000FFFF;
			m = i;
		}
	}
	return m;
}

#endif //#ifdef ADD_RAM_DISK_DRIVER
