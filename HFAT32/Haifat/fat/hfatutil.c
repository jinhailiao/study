//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatutil.c
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
#include "hfatstr.h"


const S_BYTE *FatFlag32 = "FAT32   ";
const S_BYTE *FatFlag16 = "FAT16   ";
const S_BYTE *FatFlag12 = "FAT12   ";


int hai_GetFatType(S_BYTE *BPB)
{
	S_WORD RsvdSec, FatSize, FatNum, RootSiz;
	S_DWORD TotSec, DatSec, DatClus;

#if 0
	if (!(hai_memcmp(pBS_FilSysType(BPB,FAT32_DATA_OFF), FatFlag32, 3/*sBS_FilSysType*/)== 0
		|| hai_memcmp(pBS_FilSysType(BPB,FAT16_DATA_OFF), FatFlag16, 3/*sBS_FilSysType*/)== 0
		|| hai_memcmp(pBS_FilSysType(BPB,FAT12_DATA_OFF), FatFlag12, 3/*sBS_FilSysType*/)== 0))
#else
	if (BPB[510] != 0x55 || BPB[511] != 0xAA)
#endif
		return FAT_TYPE_NO;
	
	RsvdSec = HAI_MAKEWORD(pBPB_RsvdSecCnt(BPB));
	FatSize = HAI_MAKEWORD(pBPB_FATSz16(BPB));
	if (FatSize == 0)
		FatSize = HAI_MAKEWORD(pBPB_FATSz32(BPB));
	FatNum  = (S_WORD)*pBPB_NumFATs(BPB);
	RootSiz = ((HAI_MAKEWORD(pBPB_RootEntCnt(BPB))*FAT_DIRENT_SIZE)+(HAI_MAKEWORD(pBPB_BytsPerSec(BPB))-1))/HAI_MAKEWORD(pBPB_BytsPerSec(BPB));
	TotSec  = (S_DWORD)HAI_MAKEWORD(pBPB_TotSec16(BPB));
	if (TotSec == 0)
		TotSec  = HAI_MAKEDWORD(pBPB_TotSec32(BPB));
	DatSec  = TotSec - (RsvdSec+FatNum*FatSize+RootSiz);
	DatClus = DatSec/(*pBPB_SecPerClus(BPB));
	
	if (DatClus < FAT12_CLST_MAX)
		return FAT_TYPE_12;
	else if (DatClus < FAT16_CLST_MAX)
		return FAT_TYPE_16;
	else
		return FAT_TYPE_32;
}


//following tab was copied from Microsoft window.
struct DSKSZTOSECPERCLUS
{
	S_DWORD DiskSize;
	S_BYTE  SecPerClus;
};

const struct DSKSZTOSECPERCLUS DskTabFat16[] =
{
	{      8400,  0},//SecPerClus为0表示这是一个错误
	{     32680,  2},//disks up to 16MB, 1k clustor
	{    262144,  4},//disks up to 128MB, 2k clustor
	{    524288,  8},//disks up to 256MB, 4k clustor
	{   1048576, 16},//disks up to 512MB, 8k clustor
	//The entries after this point are not used unless FAT16 is forced
	{   2097152, 32},//disks up to 1GB, 16k clustor
	{   4194304, 64},//disks up to 2GB, 32k clustor
	{0xFFFFFFFF,  0}//error
};

const struct DSKSZTOSECPERCLUS DskTabFat32[] =
{
	{     66600,  0},
	{    532480,  1},
	{  16777216,  8},
	{  33554432, 16},
	{  67108864, 32},
	{0xFFFFFFFF, 64}
};

