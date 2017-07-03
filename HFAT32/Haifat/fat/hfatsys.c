//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatsys.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"
#include "ftdevice.h"
#include "hfatutil.h"
#include "hfatsys.h"
#include "haiclib.h"
#include "hfatdent.h"
#include "hfatstr.h"


int _hai_InitFatSys(void)
{
	int err, i;
	
	_hai_CwdOperate(CWDOPERATE_SETCLUS, (S_BYTE*)0x00);//init chdir info
	hai_InitFatDeviceData();
	
	for (i = 0; i < DEVICE_SUPPORT_MAX; i++)
	{
		hai_DiskOperateLock(i);
		if (err = _hai_InitFatDevice(i))
			printferr("", err);
		if (err = hai_UpdateDevFatInfo(i))
			printferr("", err);
		hai_DiskOperateUnlock(i);
	}

	if (hai_GetDeviceNum('C') == -1)
		err = FATERR_INVALID_DISK;
	else
	{
		_hai_CwdOperate(CWDOPERATE_INITPATH, "C:");//init chdir info
		err = FATERR_NO_ERROR;
	}
	
	return err;
}

int _hai_formatfatdev(int dev, S_BYTE FatType)
{
	int err;
	S_WORD sector, num;
	S_DWORD GetVal;
	S_FILLBPB FillBPB;
	S_BYTE BPB[FAT_SEC_BUF_MAX];

	if (err = _hai_FatDevFlush(dev, 0xFFFFFFFF, S_TRUE))//Clear Driver Buffer
		return FATERR_DEV_OPERATE_ERR;

	FillBPB.Media = 0xF8;//0xF0, ...
	if (err = _hai_FatDevCmd(dev, DEVCMD_SECTOR_SIZE, 0, &GetVal))
		return FATERR_DEV_OPERATE_ERR;
	if (GetVal == 512 || GetVal == 1024 || GetVal == 2048 || GetVal == 4096)
		FillBPB.SecSize = (S_WORD)GetVal;
	else if (GetVal == 0 || GetVal == 0xFFFFFFFF)
		FillBPB.SecSize = 512;
	else
		return FATERR_INVALID_VALUE;
	
	if (err = _hai_FatDevCmd(dev, DEVCMD_SEC_PER_CLUS, 0, &GetVal))
		return FATERR_DEV_OPERATE_ERR;
	if (GetVal==1 || GetVal==2 || GetVal==4 || GetVal==8 || GetVal==16
		|| GetVal==32 || GetVal==64/* || GetVal==128*/)
		FillBPB.SecPerClus = (S_BYTE)GetVal;
	else
		FillBPB.SecPerClus = 0xFF;
	
	if (err = _hai_FatDevCmd(dev, DEVCMD_ROOTENTCNT_F16, 0, &GetVal))
		return FATERR_DEV_OPERATE_ERR;
	if (GetVal == 0xFFFFFFFF)
		FillBPB.RootEntCnt = 0;
	else
		FillBPB.RootEntCnt = (S_WORD)GetVal;

	if (err = _hai_FatDevCmd(dev, DEVCMD_FAT_SIZE, 0, &GetVal))
		return FATERR_DEV_OPERATE_ERR;
	if (GetVal == 0xFFFFFFFF)
		FillBPB.FatSize = 0;
	else
		FillBPB.FatSize = GetVal;

	if (err = _hai_FatDevCmd(dev, DEVCMD_TOTAL_SECTOR, 0, &GetVal))
		return FATERR_DEV_OPERATE_ERR;
	if (GetVal == 0 || GetVal == 0xFFFFFFFF)
		return FATERR_INVALID_VALUE;

	FillBPB.TotSec = GetVal;
	if ((S_DWORD)FillBPB.SecSize*FillBPB.SecPerClus > MAXBYTES_PER_CLUS)
	{
		if (FillBPB.SecSize == 512)
		{
			FillBPB.SecPerClus = hai_GetSectorPerCluster(FillBPB.TotSec, &FatType);
			if (FillBPB.SecPerClus == 0)
				return FATERR_INVALID_VALUE;
		}
		else
		{
			return FATERR_INVALID_VALUE;
		}
	}
	if (FatType == FAT_TYPE_NO)
		FillBPB.FatType = hai_JudgeFatType(&FillBPB);//FIXME: may be error
	else
		FillBPB.FatType = FatType;

	if (FillBPB.FatType != FAT_TYPE_16 && FillBPB.FatType != FAT_TYPE_32)
		return FATERR_UNKNOWN_FILESYS;//only support FAT16 and FAT32;
	
	if (FillBPB.FatType == FAT_TYPE_16 && FillBPB.RootEntCnt == 0)
		FillBPB.RootEntCnt = 512;
	else if (FillBPB.FatType == FAT_TYPE_32)
		FillBPB.RootEntCnt = 0;

	if (FillBPB.FatType == FAT_TYPE_32)
		FillBPB.RsvdSec = 0x20;
	else
		FillBPB.RsvdSec = 0x01;

	if (FillBPB.FatSize == 0)
		FillBPB.FatSize = hai_CountFatSize(&FillBPB);
	
	hai_memset(BPB, 0x00, sizeof(BPB));
	err = hai_FillBPB(BPB, &FillBPB);
	if (err)
		return -1;

	if (err = _hai_FatDevCmd(dev, DEVCMD_ERASE_ALL_SECTOR, 0, &GetVal))
		return FATERR_DEV_OPERATE_ERR;
	
	//write BPB
	if (err = _hai_FatDevWrite(dev, 0x00, BPB))
		return FATERR_DEV_OPERATE_ERR;
	if (FillBPB.FatType == FAT_TYPE_32)
	{//write backup BPB if it is Fat32
		if (err = _hai_FatDevWrite(dev, FAT32_BKBOOT_SEC, BPB))
			return FATERR_DEV_OPERATE_ERR;
	}

	//init FAT
	hai_memset(BPB, 0x00, sizeof(BPB));
	for (sector = 0; sector < 2 * FillBPB.FatSize; sector++)
	{
		if (sector != 0 && sector != FillBPB.FatSize)
		{
			if (err = _hai_FatDevWrite(dev, FillBPB.RsvdSec+sector, BPB))
				return FATERR_DEV_OPERATE_ERR;
		}
	}
	BPB[0] = FillBPB.Media;
	BPB[1] = 0xFF, BPB[2] = 0xFF, BPB[3] = 0xFF;
	if (FillBPB.FatType == FAT_TYPE_32)
	{
		BPB[3] = 0x0F;
		HAI_WRITEDWORD(BPB+4, 0x0FFFFFFFUL);
		HAI_WRITEDWORD(BPB+8, 0x0FFFFFFFUL);
	}
	for (sector = 0; sector < 2; sector++)
	{
		if (err = _hai_FatDevWrite(dev, FillBPB.RsvdSec+sector*FillBPB.FatSize, BPB))
			return FATERR_DEV_OPERATE_ERR;
	}

	// Init root directory area
	hai_memset(BPB, 0x00, sizeof(BPB));
	if (FillBPB.RootEntCnt != 0)
	{
		num = (S_WORD)((FillBPB.RootEntCnt * 0x20UL) / FillBPB.SecSize);
		for (sector = 0; sector < num; sector++)
		{
			if (err = _hai_FatDevWrite(dev, FillBPB.RsvdSec + 2 * FillBPB.FatSize + sector, BPB))
				return FATERR_DEV_OPERATE_ERR;
		}
	}
	else
	{
		num = FillBPB.SecPerClus;
		for (sector = 0; sector < num; sector++)
		{
			if (_hai_FatDevWrite(dev, FillBPB.RsvdSec + 2 * FillBPB.FatSize /*+ (FillBPB.RootClus - 2) * num*/+ sector, BPB))
				return FATERR_DEV_OPERATE_ERR;
		}
	}

	//create volume lab
//	err = pDevInfo->drv->FatDevWrite(FillBPB.RsvdSec + 2 * FillBPB.FatSize, BPB);
//	if (err)
//		return FATERR_DEV_OPERATE_ERR;

	//init FS_INFO
	if (FillBPB.FatType == FAT_TYPE_32)
	{
		GetVal = (FillBPB.TotSec - (FillBPB.RsvdSec + 2 * FillBPB.FatSize) - 1) / FillBPB.SecPerClus;
		hai_memset(BPB, 0x00, sizeof(BPB));
		HAI_WRITEDWORD(BPB+0x0, 0x41615252UL);//FSI_LeadSig:0x41615252
		HAI_WRITEDWORD(BPB+484, 0x61417272UL);//FSI_StrucSig:0x61417272
		HAI_WRITEDWORD(BPB+488, GetVal);//FSI_Free_Count:0x????????
		HAI_WRITEDWORD(BPB+492, 0x00000002UL);//FSI_Nxt_Free:0x00000002
		HAI_WRITEDWORD(BPB+508, 0xAA550000UL);//FSI_TrailSig:0xAA550000
		if (err = _hai_FatDevWrite(dev, 0x01, BPB))
			return FATERR_DEV_OPERATE_ERR;
		if (err = _hai_FatDevWrite(dev, FAT32_BKBOOT_SEC+0x01, BPB))
			return FATERR_DEV_OPERATE_ERR;
	}

	//init 3rd boot block
	if (FillBPB.FatType == FAT_TYPE_32)
	{
		hai_memset(BPB, 0x00, sizeof(BPB));
		BPB[510] = 0x55, BPB[511] = 0xAA;
		if (err = _hai_FatDevWrite(dev, 0x02, BPB))
			return FATERR_DEV_OPERATE_ERR;
		if (err = _hai_FatDevWrite(dev, FAT32_BKBOOT_SEC+0x02, BPB))
			return FATERR_DEV_OPERATE_ERR;
	}
	
	if (err = _hai_FatDevFlush(dev, 0xFFFFFFFF, S_FALSE))
		return FATERR_DEV_OPERATE_ERR;

	err = hai_UpdateDevFatInfo(dev);
	
	//if reach here, OK!!!
	return 0;
}

