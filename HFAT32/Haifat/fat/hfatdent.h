//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatdent.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-18  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __HFATDENT_H__
#define __HFATDENT_H__

#include "hfstype.h"
#include "ftdevice.h"

//dir entry member offset
//
#define pDIR_Name(d)				((d)+0)
#define pDIR_Attr(d)				((d)+11)
#define pDIR_NTRes(d)			((d)+12)
#define pDIR_CrtTimeTenth(d)		((d)+13)
#define pDIR_CrtTime(d)			((d)+14)
#define pDIR_CrtDate(d)			((d)+16)
#define pDIR_LastAccDate(d)		((d)+18)
#define pDIR_FstClusHI(d)			((d)+20)
#define pDIR_WrtTime(d)			((d)+22)
#define pDIR_WrtDate(d)			((d)+24)
#define pDIR_FstClusLO(d)			((d)+26)
#define pDIR_FileSize(d)			((d)+28)

//dir entry member size
//
#define sDIR_Name				11
#define sDIR_Attr					1
#define sDIR_NTRes				1
#define sDIR_CrtTimeTenth			1
#define sDIR_CrtTime				2
#define sDIR_CrtDate				2
#define sDIR_LastAccDate			2
#define sDIR_FstClusHI			2
#define sDIR_WrtTime				2
#define sDIR_WrtDate				2
#define sDIR_FstClusLO			2
#define sDIR_FileSize				4

//LDir name member offset
//
#define pLDIR_Ord(ld)				((ld)+0)
#define pLDIR_Name1(ld)			((ld)+1)
#define pLDIR_Attr(ld)				((ld)+11)
#define pLDIR_Type(ld)			((ld)+12)
#define pLDIR_Chksum(ld)			((ld)+13)
#define pLDIR_Name2(ld)			((ld)+14)
#define pLDIR_FstClusLO(ld)		((ld)+26)
#define pLDIR_Name3(ld)			((ld)+28)

//LDir name member size
//
#define sLDIR_Ord				1
#define sLDIR_Name1				10
#define sLDIR_Attr				1
#define sLDIR_Type				1
#define sLDIR_Chksum				1
#define sLDIR_Name2				12
#define sLDIR_FstClusLO			2
#define sLDIR_Name3				4


//file attribute
//
#define FILE_ATTR_READ_ONLY		0x01
#define FILE_ATTR_HIDDEN			0x02
#define FILE_ATTR_SYSTEM			0x04
#define FILE_ATTR_VOLUME_ID			0x08
#define FILE_ATTR_DIRECTORY		0x10
#define FILE_ATTR_ARCHIVE			0x20
#define FILE_ATTR_LONG_NAME		(FILE_ATTR_READ_ONLY|FILE_ATTR_HIDDEN|FILE_ATTR_SYSTEM	|FILE_ATTR_VOLUME_ID)
#define FILE_ATTR_LONG_NAME_MASK	(FILE_ATTR_READ_ONLY|FILE_ATTR_HIDDEN|FILE_ATTR_SYSTEM	|FILE_ATTR_VOLUME_ID|FILE_ATTR_DIRECTORY|FILE_ATTR_ARCHIVE)


//dir name specification
//
#define DELETED_DIR_FLAG	0xE5
#define EMPTY_DIR_FLAG		0x00
#define DIR_NAME_INSTEAD	0x05/*real val is 0xE5*/
#define DIR_NAME_MAGIC		0xE5

#define LDIR_LAST_LONG_ENTRY		0x40
#define LDIR_STR_BYTE_MAX			26
#define LDIR_STR_CHAR_MAX			(LDIR_STR_BYTE_MAX>>1)

#define IS_FAT_CUR_DIR(p)	(*(p)=='.' && *((p)+1)=='\0')
#define IS_FAT_BACK_DIR(p)	(*(p)=='.' && *((p)+1)=='.' && *((p)+2)=='\0')


#define LONGNAME_LEGAL_CHAR(c)	((c)=='+'||(c)==','||(c)==';'||(c)=='='||(c)=='['||(c)==']')
#if 0
#define SHORTNAME_ILLEGAL_CHAR(c)	((c)==0x22||((c)>=0x2A&&(c)<=0x2C)||(c)==0x2E||(c)==0x2F  \
		||((c)>=0x3A&&(c)<=0x3F)||((c)>=0x5B&&(c)<=0x5D)||(c)==0x7C)