S_BYTE hai_GetSectorPerCluster(S_DWORD TotSec, S_BYTE *pFatType)
{
	int i;
	
	if (*pFatType == FAT_TYPE_NO)
	{
		if (TotSec <= 1048526)
			*pFatType = FAT_TYPE_16;
		else
			*pFatType = FAT_TYPE_32;
	}

	if (*pFatType == FAT_TYPE_16)
	{
		if (TotSec <= 8400 || TotSec > 4194304)
			return 0;
		for (i = 0; i < sizeof(DskTabFat16)/sizeof(DskTabFat16[0]); i++)
			if (TotSec <= DskTabFat16[i].DiskSize)
				break;
		return DskTabFat16[i].SecPerClus;
	}
	else if (*pFatType == FAT_TYPE_32)
	{
		if (TotSec <= 66600 || TotSec > 67108864)
			return 0;
		for (i = 0; i < sizeof(DskTabFat32)/sizeof(DskTabFat32[0]); i++)
			if (TotSec <= DskTabFat32[i].DiskSize)
				break;
		return DskTabFat32[i].SecPerClus;
	}

	return 0;
}

S_DWORD hai_GetFat32FreeCnt(int dev)
{
	S_DWORD TotalClus = (DevInf_TotSec(dev)-(DevInf_RsvdSec(dev)+DevInf_NumFats(dev)*DevInf_FatSize(dev))-1)/DevInf_SecPerClust(dev);

	return TotalClus;
}

int hai_FillBPB(S_BYTE *BPB, SP_FILLBPB pFillBPB)
{
	S_DWORD DateTime = 0x01234567;//hai_GetDateTime32();
	S_DWORD FatDatOff = 0;

	if (pFillBPB->FatType == FAT_TYPE_32)
		FatDatOff = FAT32_DATA_OFF;
	
	*(pBS_jmpBoot(BPB)+0) = 0xEB, *(pBS_jmpBoot(BPB)+1) = 0x00, *(pBS_jmpBoot(BPB)+2) = 0x90;
	hai_memcpy(pBS_OEMName(BPB), "MSWIN4.1", sBS_OEMName);
	HAI_WRITEWORD(pBPB_BytsPerSec(BPB), pFillBPB->SecSize);
	*(pBPB_SecPerClus(BPB)) = pFillBPB->SecPerClus;
	HAI_WRITEWORD(pBPB_RsvdSecCnt(BPB), pFillBPB->RsvdSec);
	*(pBPB_NumFATs(BPB)) = FAT_NUMFATS;
	if (pFillBPB->FatType == FAT_TYPE_32)
		HAI_WRITEWORD(pBPB_RootEntCnt(BPB), 0x00);
	else
		HAI_WRITEWORD(pBPB_RootEntCnt(BPB), pFillBPB->RootEntCnt);
	if (pFillBPB->FatType == FAT_TYPE_16 && pFillBPB->TotSec < 0x10000)
	{
		HAI_WRITEWORD(pBPB_TotSec16(BPB), pFillBPB->TotSec);
		HAI_WRITEDWORD(pBPB_TotSec32(BPB), 0x00);
	}
	else
	{
		HAI_WRITEWORD(pBPB_TotSec16(BPB), 0x00);
		HAI_WRITEDWORD(pBPB_TotSec32(BPB), pFillBPB->TotSec);
	}
	*(pBPB_Media(BPB)) = pFillBPB->Media;
	if (pFillBPB->FatType == FAT_TYPE_32)
		HAI_WRITEWORD(pBPB_FATSz16(BPB), 0x00);
	else
		HAI_WRITEWORD(pBPB_FATSz16(BPB), pFillBPB->FatSize);
	HAI_WRITEWORD(pBPB_SecPerTrk(BPB), 0x00);
	HAI_WRITEWORD(pBPB_NumHeads(BPB), 0x00);
	HAI_WRITEDWORD(pBPB_HiddSec(BPB), 0x00);
	if (pFillBPB->FatType == FAT_TYPE_32)
	{
		HAI_WRITEDWORD(pBPB_FATSz32(BPB), pFillBPB->FatSize);
		//BPB_ExtFlags  Bit0-3(active FAT)  Bit4-6(reserved)  Bit7  Bit8-15(reserved) 
		//           0b0000           0b000        0b0           0b00000000
		HAI_WRITEWORD(pBPB_ExtFlags(BPB), 0x00);
		HAI_WRITEWORD(pBPB_FSVer(BPB), FAT_FILESYS_VER/*0x0103*/);//ver 1.03
		HAI_WRITEDWORD(pBPB_RootClus(BPB), FAT32_ROOT_CLUS);
		HAI_WRITEWORD(pBPB_FSInfo(BPB), FAT32_FSINFO_SEC);
		HAI_WRITEWORD(pBPB_BkBootSec(BPB), FAT32_BKBOOT_SEC);
		hai_memset(pBPB_Reserved(BPB), 0x00, sBPB_Reserved);
	}
	*(pBS_DrvNum(BPB, FatDatOff)) = 0;
	*(pBS_Reserved1(BPB, FatDatOff)) = 0;
	*(pBS_BootSig(BPB, FatDatOff)) = 0x29;//0x00: next three fields are not present
	HAI_WRITEDWORD(pBS_VolID(BPB, FatDatOff), DateTime);//serial number, e.g. date/time
	hai_memcpy(pBS_VOlLab(BPB, FatDatOff), "NO NAME    ", sBS_VOlLab);
	if (pFillBPB->FatType == FAT_TYPE_32)
		hai_memcpy(pBS_FilSysType(BPB, FatDatOff), FatFlag32, sBS_FilSysType);
	else
		hai_memcpy(pBS_FilSysType(BPB, FatDatOff), FatFlag16, sBS_FilSysType);

	BPB[510] = 0x55, BPB[511] = 0xAA;
	
	return 0;
}