int _hai_formatfat(S_BYTE disk, S_BYTE FatType)
{
	int err;
	int dev = hai_GetDeviceNum(disk);

	if (dev == -1)
		return FATERR_INVALID_DISK;

	hai_DiskOperateLock(dev);
	err = _hai_formatfatdev(dev, FatType);
	hai_DiskOperateUnlock(dev);

	return err;
}

int hai_ChangeDirOnFAT(S_BYTE disk, const S_BYTE *path)
{
	int dev, err = 0;
	S_DWORD StartClus;
	S_BYTE dentbuf[FAT_DIRENT_SIZE];
	S_BYTE dirname[HAI_FILENAME_MAX+4];

	if ((dev = hai_GetDeviceNum(disk)) == -1)
		return FATERR_INVALID_DISK;
	
	hai_memset(dentbuf, 0x00, sizeof(dentbuf));

	if (*(path+1) == ':')
	{
		_hai_CwdOperate(CWDOPERATE_INITPATH, (S_BYTE *)path);
		_hai_CwdOperate(CWDOPERATE_SETCLUS, (S_BYTE*)0x00);
		StartClus = 0, path += 2;
	}
	else
	{
		StartClus = _hai_CwdOperate(CWDOPERATE_GETCLUS, (S_BYTE*)0x00);
	}

	if (StartClus == 0 && *path == '\0')//change disk
	{
		if (DevInf_FatType(dev) == FAT_TYPE_32)
			_hai_CwdOperate(CWDOPERATE_SETCLUS, (S_BYTE*)DevInf_RootClus(dev));
		else
			_hai_CwdOperate(CWDOPERATE_SETCLUS, (S_BYTE*)0x00);
		return 0;
	}

	if (DevInf_FatType(dev) == FAT_TYPE_32 && StartClus == 0) // FAT32 rootclust
		StartClus = DevInf_RootClus(dev);
	*pDIR_Attr(dentbuf) = FILE_ATTR_DIRECTORY;
	HAI_WRITEWORD(pDIR_FstClusHI(dentbuf), StartClus>>16);
	HAI_WRITEWORD(pDIR_FstClusLO(dentbuf), StartClus);

	hai_memset(dirname, 0x00, sizeof(dirname));
	path = hai_GetOneDirName(path, dirname);

	hai_DiskOperateLock(dev);
	
	while (dirname[0])
	{
		StartClus = GetDentClust(dentbuf);
		if (!IS_FAT_CUR_DIR(dirname))
			err = hai_FindDent(dev, StartClus, dirname, dentbuf);
		if (err || !IS_FILE_ATTR(dentbuf, FILE_ATTR_DIRECTORY))
		{
			if (err == FATERR_NO_ERROR)
				err = FATERR_INVALID_PATH;
			break;
		}
		_hai_CwdOperate(CWDOPERATE_ADDPATH, dirname);
		hai_memset(dirname, 0x00, sizeof(dirname));
		path = hai_GetOneDirName(path, dirname);
	}

	hai_DiskOperateUnlock(dev);

	if (!err)
	{
		if (IS_FILE_ATTR(dentbuf, FILE_ATTR_DIRECTORY))
			_hai_CwdOperate(CWDOPERATE_SETCLUS, (S_BYTE*)GetDentClust(dentbuf));
		else
			err = FATERR_INVALID_PATH;
	}
	
	return err;
}

