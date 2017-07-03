//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatsys.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __H_FILEFAT_H__
#define __H_FILEFAT_H__

#include "hfstype.h"
#include "hfatfile.h"


#define FAT_TYPE_32		3
#define FAT_TYPE_16		2
#define FAT_TYPE_12		1
#define FAT_TYPE_NO		0

#define FAT_DIRENT_SIZE		0x20

//#define FAT32_CLST_MAX		
#define FAT16_CLST_MAX		65525
#define FAT12_CLST_MAX		4085

#define MAXBYTES_PER_CLUS		(32*1024)/*32K*/

//----------- BPB area start --------------------------------
//
#define FAT32_DATA_OFF		28
#define FAT16_DATA_OFF		0
#define FAT12_DATA_OFF		0

//BPB offset(common)
//
#define pBS_jmpBoot(BPB)		((BPB)+0)
#define pBS_OEMName(BPB)		((BPB)+3)
#define pBPB_BytsPerSec(BPB)	((BPB)+11)
#define pBPB_SecPerClus(BPB)	((BPB)+13)
#define pBPB_RsvdSecCnt(BPB)	((BPB)+14)
#define pBPB_NumFATs(BPB)		((BPB)+16)
#define pBPB_RootEntCnt(BPB)	((BPB)+17)
#define pBPB_TotSec16(BPB)		((BPB)+19)
#define pBPB_Media(BPB)			((BPB)+21)
#define pBPB_FATSz16(BPB)		((BPB)+22)
#define pBPB_SecPerTrk(BPB)		((BPB)+24)
#define pBPB_NumHeads(BPB)		((BPB)+26)
#define pBPB_HiddSec(BPB)		((BPB)+28)
#define pBPB_TotSec32(BPB)		((BPB)+32)

#define pBS_DrvNum(BPB,Off)		((BPB)+(Off)+36)
#define pBS_Reserved1(BPB,Off)	((BPB)+(Off)+37)
#define pBS_BootSig(BPB,Off)	((BPB)+(Off)+38)
#define pBS_VolID(BPB,Off)		((BPB)+(Off)+39)
#define pBS_VOlLab(BPB,Off)		((BPB)+(Off)+43)
#define pBS_FilSysType(BPB,Off)	((BPB)+(Off)+54)

//BPB parameter size(common)
//
#define sBS_jmpBoot			3
#define sBS_OEMName			8
#define sBPB_BytsPerSec		2
#define sBPB_SecPerClus		1
#define sBPB_RsvdSecCnt		2
#define sBPB_NumFATs		1
#define sBPB_RootEntCnt		2
#define sBPB_TotSec16		2
#define sBPB_Media			1
#define sBPB_FATSz16		2
#define sBPB_SecPerTrk		2
#define sBPB_NumHeads		2
#define sBPB_HiddSec		2
#define sBPB_TotSec32		4

#define sBS_DrvNum			1
#define sBS_Reserved1		1
#define sBS_BootSig			1
#define sBS_VolID			4
#define sBS_VOlLab			11
#define sBS_FilSysType		8

//BPB offset(FAT32)
//
#define pBPB_FATSz32(BPB)		((BPB)+36)
#define pBPB_ExtFlags(BPB)		((BPB)+40)
#define pBPB_FSVer(BPB)			((BPB)+42)
#define pBPB_RootClus(BPB)		((BPB)+44)
#define pBPB_FSInfo(BPB)		((BPB)+48)
#define pBPB_BkBootSec(BPB)		((BPB)+50)
#define pBPB_Reserved(BPB)		((BPB)+52)

//BPB parameter size(FAT32)
//
#define sBPB_FATSz32		4
#define sBPB_ExtFlags		2
#define sBPB_FSVer			2
#define sBPB_RootClus		4
#define sBPB_FSInfo			2
#define sBPB_BkBootSec		2
#define sBPB_Reserved		12

//----------- BPB area end --------------------------------



int _hai_InitFatSys(void);
int _hai_formatfat(S_BYTE disk, S_BYTE FatType);
int hai_ChangeDirOnFAT(S_BYTE disk, const S_BYTE *path);
int _hai_mkdirOnFat(S_BYTE disk, S_BYTE *dirname);
int _hai_rmdirOnFat(S_BYTE disk, S_BYTE *dirname);
int hai_flushallOnFat(void);
int _hai_fflushOnFat(S_BYTE disk, SH_FATFILEOP hfileOp);

#endif //__H_FILEFAT_H__