S_BYTE hai_JudgeFatType(SP_FILLBPB pFillBPB)
{
	S_DWORD DatClus;
//The methed may be error.
	DatClus = (pFillBPB->TotSec - (pFillBPB->FatSize*2) - 
		(((pFillBPB->RootEntCnt*32)+(pFillBPB->SecSize-1))/pFillBPB->SecSize))/pFillBPB->SecPerClus;

	if (DatClus < FAT12_CLST_MAX)
		return FAT_TYPE_12;
	else if (DatClus < FAT16_CLST_MAX)
		return FAT_TYPE_16;
	else
		return FAT_TYPE_32;
}

S_DWORD hai_CountFatSize(SP_FILLBPB pFillBPB)
{
	S_DWORD RootDirSec, TmpVal1, TmpVal2;

	RootDirSec = ((pFillBPB->RootEntCnt*32)+(pFillBPB->SecSize-1))/pFillBPB->SecSize;
	TmpVal1 = pFillBPB->TotSec - (pFillBPB->RsvdSec + RootDirSec);
	TmpVal2 = ((S_DWORD)256 * pFillBPB->SecPerClus) + 2/*BPB_NumFATs*/;
	if (pFillBPB->FatType == FAT_TYPE_32)
		TmpVal2 = TmpVal2 / 2;

	pFillBPB->FatSize = (TmpVal1 + (TmpVal2-1)) / TmpVal2;

	if (pFillBPB->FatType != FAT_TYPE_32)
		pFillBPB->FatSize &= 0xFFFF;

	return pFillBPB->FatSize;
}

S_DWORD hai_GetNextCluster(int dev, S_DWORD CurClust)
{
	S_DWORD FatOff;
	S_DWORD FatSecNum, FatEntOff;
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];
	
	if (DevInf_FatType(dev) == FAT_TYPE_16)
		FatOff = CurClust << 1;
	else if (DevInf_FatType(dev) == FAT_TYPE_32)
		FatOff = CurClust << 2;
	else
		return 0xFFFFFFFF;

	FatSecNum = DevInf_RsvdSec(dev) + (FatOff/DevInf_BytsPerSec(dev));
	FatEntOff = FatOff%DevInf_BytsPerSec(dev);

	if (_hai_FatDevRead(dev, FatSecNum, SecBuf))
		return 0xFFFFFFFF;
	if (DevInf_FatType(dev) == FAT_TYPE_16)
	{
		CurClust = (S_DWORD)HAI_MAKEWORD(SecBuf+FatEntOff);
		if (CurClust == 0xFFFF)
			CurClust = 0xFFFFFFFF;
	}
	else
	{
		CurClust = HAI_MAKEDWORD(SecBuf+FatEntOff) & 0x0FFFFFFF;//28 bit are valid.
		if (CurClust == 0xFFFFFFF)
			CurClust = 0xFFFFFFFF;
	}

	return CurClust;
}