int _hai_mkdirOnFat(S_BYTE disk, S_BYTE *dirname)
{
	int err;
	int dev = hai_GetDeviceNum(disk);
	S_BYTE *pname;
	S_BYTE dirbuf[FAT_DIRENT_SIZE];
	S_DWORD clust;

	if (dev == -1)
		return FATERR_INVALID_DISK;
	pname = hai_MakeLegalFatFileName(dirname);
	if (!pname)
		return FATERR_INVALID_FILENAME;

	hai_memset(dirbuf, 0x00, sizeof(dirbuf));
	clust = _hai_CwdOperate(CWDOPERATE_GETCLUS, S_NULL);
	hai_DiskOperateLock(dev);
	err = hai_SearchDent(dev, clust, dirname, dirbuf);
	if (err == FATERR_INVALID_FILENAME)
	{
		hai_memset(dirbuf, 0x00, sizeof(dirbuf));
		*pDIR_Attr(dirbuf) = FILE_ATTR_DIRECTORY|FILE_ATTR_READ_ONLY;
		err = hai_CreateDirDent(dev, clust, dirname, dirbuf);
	}
	else
	{
		err = FATERR_EXISTED_FILENAME;
	}
	hai_DiskOperateUnlock(dev);
	
	return err;
}

int _hai_rmdirOnFat(S_BYTE disk, S_BYTE *dirname)
{
	int err = -1;
	int dev = hai_GetDeviceNum(disk);
	S_BYTE DentBuf[FAT_DIRENT_SIZE];
	S_WORD DentNum, LDentNum;
	S_DWORD DSecNum, LDSecNum, clust;

	if (dev == -1)
		return FATERR_INVALID_DISK;
	if (!*dirname||IS_FAT_CUR_DIR(dirname)||IS_FAT_BACK_DIR(dirname))
		return FATERR_INVALID_FILENAME;

	hai_DiskOperateLock(dev);

	clust = _hai_CwdOperate(CWDOPERATE_GETCLUS, S_NULL);
	err = hai_LocationDent(dev, clust, dirname, &DSecNum, &DentNum, &LDSecNum, &LDentNum, DentBuf);
	if (err == FATERR_NO_ERROR)
	{
		err = hai_RemoveDent(dev, DSecNum, DentNum, LDSecNum, LDentNum, DentBuf);
		if (!err)
		{
			clust = GetDentClust(DentBuf);
			err = hai_UnlinkClust(dev, clust);
		}
	}

	hai_DiskOperateUnlock(dev);

	return err;
}

