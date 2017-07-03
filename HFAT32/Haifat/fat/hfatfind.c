//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatfind.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-26  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"
#include "hfileapi.h"
#include "hfatfind.h"
#include "haiclib.h"
#include "hfatdent.h"
#include "hfatstr.h"
#include "hfatsys.h"
#include "hfatutil.h"


long _hai_findfirstOnFat(S_BYTE disk, S_BYTE *filespec, struct finddata *fileinfo)
{
	S_DWORD clust;
	_SP_FINEINFO pFindInf;

	if ((pFindInf = hai_FatMemAlloc(sizeof(*pFindInf)))==S_NULL)
		return 0;
	if (hai_strlen(filespec)>=FINDINFO_FILEFILTER_MAX)
		return 0;

	clust = _hai_CwdOperate(CWDOPERATE_GETCLUS, S_NULL);
	pFindInf->dev = hai_GetDeviceNum(disk);
	if (DevInf_FatType(pFindInf->dev)==FAT_TYPE_16 && clust == 0)
		pFindInf->SenNum = DevInf_RsvdSec(pFindInf->dev)+DevInf_NumFats(pFindInf->dev)*DevInf_FatSize(pFindInf->dev);
	else
	{
		if (clust == 0)//fat32 root
			clust = DevInf_RootClus(pFindInf->dev);
		pFindInf->SenNum = DevInf_FirstDatSec(pFindInf->dev)+(clust-2)*DevInf_SecPerClust(pFindInf->dev);
	}
	pFindInf->DentNum = 0;
	hai_strcpy(pFindInf->FileFilter, filespec);

	if (_hai_findnextOnFat((long)pFindInf, fileinfo))
	{
		hai_FatMemFree(pFindInf);
		pFindInf = S_NULL;
	}

	return (long)pFindInf;
}

void hai_FillFindData(struct finddata *fileinfo, S_BYTE *DentBuf, S_BYTE *filename)
{
	fileinfo->attrib    = *pDIR_Attr(DentBuf);
	fileinfo->CreatTime = ((S_WORD)(*pDIR_CrtTime(DentBuf))<<16)|*pDIR_CrtDate(DentBuf);//TODO:
	fileinfo->AccesTime = *pDIR_LastAccDate(DentBuf);
	fileinfo->WriteTime = ((S_WORD)(*pDIR_WrtTime(DentBuf))<<16)|*pDIR_WrtDate(DentBuf);
	fileinfo->size      = *pDIR_FileSize(DentBuf);
	hai_strcpy(fileinfo->name, filename);
}