int hai_FsInfoOperate(int dev, S_DWORD *NxtFree, S_DWORD *FreeCnt)
{
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];

	if (_hai_FatDevRead(dev, DevInf_FSInfo(dev), SecBuf))
		return FATERR_DEV_OPERATE_ERR;
	if (HAI_MAKEDWORD(pFSI_LeadSig(SecBuf))==FSI_LEADSIG
			&& HAI_MAKEDWORD(pFSI_StrucSig(SecBuf))==FSI_STRUCSIG
			&& HAI_MAKEDWORD(pFSI_TrailSig(SecBuf))==FSI_TRAILSIG)
	{
		if (*NxtFree==FSI_NXTFREE_INVALID&&*FreeCnt==FSI_FREECNT_INVALID)
		{
			*NxtFree = HAI_MAKEDWORD(pFSI_NextFree(SecBuf));
			*FreeCnt = HAI_MAKEDWORD(pFSI_FreeCount(SecBuf));
		}
		else
		{
			HAI_WRITEDWORD(pFSI_NextFree(SecBuf), *NxtFree);
			HAI_WRITEDWORD(pFSI_FreeCount(SecBuf), *FreeCnt);
			if (_hai_FatDevWrite(dev, DevInf_FSInfo(dev), SecBuf))
				return FATERR_DEV_OPERATE_ERR;
//			if (_hai_FatDevWrite(dev, DevInf_BkBootSec(dev)+DevInf_FSInfo(dev), SecBuf))
//				return FATERR_DEV_OPERATE_ERR;
		}
	}

	return 0;
}

S_DWORD hai_AllocIdleClust(int dev)
{
	S_WORD Offset;
	S_DWORD Sec, TmpClust, IdleClus = 0;
	S_DWORD NxtFree, FreeCnt; 
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];

	if (DevInf_FatType(dev) == FAT_TYPE_32)
	{
		NxtFree=FSI_NXTFREE_INVALID, FreeCnt=FSI_FREECNT_INVALID;
		hai_FsInfoOperate(dev, &NxtFree, &FreeCnt);
		if (NxtFree==FSI_NXTFREE_INVALID) TmpClust = 0;
		else TmpClust = NxtFree & FAT32_CLUST_VALIDBIT_MASK;
		Sec = (TmpClust<<2)/DevInf_BytsPerSec(dev);
		Offset = DevInf_BytsPerSec(dev);
		for (; Sec < DevInf_FatSize(dev); Sec++)
		{
			if (_hai_FatDevRead(dev, DevInf_RsvdSec(dev)+Sec, SecBuf))
				break;
			for (Offset = 0; Offset < DevInf_BytsPerSec(dev); Offset += 4)
			{
				if ((HAI_MAKEDWORD(SecBuf+Offset)&FAT32_CLUST_VALIDBIT_MASK) == 0x00)
					break;
			}
			if (Offset < DevInf_BytsPerSec(dev))// find out
				break;
		}
		if (Offset < DevInf_BytsPerSec(dev))
		{
			TmpClust = Sec*(DevInf_BytsPerSec(dev)>>2) + (Offset>>2);
			if (TmpClust < ((DevInf_TotSec(dev)-DevInf_FirstDatSec(dev))/DevInf_SecPerClust(dev)+2))//is a valid cluster
			{
				HAI_WRITEDWORD(SecBuf+Offset, HAI_MAKEDWORD(SecBuf+Offset)|FAT32_EOF_CLUST);
				if (!_hai_FatDevWrite(dev, DevInf_RsvdSec(dev)+Sec, SecBuf))
				{
					if (DevInf_NumFats(dev) == 2)//write 2th FAT
						_hai_FatDevWrite(dev, DevInf_RsvdSec(dev)+DevInf_FatSize(dev)+Sec, SecBuf);	
					IdleClus = TmpClust;
				}
			}
		}
		if (IdleClus)
		{
			NxtFree = IdleClus;
			if (FreeCnt == FSI_FREECNT_INVALID)
				FreeCnt = hai_GetFat32FreeCnt(dev) - 1;
			else if (FreeCnt > 1)
				FreeCnt -= 1;
			hai_FsInfoOperate(dev, &NxtFree, &FreeCnt);
		}
	}
	else
	{
		Offset = DevInf_BytsPerSec(dev);
		for (Sec = 0; Sec < DevInf_FatSize(dev); Sec++)
		{
			if (_hai_FatDevRead(dev, DevInf_RsvdSec(dev)+Sec, SecBuf))
				break;
			for (Offset = 0; Offset < DevInf_BytsPerSec(dev); Offset += 2)
			{
				if (HAI_MAKEWORD(SecBuf+Offset) == 0x00)
					break;
			}
			if (Offset < DevInf_BytsPerSec(dev))// find out
				break;
		}
		if (Offset < DevInf_BytsPerSec(dev))
		{
			TmpClust = Sec*(DevInf_BytsPerSec(dev)>>1) + (Offset>>1);
			if (TmpClust < ((DevInf_TotSec(dev)-DevInf_FirstDatSec(dev))/DevInf_SecPerClust(dev)+2))//is a valid cluster
			{
				HAI_WRITEWORD(SecBuf+Offset, FAT16_EOF_CLUST);
				if (!_hai_FatDevWrite(dev, DevInf_RsvdSec(dev)+Sec, SecBuf))
				{
					if (DevInf_NumFats(dev) == 2)//write 2th FAT
						_hai_FatDevWrite(dev, DevInf_RsvdSec(dev)+DevInf_FatSize(dev)+Sec, SecBuf);	
					IdleClus = TmpClust;
				}
			}
		}
	}

	if (IdleClus)
	{//clear the space of IdleClus
		hai_memset(SecBuf, 0x00, sizeof(SecBuf));
		Sec = DevInf_FirstDatSec(dev) + (IdleClus-2)*DevInf_SecPerClust(dev);
		for (Offset = 0; Offset < DevInf_SecPerClust(dev); Offset++)
		{
			if (_hai_FatDevWrite(dev, Sec+Offset, SecBuf))
				return 0;
		}
	}

	return IdleClus;
}