#define LONGNAME_ILLEGAL_CHAR(c)	((c)==0x22||(c)==0x2A||(c)==0x2E||(c)==0x2F||(c)==0x3A  \
		||(c)==0x3C||(c)==0x3E||(c)==0x3F||(c)==0x5C||(c)==0x7C)
#else
#define LONGNAME_ILLEGAL_CHAR(c)	((c)<0x20||(c)=='/'||(c)=='\\'||(c)==':'||(c)=='*'||(c)=='?'||(c)=='\"'||(c)=='<'||(c)=='>'||(c)=='|')
#define SHORTNAME_ILLEGAL_CHAR(c)	(LONGNAME_ILLEGAL_CHAR(c)||LONGNAME_LEGAL_CHAR(c))
#endif

#define SHORTNAME_NORMAL_LENG		12

//
//
#define IS_FILE_ATTR(p, a)		(*pDIR_Attr(p)&a)
#define GetDentClust(p)		HAI_COMBDWORD(HAI_MAKEWORD(pDIR_FstClusHI(p)), HAI_MAKEWORD(pDIR_FstClusLO(p)))


//dir entry struct
//
typedef struct tagDENT
{
	S_BYTE Name[HAI_FILENAME_MAX+4];
	S_BYTE Attr;
	S_BYTE NTRes;
	S_BYTE CrtTimeTenth;
	S_WORD CrtTime;
	S_WORD CrtDate;
	S_WORD LastAccDate;
	S_WORD WrtTime;
	S_WORD WrtDate;
	S_DWORD FstClus;
	S_DWORD FileSize;
}
S_DENT, *SP_DENT;

typedef struct tagLDENT
{
	S_BYTE Ord;
	S_BYTE Name1[10];
	S_BYTE Attr;
	S_BYTE Type;
	S_BYTE Chksum;
	S_BYTE Name2[12];
	S_WORD FstClusLO;
	S_BYTE Name3[4];
}
S_LDENT, *SP_LDENT;


void hai_FillDent(S_BYTE *dent, S_BYTE *Name83, S_BYTE attr, S_DWORD filesize, S_DWORD clust);
void hai_UpdateDent(S_BYTE *dent, S_BYTE IsWrite);
#if 0
int hai_FindDentOnFAT16Root(int dev, S_BYTE *dentName, S_BYTE *dentbuf);
int hai_GetDent(int dev, S_DWORD Clust, S_BYTE *pname, S_BYTE *dentbuf);
#endif
int hai_SearchDent(int Dev, S_DWORD StartClust, const S_BYTE *path, S_BYTE *dentbuf);
int hai_CreateDirDent(int dev, S_DWORD StartClust, S_BYTE *DirName, S_BYTE *dentbuf);
int hai_WriteDent(int dev, S_DWORD SecNum, S_WORD DentNum, const S_BYTE *LongName, S_BYTE *ShortDent);
int hai_InitDirectory(int dev, S_DWORD CurClus, S_DWORD PreClus);
int _hai_TruncateFile(int dev, S_DWORD SecNum, S_WORD DentNum);
int hai_FindDent(int dev, S_DWORD clust, const S_BYTE *DentName, S_BYTE *DentBuf);
int hai_LocationDent(int dev, S_DWORD clust, const S_BYTE *DentName, S_DWORD *DSecNum, S_WORD *DentNum, S_DWORD *LDSecNum, S_WORD *LDentNum, S_BYTE *DentBuf);
int hai_RemoveDent(int dev, S_DWORD DSecNum, S_WORD DentNum, S_DWORD LDSecNum, S_WORD LDentNum, S_BYTE *DentBuf);
S_BYTE hai_ShortFileNameChkSum(S_BYTE *pName);
S_DWORD hai_AllocDent(int dev, S_DWORD StartClust, int DentCnt, S_WORD *DentNum);
S_DWORD  hai_CreateFileDent(int dev, S_DWORD clust, const S_BYTE *file, S_WORD *DentNum, S_BYTE *DentBuf);


#endif //__HFATDENT_H__



