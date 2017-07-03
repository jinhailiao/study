//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatutil.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __HFATUTIL_H__
#define __HFATUTIL_H__

#include "hfstype.h"
#include "ftdevice.h"

//global variable declare
//
extern const S_BYTE *FatFlag32;
extern const S_BYTE *FatFlag16;
extern const S_BYTE *FatFlag12;


//
//
#define FAT_SEC_BUF_MAX			(512+16)
#define FAT_NUMFATS			0x02
#define FAT32_ROOT_CLUS	0x02
#define FAT32_FSINFO_SEC	0x01
#define FAT32_BKBOOT_SEC	0x06
#define FAT_FILESYS_VER		0x00

//FAT flag macro define
//
#define FAT32_VALID_CLUST		0x00000000UL
#define FAT32_BAD_CLUST		0x0FFFFFF7UL
#define FAT32_INUSE_CLUST_S		0x00000002UL
#define FAT32_INUSE_CLUST_E		0x0FFFFFEFUL
#define FAT32_RSVD_CLUST_S		0x0FFFFFF0UL
#define FAT32_RSVD_CLUST_E		0x0FFFFFF6UL
#define FAT32_EOF_CLUST_S		0x0FFFFFF8UL
#define FAT32_EOF_CLUST_E		0x0FFFFFFFUL
#define FAT32_EOF_CLUST		0x0FFFFFFFUL
#define IS_FAT32_INUSE_CLUST(c)	((c)>=FAT32_INUSE_CLUST_S && (c)<=FAT32_INUSE_CLUST_E)
#define IS_FAT32_EOF_CLUST(c)		((c)>=FAT32_EOF_CLUST_S && (c)<=FAT32_EOF_CLUST_E)
#define IS_FAT32_RSVD_CLUST(c)		((c)>=FAT32_RSVD_CLUST_S && (c)<=FAT32_RSVD_CLUST_E)
#define FAT32_CLUST_VALIDBIT_MASK		0x0FFFFFFFUL

#define FAT16_VALID_CLUST		0x0000UL
#define FAT16_BAD_CLUST		0xFFF7UL
#define FAT16_INUSE_CLUST_S		0x0002UL
#define FAT16_INUSE_CLUST_E		0xFFEFUL
#define FAT16_RSVD_CLUST_S		0xFFF0UL
#define FAT16_RSVD_CLUST_E		0xFFF6UL
#define FAT16_EOF_CLUST_S		0xFFF8UL
#define FAT16_EOF_CLUST_E		0xFFFFUL
#define FAT16_EOF_CLUST		0xFFFFUL
#define IS_FAT16_INUSE_CLUST(c)	((c)>=FAT16_INUSE_CLUST_S && (c)<=FAT16_INUSE_CLUST_E)
#define IS_FAT16_EOF_CLUST(c)		((c)>=FAT16_EOF_CLUST_S && (c)<=FAT16_EOF_CLUST_E)
#define IS_FAT16_RSVD_CLUST(c)		((c)>=FAT16_RSVD_CLUST_S && (c)<=FAT16_RSVD_CLUST_E)

//FS Info for FAT32
//
#define pFSI_LeadSig(p)		((p)+0)
#define pFSI_Reserved1(p)	((p)+4)
#define pFSI_StrucSig(p)	((p)+484)
#define pFSI_FreeCount(p)	((p)+488)
#define pFSI_NextFree(p)	((p)+492)
#define pFSI_Reserved2(p)	((p)+496)
#define pFSI_TrailSig(p)	((p)+508)

#define sFSI_LeadSig		4
#define sFSI_Reserved1		480
#define sFSI_StrucSig		4
#define sFSI_FreeCount		4
#define sFSI_NextFree		4
#define sFSI_Reserved2		12
#define sFSI_TrailSig		4

#define FSI_LEADSIG			0x41615252
#define FSI_STRUCSIG		0x61417272
#define FSI_TRAILSIG		0xAA550000
#define FSI_FREECNT_INVALID		0xFFFFFFFF
#define FSI_NXTFREE_INVALID		0xFFFFFFFF


//data struct define
//
typedef struct tagFillBPB
{
	S_BYTE  FatType;
	S_BYTE  SecPerClus;
	S_BYTE  Media;
	S_WORD  SecSize;
	S_WORD  RootEntCnt;
	S_WORD  RsvdSec;
	S_DWORD TotSec;
	S_DWORD FatSize;
}
S_FILLBPB, *SP_FILLBPB;


//function prototype
//
int hai_GetFatType(S_BYTE *BPB);
int hai_FillBPB(S_BYTE *BPB, SP_FILLBPB pFillBPB);
int hai_LinkClust(int dev, S_DWORD CurClust, S_DWORD LinkClust);
int hai_UnlinkClust(int dev, S_DWORD clust);
int hai_FsInfoOperate(int dev, S_DWORD *NxtFree, S_DWORD *FreeCnt);
S_BYTE hai_GetSectorPerCluster(S_DWORD TotSec, S_BYTE *pFatType);
S_BYTE hai_JudgeFatType(SP_FILLBPB pFillBPB);
S_DWORD hai_CountFatSize(SP_FILLBPB pFillBPB);
S_DWORD hai_GetNextCluster(int dev, S_DWORD CurClust);
S_DWORD hai_GetDateTime32(S_VOID);
S_WORD hai_GetDate16(S_VOID);
S_WORD hai_GetTime16(S_VOID);
S_BYTE hai_GetTimeTenth8(S_VOID);
S_DWORD hai_AllocIdleClust(int dev);
S_DWORD hai_GetFat32FreeCnt(int dev);
S_BYTE hai_IsFATDisk(S_BYTE disk);


#endif //__HFATUTIL_H__