int hai_LinkClust(int dev, S_DWORD CurClust, S_DWORD LinkClust)
{
	S_DWORD mask;
	S_DWORD Sec, Offset;
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];

	if (DevInf_FatType(dev) == FAT_TYPE_32)
	{
		Sec = (CurClust<<2)/DevInf_BytsPerSec(dev);
		if (_hai_FatDevRead(dev, DevInf_RsvdSec(dev)+Sec, SecBuf))
			return FATERR_DEV_OPERATE_ERR;
		Offset = (CurClust<<2)%DevInf_BytsPerSec(dev);
		mask = HAI_MAKEDWORD(SecBuf+Offset)&~FAT32_CLUST_VALIDBIT_MASK;
		HAI_WRITEDWORD(SecBuf+Offset, mask|LinkClust);
	}
	else
	{
		Sec = (CurClust<<1)/DevInf_BytsPerSec(dev);
		if (_hai_FatDevRead(dev, DevInf_RsvdSec(dev)+Sec, SecBuf))
			return FATERR_DEV_OPERATE_ERR;
		Offset = (CurClust<<1)%DevInf_BytsPerSec(dev);
		HAI_WRITEWORD(SecBuf+Offset, LinkClust);
	}
	
	if (_hai_FatDevWrite(dev, DevInf_RsvdSec(dev)+Sec, SecBuf))
		return FATERR_DEV_OPERATE_ERR;
	if (DevInf_NumFats(dev) == 2)//write 2th FAT
		_hai_FatDevWrite(dev, DevInf_RsvdSec(dev)+DevInf_FatSize(dev)+Sec, SecBuf);	

	return 0;
}