int _hai_fflushOnFat(S_BYTE disk, SH_FATFILEOP hfileOp)
{
	int err = FATERR_INVALID_DISK;
	S_BYTE *pSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	SP_FATFILEINF pfileInf = hfileOp->pfileinf;

	if (hai_GetDeviceNum(disk) != hfileOp->dev)
		goto FFLUSH_ERR;
	if (!hai_FatDevExist(hfileOp->dev))
		goto FFLUSH_ERR;

	//flush filebuf
	if (pfileInf->pSecbuf)
	{
		if (err = _hai_FlushFileBuf(hfileOp->dev, pfileInf->pSecbuf))
			goto FFLUSH_ERR;
	}

	//flush fileinf
	if (err = _hai_FatDevRead(hfileOp->dev, pfileInf->DentSec, SecBuf))
		goto FFLUSH_ERR;
	pSecBuf = SecBuf + pfileInf->DentNum * FAT_DIRENT_SIZE;
	HAI_WRITEDWORD(pDIR_FileSize(pSecBuf), pfileInf->FileSize);
	hai_UpdateDent(pSecBuf, S_FALSE);
	if (err = _hai_FatDevWrite(hfileOp->dev, pfileInf->DentSec, SecBuf))
		goto FFLUSH_ERR;
		
	//flush drvbuf
	err = _hai_FatDevFlush(hfileOp->dev, 0xFFFFFFFF, S_FALSE);

FFLUSH_ERR:
	hfileOp->err = err;
	return err;
}

int hai_flushallOnFat(void)
{
	int i, err = 0;

	for (i = 0; i < DEVICE_SUPPORT_MAX; i++)
	{
		hai_DiskOperateLock(i);
		if (hai_FatDevExist(i))
		{
			if (_hai_FatDevFlush(i, 0xFFFFFFFF, S_FALSE))
				err = -1;
		}
		hai_DiskOperateUnlock(i);
	}
	return err;
}