int hai_findnextOnFat16Root(long handle, struct finddata *fileinfo)
{
	S_BYTE ChkSum, LDirOrd = DELETED_DIR_FLAG;
	S_BYTE DentBuf[FAT_DIRENT_SIZE];
	S_BYTE *pSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	S_BYTE tmpname[HAI_FILENAME_MAX+4];
	S_DWORD RootSiz;
	_SP_FINEINFO pFindInf = (_SP_FINEINFO)handle;
	int dev = pFindInf->dev;

	RootSiz  = DevInf_RsvdSec(dev)+DevInf_NumFats(dev)*DevInf_FatSize(dev);
	RootSiz += ((DevInf_RootEntCnt(dev)*FAT_DIRENT_SIZE)+(DevInf_BytsPerSec(dev)-1))/DevInf_BytsPerSec(dev);
	hai_memset(tmpname, 0x00, sizeof(tmpname));
	pSecBuf = SecBuf + pFindInf->DentNum*FAT_DIRENT_SIZE;

	for (; pFindInf->SenNum < RootSiz; pFindInf->SenNum++)
	{//TODO:
		if (_hai_FatDevRead(dev, pFindInf->SenNum, SecBuf))
			return FATERR_DEV_OPERATE_ERR;
		for (; (pSecBuf-SecBuf)<DevInf_BytsPerSec(dev);pSecBuf += FAT_DIRENT_SIZE)
		{
			if (*pDIR_Name(pSecBuf)==EMPTY_DIR_FLAG)//finish
				return FATERR_END_OF_FILE;
			else if (*pDIR_Name(pSecBuf)==DELETED_DIR_FLAG)//invalid dir entry
				LDirOrd = DELETED_DIR_FLAG;
			else if ((*pLDIR_Attr(pSecBuf)&FILE_ATTR_LONG_NAME_MASK)==FILE_ATTR_LONG_NAME)//long name sub-component
			{
				if (*pLDIR_Ord(pSecBuf)&LDIR_LAST_LONG_ENTRY)
				{
					LDirOrd = (*pLDIR_Ord(pSecBuf)&~LDIR_LAST_LONG_ENTRY)-1;
					ChkSum = *pLDIR_Chksum(pSecBuf);
					if (!hai_GetFileNameFromLDent(pSecBuf, tmpname+LDirOrd*LDIR_STR_BYTE_MAX))
						LDirOrd = DELETED_DIR_FLAG;
				}
				else if (LDirOrd==*pLDIR_Ord(pSecBuf) && ChkSum==*pLDIR_Chksum(pSecBuf))
				{
					LDirOrd--;
					if (!hai_GetFileNameFromLDent(pSecBuf, tmpname+LDirOrd*LDIR_STR_BYTE_MAX))
						LDirOrd = DELETED_DIR_FLAG;
				}
				else
				{
					LDirOrd = DELETED_DIR_FLAG;
				}
			}
			else if (((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == 0x00)// find a file
					||((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_DIRECTORY))// find a directory
			{
				if (LDirOrd == 0)//have long name
				{
					hai_Unicode2OtherCode(tmpname);
					if (hai_ShortFileNameChkSum(pSecBuf) != ChkSum)//long name is error
						hai_GetFileNameFromDent(pSecBuf, tmpname);
				}
				else
				{
					hai_GetFileNameFromDent(pSecBuf, tmpname);
				}
				hai_memcpy(DentBuf, pSecBuf, FAT_DIRENT_SIZE);
				pFindInf->DentNum = (pSecBuf-SecBuf)/FAT_DIRENT_SIZE + 1;
				hai_FillFindData(fileinfo, DentBuf, tmpname);
				return FATERR_NO_ERROR;
			}
//			else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_DIRECTORY)// find a directory
//			{
//			}
			else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_VOLUME_ID)// find a volume lab
				LDirOrd = DELETED_DIR_FLAG;
			else
				LDirOrd = DELETED_DIR_FLAG;// illegal dir entry
		}
		pSecBuf = SecBuf;
	}

	return FATERR_END_OF_FILE;
}

int __hai_findnextOnFat(long handle, struct finddata *fileinfo)
{
	S_BYTE ChkSum, LDirOrd = DELETED_DIR_FLAG;
	S_BYTE DentBuf[FAT_DIRENT_SIZE];
	S_BYTE *pSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	S_BYTE tmpname[HAI_FILENAME_MAX+4];
	S_DWORD SecEnd, FirstSec, clust;
	_SP_FINEINFO pFindInf = (_SP_FINEINFO)handle;
	int dev = pFindInf->dev;

	if (DevInf_FatType(dev) == FAT_TYPE_16)
	{
		FirstSec  = DevInf_RsvdSec(dev)+DevInf_NumFats(dev)*DevInf_FatSize(dev);
		FirstSec += ((DevInf_RootEntCnt(dev)*FAT_DIRENT_SIZE)+(DevInf_BytsPerSec(dev)-1))/DevInf_BytsPerSec(dev);
		if (pFindInf->SenNum < FirstSec)
			return hai_findnextOnFat16Root(handle, fileinfo);
	}

	hai_memset(tmpname, 0x00, sizeof(tmpname));
	FirstSec = pFindInf->SenNum - DevInf_FirstDatSec(dev);
	clust  = FirstSec/DevInf_SecPerClust(dev) + 2;

	FirstSec = DevInf_FirstDatSec(dev)+(clust-2)*DevInf_SecPerClust(dev);
	SecEnd = FirstSec + DevInf_SecPerClust(dev);
	
	FirstSec = pFindInf->SenNum;
	pSecBuf = SecBuf + pFindInf->DentNum * FAT_DIRENT_SIZE;

	while (DevInf_FatType(dev)==FAT_TYPE_16?IS_FAT16_INUSE_CLUST(clust):IS_FAT32_INUSE_CLUST(clust))
	{
		for (; FirstSec < SecEnd; FirstSec++)
		{
			if (_hai_FatDevRead(dev, FirstSec, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			for (; (pSecBuf-SecBuf)<DevInf_BytsPerSec(dev);pSecBuf += FAT_DIRENT_SIZE)
			{
				if (*pDIR_Name(pSecBuf)==EMPTY_DIR_FLAG)//finish
					return FATERR_END_OF_FILE;
				else if (*pDIR_Name(pSecBuf)==DELETED_DIR_FLAG)//invalid dir entry
					LDirOrd = DELETED_DIR_FLAG;
				else if ((*pLDIR_Attr(pSecBuf)&FILE_ATTR_LONG_NAME_MASK)==FILE_ATTR_LONG_NAME)//long name sub-component
				{
					if (*pLDIR_Ord(pSecBuf)&LDIR_LAST_LONG_ENTRY)
					{
						LDirOrd = (*pLDIR_Ord(pSecBuf)&~LDIR_LAST_LONG_ENTRY)-1;
						ChkSum = *pLDIR_Chksum(pSecBuf);
						if (!hai_GetFileNameFromLDent(pSecBuf, tmpname+LDirOrd*LDIR_STR_BYTE_MAX))
							LDirOrd = DELETED_DIR_FLAG;
					}
					else if (LDirOrd==*pLDIR_Ord(pSecBuf) && ChkSum==*pLDIR_Chksum(pSecBuf))
					{
						LDirOrd--;
						if (!hai_GetFileNameFromLDent(pSecBuf, tmpname+LDirOrd*LDIR_STR_BYTE_MAX))
							LDirOrd = DELETED_DIR_FLAG;
					}
					else
					{
						LDirOrd = DELETED_DIR_FLAG;
					}
				}
				else if (((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == 0x00)// find a file
						||((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_DIRECTORY))// find a directory
				{
					if (LDirOrd == 0)//have long name
					{
						hai_Unicode2OtherCode(tmpname);
						if (hai_ShortFileNameChkSum(pSecBuf) != ChkSum)//long name is error
							hai_GetFileNameFromDent(pSecBuf, tmpname);
					}
					else
					{
						hai_GetFileNameFromDent(pSecBuf, tmpname);
					}
					hai_memcpy(DentBuf, pSecBuf, FAT_DIRENT_SIZE);
					pFindInf->SenNum  = FirstSec;
					pFindInf->DentNum = (pSecBuf-SecBuf)/FAT_DIRENT_SIZE + 1;
					hai_FillFindData(fileinfo, DentBuf, tmpname);
					return FATERR_NO_ERROR;
				}
//				else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_DIRECTORY)// find a directory
//				{
//				}
				else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_VOLUME_ID)// find a volume lab
					LDirOrd = DELETED_DIR_FLAG;
				else
					LDirOrd = DELETED_DIR_FLAG;// illegal dir entry
			}
			pSecBuf = SecBuf;
		}
		clust = hai_GetNextCluster(dev, clust);
		FirstSec = DevInf_FirstDatSec(dev) + (clust-2)*DevInf_SecPerClust(dev);//get first sec of clust;
		SecEnd = FirstSec+DevInf_SecPerClust(dev);
	}

	return FATERR_END_OF_FILE;
}

int hai_filefilter(S_BYTE *filerilter, S_BYTE *filename)
{
	return 0;
}

int _hai_findnextOnFat(long handle, struct finddata *fileinfo)
{
	int err;
	_SP_FINEINFO pFindInf = (_SP_FINEINFO)handle;

	do{
		err = __hai_findnextOnFat(handle, fileinfo);
	}while (!err && hai_filefilter(pFindInf->FileFilter, fileinfo->name));

	return err;
}