int hai_UnlinkClust(int dev, S_DWORD clust)
{
	S_DWORD mask;
	S_DWORD Sec, Offset;
	S_DWORD curFreeCnt = 0, curFreeClust = 0xFFFFFFFF;
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];

	while (DevInf_FatType(dev)==FAT_TYPE_16?IS_FAT16_INUSE_CLUST(clust):IS_FAT32_INUSE_CLUST(clust))
	{
		if (DevInf_FatType(dev) == FAT_TYPE_32)
		{
			curFreeCnt++;
			if (clust < curFreeClust)
				curFreeClust = clust;
			Sec = (clust<<2)/DevInf_BytsPerSec(dev);
			if (_hai_FatDevRead(dev, DevInf_RsvdSec(dev)+Sec, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			Offset = (clust<<2)%DevInf_BytsPerSec(dev);
			clust = HAI_MAKEDWORD(SecBuf+Offset);
			mask = clust&~FAT32_CLUST_VALIDBIT_MASK;
			clust &= FAT32_CLUST_VALIDBIT_MASK;
			HAI_WRITEDWORD(SecBuf+Offset, mask|FAT32_VALID_CLUST);
		}
		else
		{
			Sec = (clust<<1)/DevInf_BytsPerSec(dev);
			if (_hai_FatDevRead(dev, DevInf_RsvdSec(dev)+Sec, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			Offset = (clust<<1)%DevInf_BytsPerSec(dev);
			clust = HAI_MAKEWORD(SecBuf+Offset);
			HAI_WRITEWORD(SecBuf+Offset, FAT16_VALID_CLUST);
		}
		if (_hai_FatDevWrite(dev, DevInf_RsvdSec(dev)+Sec, SecBuf))
			return FATERR_DEV_OPERATE_ERR;
		if (DevInf_NumFats(dev) == 2)//write 2th FAT
			_hai_FatDevWrite(dev, DevInf_RsvdSec(dev)+DevInf_FatSize(dev)+Sec, SecBuf);	
	}
	if (DevInf_FatType(dev) == FAT_TYPE_32)
	{
		S_DWORD FreeCnt = FSI_FREECNT_INVALID, FreeClust = FSI_NXTFREE_INVALID;
		if (hai_FsInfoOperate(dev, &FreeClust, &FreeCnt) == 0)
		{
			if (FreeCnt == FSI_FREECNT_INVALID)
				FreeCnt = hai_GetFat32FreeCnt(dev);
			else if (FSI_FREECNT_INVALID-FreeCnt > curFreeCnt)
				FreeCnt += curFreeCnt;
				
			if (FreeClust == FSI_NXTFREE_INVALID || FreeClust > curFreeClust)
				FreeClust = curFreeClust;
			hai_FsInfoOperate(dev, &FreeClust, &FreeCnt);
		}
	}
	
	return 0;
}

S_BYTE hai_IsFATDisk(S_BYTE disk)
{
	int dev;

	if ((dev = hai_GetDeviceNum(disk)) == -1)
		return S_FALSE;
	return (DevInf_FatType(dev)==FAT_TYPE_32 || DevInf_FatType(dev)==FAT_TYPE_16);
}

/*
32-bit Windows Time/Date Formats
Remarks

The file time and the date are stored individually, using unsigned integers as bit fields. File time and date are packed as follows:

Time

Bit Position: 0   1   2   3   4 5   6   7   8   9   A B   C   D   E   F  
Length:       5                 6                     5 
Contents:     hours             minutes               2-second increments 
Value Range:  0–23              0-59                  0–29 in 2-second intervals 


Date

Bit Position: 0   1   2   3   4   5   6 7   8   9   A B   C   D   E   F 
Length:       7                         4             5 
Contents:     year                      month         day 
Value Range:  0–119                    1–12         1–31 
             (relative to 1980)     
*/
S_DWORD hai_GetDateTime32(S_VOID)
{
	S_DWORD DateTime = 0x13371337;

	return DateTime;
}

S_WORD hai_GetDate16(S_VOID)
{
	return 0x00;
}

S_WORD hai_GetTime16(S_VOID)
{
	return 0x00;
}

S_BYTE hai_GetTimeTenth8(S_VOID)
{
	return 0x00;
}

