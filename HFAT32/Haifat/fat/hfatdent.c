//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatdent.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-18  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"
#include "hfatdent.h"
#include "ftdevice.h"
#include "hfatsys.h"
#include "hfatutil.h"
#include "haiclib.h"
#include "hfatstr.h"

int _hai_TruncateFile(int dev, S_DWORD SecNum, S_WORD DentNum)
{
	int err;
	S_BYTE *pSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	S_WORD ClustHi;
	S_WORD ClustLo;
	S_DWORD FstClust;

	if (err = _hai_FatDevRead(dev, SecNum, SecBuf))
		return err;
	pSecBuf = SecBuf+DentNum*FAT_DIRENT_SIZE;
	ClustHi = HAI_MAKEWORD(pDIR_FstClusHI(pSecBuf));
	ClustLo = HAI_MAKEWORD(pDIR_FstClusLO(pSecBuf));
	FstClust = HAI_COMBDWORD(ClustHi, ClustLo);
	if ((err = hai_UnlinkClust(dev, FstClust))==0x00)
	{
		HAI_WRITEWORD(pDIR_FstClusHI(pSecBuf), 0x00);
		HAI_WRITEWORD(pDIR_FstClusLO(pSecBuf), 0x00);
		HAI_WRITEDWORD(pDIR_FileSize(pSecBuf), 0x00);
		err = _hai_FatDevWrite(dev, SecNum, SecBuf);
	}
	
	return err;
}

S_BYTE hai_ShortFileNameChkSum(S_BYTE *pName)
{
	int i = 0;
	S_BYTE sum = 0;

	for (i = 0; i < 11; i++)
		sum = ((sum&1)?0x80:0x00) + (sum>>1) + pName[i];
	
	return sum;
}

void hai_FillDent(S_BYTE *dent, S_BYTE *Name83, S_BYTE attr, S_DWORD filesize, S_DWORD clust)
{
	S_WORD Time = hai_GetTime16();
	S_WORD Date = hai_GetDate16();

	hai_memset(dent, 0x00, FAT_DIRENT_SIZE);
	hai_WriteShortName2Dent(dent, Name83);
	*pDIR_Attr(dent) = attr;
	*pDIR_CrtTimeTenth(dent) = hai_GetTimeTenth8();
	HAI_WRITEWORD(pDIR_CrtTime(dent), Time);
	HAI_WRITEWORD(pDIR_CrtDate(dent), Date);
	HAI_WRITEWORD(pDIR_LastAccDate(dent), Date);
	HAI_WRITEWORD(pDIR_FstClusHI(dent), clust>>16);
	HAI_WRITEWORD(pDIR_WrtTime(dent), Time);
	HAI_WRITEWORD(pDIR_WrtDate(dent), Date);
	HAI_WRITEWORD(pDIR_FstClusLO(dent), clust);
	HAI_WRITEDWORD(pDIR_FileSize(dent), filesize);
}

void hai_UpdateDent(S_BYTE *dent, S_BYTE IsWrite)
{
	S_WORD Time = hai_GetTime16();
	S_WORD Date = hai_GetDate16();

	HAI_WRITEWORD(pDIR_LastAccDate(dent), Date);
	if (IsWrite)
	{
		HAI_WRITEWORD(pDIR_WrtTime(dent), Time);
		HAI_WRITEWORD(pDIR_WrtDate(dent), Date);
	}
}

S_WORD hai_FillLDent(S_BYTE *dent, const S_BYTE *LongName, S_BYTE ChkSum, S_BYTE Order)
{
	S_BYTE NameBuf[LDIR_STR_BYTE_MAX];
	S_WORD len = hai_CountByteUni(LongName);

	if (len > LDIR_STR_BYTE_MAX)
		len = LDIR_STR_BYTE_MAX;
	hai_memcpy(NameBuf, LongName, len);
	if (len < LDIR_STR_BYTE_MAX)
	{
		NameBuf[len] = 0, NameBuf[len+1] = 0;
		hai_memset(NameBuf+len+2, 0xFF, LDIR_STR_BYTE_MAX-len-2);
	}

	*pLDIR_Ord(dent) = Order;
	*pLDIR_Attr(dent) = FILE_ATTR_LONG_NAME;
	*pLDIR_Type(dent) = 0;
	*pLDIR_Chksum(dent) = ChkSum;
	HAI_WRITEWORD(pLDIR_FstClusLO(dent), 0x00);
	hai_memcpy(pLDIR_Name1(dent), NameBuf, sLDIR_Name1);
	hai_memcpy(pLDIR_Name2(dent), NameBuf+sLDIR_Name1, sLDIR_Name2);
	hai_memcpy(pLDIR_Name3(dent), NameBuf+sLDIR_Name1+sLDIR_Name2, sLDIR_Name3);

	return (S_WORD)len;
}

#if 0
int hai_FindDentOnFAT16Root(int dev, S_BYTE *dentName, S_BYTE *dentbuf)
{
	S_BYTE LDirOrd = DELETED_DIR_FLAG;
	S_BYTE *pSecBuf;
	S_DWORD RootSec = DevInf_RsvdSec(dev)+DevInf_NumFats(dev)*DevInf_FatSize(dev);
	S_DWORD RootSiz = ((DevInf_RootEntCnt(dev)*FAT_DIRENT_SIZE)+(DevInf_BytsPerSec(dev)-1))/DevInf_BytsPerSec(dev);
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];
	S_BYTE tmpname[HAI_FILENAME_MAX+4], *pLDir = tmpname;

	if (!*dentName || IS_FAT_CUR_DIR(dentName) || IS_FAT_BACK_DIR(dentName))
		return FATERR_INVALID_PATH;

	hai_memset(tmpname, 0x00, sizeof(tmpname));

	for (RootSiz += RootSec; RootSec < RootSiz; RootSec++)
	{
		if (_hai_FatDevRead(dev, RootSec, SecBuf))
			return FATERR_DEV_OPERATE_ERR;
		for (pSecBuf = SecBuf; (pSecBuf-SecBuf)<DevInf_BytsPerSec(dev);pSecBuf += FAT_DIRENT_SIZE)
		{
			if (*pDIR_Name(pSecBuf)==EMPTY_DIR_FLAG)//finish
			{
				if (!hai_stricmp(dentName, tmpname))//compare previous
					return 0;						
				return FATERR_INVALID_PATH;
			}
			else if (*pDIR_Name(pSecBuf)==DELETED_DIR_FLAG)//invalid dir entry
				LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;
			else if ((*pLDIR_Attr(pSecBuf)&FILE_ATTR_LONG_NAME_MASK)==FILE_ATTR_LONG_NAME)//long name sub-component
			{
				if (LDirOrd+1 == (*pLDIR_Ord(pSecBuf)&~LDIR_LAST_LONG_ENTRY))
				{
					int num = hai_GetFileNameFromLDent(pSecBuf, pLDir);
					if (!num)
					{
						LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;
						continue;
					}
					if (*pLDIR_Ord(pSecBuf)&LDIR_LAST_LONG_ENTRY)
					{
						if (hai_ShortFileNameChkSum(dentbuf)==*pLDIR_Chksum(pSecBuf) && !hai_stricmp(dentName, tmpname))
							return 0;
						LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;
					}
					else
					{
						if (num == LDIR_STR_CHAR_MAX)//because it stores 26 char in one long dent
							LDirOrd += 1, pLDir += num;
						else//be error
							LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;
					}
				}
			}
			else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == 0x00)// find a file
			{
				if (!hai_stricmp(dentName, tmpname))//compare previous
					return 0;						
				LDirOrd = 0, pLDir = tmpname;
				hai_memcpy(dentbuf, pSecBuf, FAT_DIRENT_SIZE);
				hai_GetFileNameFromDent(dentbuf, tmpname);
			}
			else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_DIRECTORY)// find a directory
			{
				if (!hai_stricmp(dentName, tmpname))//compare previous
					return 0;						
				LDirOrd = 0, pLDir = tmpname;
				hai_memcpy(dentbuf, pSecBuf, FAT_DIRENT_SIZE);
				hai_GetFileNameFromDent(dentbuf, tmpname);
			}
			else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_VOLUME_ID)// find a volume lab
				LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;
			else
				LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;// illegal dir entry
		}
	}

	return FATERR_INVALID_PATH;
}

int hai_GetDent(int dev, S_DWORD StartClust, S_BYTE *filename, S_BYTE *dentbuf)
{
	S_BYTE LDirOrd = DELETED_DIR_FLAG;
	int err = FATERR_INVALID_PATH;
	S_DWORD SecStart, SecEnd;
	S_BYTE *pSecBuf;
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];
	S_BYTE tmpname[HAI_FILENAME_MAX+4], *pLDir = tmpname;

	if (DevInf_FatType(dev)== FAT_TYPE_16 && StartClust == 0)
		return hai_FindDentOnFAT16Root(dev, filename, dentbuf);

	if (StartClust == 0) // FAT32 rootclust
		StartClust = DevInf_RootClus(dev);

	hai_memset(tmpname, 0x00, sizeof(tmpname));

	while (DevInf_FatType(dev)==FAT_TYPE_16?IS_FAT16_INUSE_CLUST(StartClust):IS_FAT32_INUSE_CLUST(StartClust))
	{
		SecStart = DevInf_FirstDatSec(dev) + (StartClust-2)*DevInf_SecPerClust(dev);//get first sec of clust;
		for (SecEnd = SecStart+DevInf_SecPerClust(dev); SecStart < SecEnd; SecStart++)
		{
			if (_hai_FatDevRead(dev, SecStart, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			for (pSecBuf = SecBuf; (pSecBuf-SecBuf)<DevInf_BytsPerSec(dev);pSecBuf += FAT_DIRENT_SIZE)
			{
				if (*pDIR_Name(pSecBuf)==EMPTY_DIR_FLAG)//finish
				{
					if (!hai_stricmp(filename, tmpname))//compare previous
						return 0;						
					return FATERR_INVALID_PATH;
				}
				else if (*pDIR_Name(pSecBuf)==DELETED_DIR_FLAG)//invalid dir entry
					LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;
				else if ((*pLDIR_Attr(pSecBuf)&FILE_ATTR_LONG_NAME_MASK)==FILE_ATTR_LONG_NAME)//long name sub-component
				{
					if (LDirOrd+1 == (*pLDIR_Ord(pSecBuf)&~LDIR_LAST_LONG_ENTRY))
					{
						int num = hai_GetFileNameFromLDent(pSecBuf, pLDir);
						if (!num)
						{
							LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;
							continue;
						}
						if (*pLDIR_Ord(pSecBuf)&LDIR_LAST_LONG_ENTRY)
						{
							if (hai_ShortFileNameChkSum(dentbuf)==*pLDIR_Chksum(pSecBuf) && !hai_stricmp(filename, tmpname))
								return 0;
							LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;
						}
						else
						{
							if (num == LDIR_STR_CHAR_MAX)//because it stores 26 char in one long dent
								LDirOrd += 1, pLDir += num;
							else//be error
								LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;
						}
					}
				}
				else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == 0x00)// find a file
				{
					if (!hai_stricmp(filename, tmpname))//compare previous
						return 0;						
					LDirOrd = 0, pLDir = tmpname;
					hai_memcpy(dentbuf, pSecBuf, FAT_DIRENT_SIZE);
					hai_GetFileNameFromDent(dentbuf, tmpname);
				}
				else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_DIRECTORY)// find a directory
				{
					if (!hai_stricmp(filename, tmpname))
						return 0;						
					LDirOrd = 0, pLDir = tmpname;
					hai_memcpy(dentbuf, pSecBuf, FAT_DIRENT_SIZE);
					hai_GetFileNameFromDent(dentbuf, tmpname);
				}
				else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_VOLUME_ID)// find a volume lab
					LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;
				else
					LDirOrd = DELETED_DIR_FLAG, pLDir = tmpname;// illegal dir entry
			}
		}
		StartClust = hai_GetNextCluster(dev, StartClust);
	}
	
	return err;
}
#endif

int hai_SearchDent(int Dev, S_DWORD StartClust, const S_BYTE *path, S_BYTE *dentbuf)
{
	int err = 0;
	S_BYTE dirname[HAI_FILENAME_MAX+4];

	if (DevInf_FatType(Dev) == FAT_TYPE_16 && StartClust == 0) // FAT32 rootclust
		StartClust = DevInf_RootClus(Dev);
	*pDIR_Attr(dentbuf) = FILE_ATTR_DIRECTORY;
	HAI_WRITEWORD(pDIR_FstClusHI(dentbuf), StartClust>>16);
	HAI_WRITEWORD(pDIR_FstClusLO(dentbuf), StartClust);

	hai_memset(dirname, 0x00, sizeof(dirname));
	path = hai_GetOneDirName(path, dirname);
	while (dirname[0])
	{
		StartClust = GetDentClust(dentbuf);
		if (!IS_FAT_CUR_DIR(dirname))
			err = hai_FindDent(Dev, StartClust, dirname, dentbuf);
		if (err || !IS_FILE_ATTR(dentbuf, FILE_ATTR_DIRECTORY))
		{
			if (err == FATERR_NO_ERROR)
				err = FATERR_INVALID_PATH;
			break;
		}
		hai_memset(dirname, 0x00, sizeof(dirname));
		path = hai_GetOneDirName(path, dirname);
	}
	
	return err;
}

int hai_FindShortNameOnFAT16Root(int dev, S_BYTE *Name83, S_BYTE *dentbuf)
{
	S_BYTE *pSecBuf;
	S_DWORD RootSec = DevInf_RsvdSec(dev)+DevInf_NumFats(dev)*DevInf_FatSize(dev);
	S_DWORD RootSiz = ((DevInf_RootEntCnt(dev)*FAT_DIRENT_SIZE)+(DevInf_BytsPerSec(dev)-1))/DevInf_BytsPerSec(dev);
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];
	S_BYTE tmpname[HAI_FILENAME_MAX+4];

	if (!*Name83 || IS_FAT_CUR_DIR(Name83) || IS_FAT_BACK_DIR(Name83))
		return FATERR_INVALID_PATH;

	hai_memset(tmpname, 0x00, sizeof(tmpname));

	for (RootSiz += RootSec; RootSec < RootSiz; RootSec++)
	{
		if (_hai_FatDevRead(dev, RootSec, SecBuf))
			return FATERR_DEV_OPERATE_ERR;
		for (pSecBuf = SecBuf; (pSecBuf-SecBuf)<DevInf_BytsPerSec(dev);pSecBuf += FAT_DIRENT_SIZE)
		{
			if (*pDIR_Name(pSecBuf)==EMPTY_DIR_FLAG)//finish
				return FATERR_INVALID_PATH;
			else if (*pDIR_Name(pSecBuf)==DELETED_DIR_FLAG)//invalid dir entry
				;
			else if ((*pLDIR_Attr(pSecBuf)&FILE_ATTR_LONG_NAME_MASK)==FILE_ATTR_LONG_NAME)//long name sub-component
				;
			else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == 0x00)// find a file
			{
				hai_memcpy(dentbuf, pSecBuf, FAT_DIRENT_SIZE);
				hai_GetFileNameFromDent(dentbuf, tmpname);
				if (!hai_stricmp(Name83, tmpname))
					return 0;						
			}
			else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_DIRECTORY)// find a directory
			{
				hai_memcpy(dentbuf, pSecBuf, FAT_DIRENT_SIZE);
				hai_GetFileNameFromDent(dentbuf, tmpname);
				if (!hai_stricmp(Name83, tmpname))
					return 0;						
			}
			else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_VOLUME_ID)// find a volume lab
				;
			else
				;// illegal dir entry
		}
	}

	return FATERR_INVALID_PATH;

}

int hai_FindShortName(int dev, S_DWORD StartClust, S_BYTE *Name83, S_BYTE *dentbuf)
{
	int err = FATERR_INVALID_PATH;
	S_DWORD SecStart, SecEnd;
	S_BYTE *pSecBuf;
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];
	S_BYTE tmpname[HAI_FILENAME_MAX+4];

	if (DevInf_FatType(dev)== FAT_TYPE_16 && StartClust == 0)
		return hai_FindShortNameOnFAT16Root(dev, Name83, dentbuf);

	if (StartClust == 0) // FAT32 rootclust
		StartClust = DevInf_RootClus(dev);

	hai_memset(tmpname, 0x00, sizeof(tmpname));

	while (DevInf_FatType(dev)==FAT_TYPE_16?IS_FAT16_INUSE_CLUST(StartClust):IS_FAT32_INUSE_CLUST(StartClust))
	{
		SecStart = DevInf_FirstDatSec(dev) + (StartClust-2)*DevInf_SecPerClust(dev);//get first sec of clust;
		for (SecEnd = SecStart+DevInf_SecPerClust(dev); SecStart < SecEnd; SecStart++)
		{
			if (_hai_FatDevRead(dev, SecStart, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			for (pSecBuf = SecBuf; (pSecBuf-SecBuf)<DevInf_BytsPerSec(dev);pSecBuf += FAT_DIRENT_SIZE)
			{
				if (*pDIR_Name(pSecBuf)==EMPTY_DIR_FLAG)//finish
					return FATERR_INVALID_PATH;
				else if (*pDIR_Name(pSecBuf)==DELETED_DIR_FLAG)//invalid dir entry
					;
				else if ((*pLDIR_Attr(pSecBuf)&FILE_ATTR_LONG_NAME_MASK)==FILE_ATTR_LONG_NAME)//long name sub-component
					;
				else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == 0x00)// find a file
				{
					hai_memcpy(dentbuf, pSecBuf, FAT_DIRENT_SIZE);
					hai_GetFileNameFromDent(dentbuf, tmpname);
					if (!hai_stricmp(Name83, tmpname))
						return 0;						
				}
				else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_DIRECTORY)// find a directory
				{
					hai_memcpy(dentbuf, pSecBuf, FAT_DIRENT_SIZE);
					hai_GetFileNameFromDent(dentbuf, tmpname);
					if (!hai_stricmp(Name83, tmpname))
						return 0;						
				}
				else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_VOLUME_ID)// find a volume lab
					;
				else
					;// illegal dir entry
			}
		}
		StartClust = hai_GetNextCluster(dev, StartClust);
	}
	
	return err;
}

S_DWORD hai_AllocDentOnFAT16Root(int dev, int DentCnt, S_WORD *DentNum)
{
	int cnt = 0;
	S_BYTE *pSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	S_DWORD SecNum = 0;
	S_DWORD RootSec = DevInf_RsvdSec(dev)+DevInf_NumFats(dev)*DevInf_FatSize(dev);
	S_DWORD RootSiz = ((DevInf_RootEntCnt(dev)*FAT_DIRENT_SIZE)+(DevInf_BytsPerSec(dev)-1))/DevInf_BytsPerSec(dev);

	for (RootSiz += RootSec; RootSec < RootSiz; RootSec++)
	{
		if (_hai_FatDevRead(dev, RootSec, SecBuf))
			return 0;
		for (pSecBuf = SecBuf; (pSecBuf-SecBuf)<DevInf_BytsPerSec(dev);pSecBuf += FAT_DIRENT_SIZE)
		{
			if ((*pDIR_Name(pSecBuf)==EMPTY_DIR_FLAG)//finish dir
				|| (*pDIR_Name(pSecBuf)==DELETED_DIR_FLAG))// invalid dir entry
			{
				cnt++;
				if (SecNum == 0)
					SecNum = RootSec, *DentNum = (pSecBuf-SecBuf)/FAT_DIRENT_SIZE;
			}
			else
			{
				cnt = 0, *DentNum = 0, SecNum = 0;
			}
			if (cnt >= DentCnt)
				return SecNum;
		}
	}

	return 0;
}


S_DWORD hai_AllocDent(int dev, S_DWORD StartClust, int DentCnt, S_WORD *DentNum)
{
	int cnt = 0;
	S_BYTE *pSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	S_DWORD SecStart, SecEnd, SecNum = 0;
	S_DWORD TmpClust;

	if (DevInf_FatType(dev) == FAT_TYPE_16 && StartClust == 0)
		return hai_AllocDentOnFAT16Root(dev, DentCnt, DentNum);
	
	if (StartClust == 0) // FAT32 rootclust
		StartClust = DevInf_RootClus(dev);

	while (DevInf_FatType(dev)==FAT_TYPE_16?IS_FAT16_INUSE_CLUST(StartClust):IS_FAT32_INUSE_CLUST(StartClust))
	{
		SecStart = DevInf_FirstDatSec(dev) + (StartClust-2)*DevInf_SecPerClust(dev);//get first sec of clust;
		for (SecEnd = SecStart+DevInf_SecPerClust(dev); SecStart < SecEnd; SecStart++)
		{
			if (_hai_FatDevRead(dev, SecStart, SecBuf))
				return 0;
			for (pSecBuf = SecBuf; (pSecBuf-SecBuf)<DevInf_BytsPerSec(dev);pSecBuf += FAT_DIRENT_SIZE)
			{
				if ((*pDIR_Name(pSecBuf)==EMPTY_DIR_FLAG)//finish
					 || (*pDIR_Name(pSecBuf)==DELETED_DIR_FLAG))//invalid dir entry
				{
					cnt++;
					if (SecNum == 0)
						SecNum = SecStart, *DentNum = (pSecBuf-SecBuf)/FAT_DIRENT_SIZE;
				}
				else
				{
					cnt = 0, *DentNum = 0, SecNum = 0;
				}
				if (cnt >= DentCnt)
					return SecNum;
			}
		}
		StartClust = hai_GetNextCluster(dev, StartClust);
		if (!(DevInf_FatType(dev)==FAT_TYPE_16?IS_FAT16_INUSE_CLUST(StartClust):IS_FAT32_INUSE_CLUST(StartClust)))
		{
			if ((TmpClust = hai_AllocIdleClust(dev)) == 0)
				break;
			if (!hai_LinkClust(dev, StartClust, TmpClust))
				StartClust = TmpClust;
		}
	}

	return 0;
}

int hai_WriteDentOnFat16Root(int dev, S_DWORD SecNum, S_WORD DentNum, const S_BYTE *LongName, S_BYTE *ShortDent)
{
	S_BYTE ChkSum, Order;
	S_WORD num, DentCnt = 1;
	S_BYTE *pSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	S_BYTE UniStr[HAI_FILENAME_MAX+4];

	if (LongName && LongName[0])
	{
		DentCnt += (S_WORD)((hai_CountChar(LongName)+LDIR_STR_CHAR_MAX-1)/LDIR_STR_CHAR_MAX);
		ChkSum = hai_ShortFileNameChkSum(ShortDent);
		hai_OtherCode2Unicode(UniStr, LongName);
	}
	
	if (_hai_FatDevRead(dev, SecNum, SecBuf))
		return FATERR_DEV_OPERATE_ERR;
	pSecBuf = SecBuf+DentNum*FAT_DIRENT_SIZE;

	for (num = DentCnt; num > 0; num--)
	{
		if (pSecBuf == SecBuf+DevInf_BytsPerSec(dev))
		{
			if (_hai_FatDevWrite(dev, SecNum, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			if (_hai_FatDevRead(dev, ++SecNum, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			pSecBuf = SecBuf;
		}
		if (num == 1)
		{
			hai_memcpy(pSecBuf, ShortDent, FAT_DIRENT_SIZE);
			pSecBuf += FAT_DIRENT_SIZE;
		}
		else
		{
			Order = num - 1;
			if (num == DentCnt)
				Order |= LDIR_LAST_LONG_ENTRY;
			hai_FillLDent(pSecBuf, UniStr+(num-2)*LDIR_STR_BYTE_MAX, ChkSum, Order);
			pSecBuf  += FAT_DIRENT_SIZE;
		}
	}
	if (_hai_FatDevWrite(dev, SecNum, SecBuf))
		return FATERR_DEV_OPERATE_ERR;

	return 0;
}

int hai_WriteDent(int dev, S_DWORD SecNum, S_WORD DentNum, const S_BYTE *LongName, S_BYTE *ShortDent)
{
	S_BYTE Order, ChkSum;
	S_WORD num, DentCnt = 1;
	S_BYTE *pSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	S_DWORD FirstSec, TmpClust;
	S_BYTE UniStr[HAI_FILENAME_MAX+4];

	if (DevInf_FatType(dev) == FAT_TYPE_16)
	{
		TmpClust   = DevInf_RsvdSec(dev)+DevInf_NumFats(dev)*DevInf_FatSize(dev);
		TmpClust += ((DevInf_RootEntCnt(dev)*FAT_DIRENT_SIZE)+(DevInf_BytsPerSec(dev)-1))/DevInf_BytsPerSec(dev);
		if (SecNum < TmpClust)
			return hai_WriteDentOnFat16Root(dev, SecNum, DentNum, LongName, ShortDent);
	}
	
	SecNum -= DevInf_FirstDatSec(dev);
	TmpClust = SecNum/DevInf_SecPerClust(dev) + 2;
	FirstSec = DevInf_FirstDatSec(dev)+(TmpClust-2)*DevInf_SecPerClust(dev);
	SecNum = SecNum%DevInf_SecPerClust(dev);
	
	if (LongName && LongName[0])
	{
		DentCnt += (S_WORD)((hai_CountChar(LongName)+LDIR_STR_CHAR_MAX-1)/LDIR_STR_CHAR_MAX);
		ChkSum = hai_ShortFileNameChkSum(ShortDent);
		hai_OtherCode2Unicode(UniStr, LongName);
	}
	
	if (_hai_FatDevRead(dev, FirstSec+SecNum, SecBuf))
		return FATERR_DEV_OPERATE_ERR;
	pSecBuf = SecBuf+DentNum*FAT_DIRENT_SIZE;

	for (num = DentCnt; num > 0; num--)
	{
		if (pSecBuf == SecBuf+DevInf_BytsPerSec(dev))
		{
			if (_hai_FatDevWrite(dev, FirstSec+SecNum, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			if (SecNum+1 == DevInf_SecPerClust(dev))//clust end
			{
				TmpClust = hai_GetNextCluster(dev, TmpClust);
				FirstSec = DevInf_FirstDatSec(dev)+(TmpClust-2)*DevInf_SecPerClust(dev);
				SecNum   = 0;
			}
			else
				SecNum++;
			if (_hai_FatDevRead(dev, FirstSec+SecNum, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			pSecBuf = SecBuf;
		}
		if (num == 1)
		{
			hai_memcpy(pSecBuf, ShortDent, FAT_DIRENT_SIZE);
			pSecBuf += FAT_DIRENT_SIZE;
		}
		else
		{
			Order = num - 1;
			if (num == DentCnt)
				Order |= LDIR_LAST_LONG_ENTRY;
			hai_FillLDent(pSecBuf, UniStr+(num-2)*LDIR_STR_BYTE_MAX, ChkSum, Order);
			pSecBuf  += FAT_DIRENT_SIZE;
		}
	}
	if (_hai_FatDevWrite(dev, FirstSec+SecNum, SecBuf))
		return FATERR_DEV_OPERATE_ERR;

	return 0;
}

int hai_InitDirectory(int dev, S_DWORD CurClus, S_DWORD PreClus)
{
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];
	S_DWORD SecNum;

	if (DevInf_FatType(dev) == FAT_TYPE_32 && PreClus == DevInf_RootClus(dev))
		PreClus = 0;

	hai_memset(SecBuf, 0x00, sizeof(SecBuf));
	//init dent .
	hai_FillDent(SecBuf, "", FILE_ATTR_DIRECTORY|FILE_ATTR_READ_ONLY, 0x00, CurClus);
	*pDIR_Name(SecBuf) = '.'; 
	//init dent ..
	hai_FillDent(SecBuf+FAT_DIRENT_SIZE, "", FILE_ATTR_DIRECTORY|FILE_ATTR_READ_ONLY, 0x00, PreClus);
	*(pDIR_Name(SecBuf)+FAT_DIRENT_SIZE) = '.'; 
	*(pDIR_Name(SecBuf)+FAT_DIRENT_SIZE+1) = '.';

	SecNum = DevInf_FirstDatSec(dev) + (CurClus-2)*DevInf_SecPerClust(dev);
	if (_hai_FatDevWrite(dev, SecNum, SecBuf))
		return FATERR_DEV_OPERATE_ERR;
#if 0
	hai_memset(SecBuf, 0x00, sizeof(SecBuf));
	for (Sec = 1; Sec < DevInf_SecPerClust(dev); Sec++)
	{
		if (_hai_FatDevWrite(dev, SecNum+Sec, SecBuf))
			return FATERR_DEV_OPERATE_ERR;
	}
#endif	
	return 0;
}

int hai_CreateDirDent(int dev, S_DWORD StartClust, S_BYTE *pName, S_BYTE *dentbuf)
{
	int LongName;
	int err, DentCnt = 1;
	S_BYTE Name83[16], dentbuf2[FAT_DIRENT_SIZE];
	S_WORD dentNum;
	S_DWORD SecNum, IdleClus;
	
	if (IS_FILE_ATTR(dentbuf, FILE_ATTR_VOLUME_ID))
		return FATERR_UNKNOWN_ERR;
	if (hai_Make83Format(pName, Name83)==0)
		return FATERR_INVALID_FILENAME;

	while (!hai_FindShortName(dev, StartClust, Name83, dentbuf2))
		hai_MakeNumericTailName(Name83);
	if (LongName = hai_strcmp(pName, Name83))
	{
		DentCnt += (hai_CountChar(pName)+(LDIR_STR_CHAR_MAX-1))/LDIR_STR_CHAR_MAX;
		hai_memset(dentbuf2, 0x00, sizeof(dentbuf2));
	}
	else
	{
		pName = S_NULL;//only use Name83
	}
		
	SecNum = hai_AllocDent(dev, StartClust, DentCnt, &dentNum);
	if (!SecNum)
		return FATERR_NO_DISK_SPACE;
	IdleClus = hai_AllocIdleClust(dev);
	if (!IdleClus)
		return FATERR_NO_DISK_SPACE;
	err = hai_InitDirectory(dev, IdleClus, StartClust);
	if (!err)
	{
		hai_FillDent(dentbuf, Name83, *pDIR_Attr(dentbuf), 0x00, IdleClus);
		err = hai_WriteDent(dev, SecNum, dentNum, pName, dentbuf);
	}
	return err;
}

S_DWORD hai_CreateFileDent(int dev, S_DWORD clust, const S_BYTE *file, S_WORD *DentNum, S_BYTE *DentBuf)
{
	int LongName;
	int DentCnt = 1;
	S_BYTE Name83[16];
	S_DWORD SecNum;
	
	if (hai_Make83Format(file, Name83)==0)
		return 0;

	while (!hai_FindShortName(dev, clust, Name83, DentBuf))
		hai_MakeNumericTailName(Name83);
	if (LongName = hai_strcmp(file, Name83))
		DentCnt += (hai_CountChar(file)+(LDIR_STR_CHAR_MAX-1))/LDIR_STR_CHAR_MAX;
	else
		file = S_NULL;//only use Name83
		
	SecNum = hai_AllocDent(dev, clust, DentCnt, DentNum);
	if (SecNum)
	{
		hai_FillDent(DentBuf, Name83, 0x00, 0x00, 0x00);
		if (hai_WriteDent(dev, SecNum, *DentNum, file, DentBuf))
			SecNum = 0;
	}
	return SecNum;
}

int hai_LocationDentOnFAT16Root(int dev, const S_BYTE *DentName, S_DWORD *DSecNum, S_WORD *DentNum, S_DWORD *LDSecNum, S_WORD *LDentNum, S_BYTE *DentBuf)
{
	S_BYTE ChkSum, LDirOrd = DELETED_DIR_FLAG;
	S_BYTE *pSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	S_BYTE tmpname[HAI_FILENAME_MAX+4];
	S_DWORD RootSec = DevInf_RsvdSec(dev)+DevInf_NumFats(dev)*DevInf_FatSize(dev);
	S_DWORD RootSiz = ((DevInf_RootEntCnt(dev)*FAT_DIRENT_SIZE)+(DevInf_BytsPerSec(dev)-1))/DevInf_BytsPerSec(dev);

	for (RootSiz += RootSec; RootSec < RootSiz; RootSec++)
	{
		if (_hai_FatDevRead(dev, RootSec, SecBuf))
			return FATERR_DEV_OPERATE_ERR;
		for (pSecBuf = SecBuf; (pSecBuf-SecBuf)<DevInf_BytsPerSec(dev);pSecBuf += FAT_DIRENT_SIZE)
		{
			if (*pDIR_Name(pSecBuf)==EMPTY_DIR_FLAG)//finish
				return FATERR_INVALID_FILENAME;
			else if (*pDIR_Name(pSecBuf)==DELETED_DIR_FLAG)//invalid dir entry
				LDirOrd = DELETED_DIR_FLAG;
			else if ((*pLDIR_Attr(pSecBuf)&FILE_ATTR_LONG_NAME_MASK)==FILE_ATTR_LONG_NAME)//long name sub-component
			{
				if (*pLDIR_Ord(pSecBuf)&LDIR_LAST_LONG_ENTRY)
				{
					LDirOrd = (*pLDIR_Ord(pSecBuf)&~LDIR_LAST_LONG_ENTRY)-1;
					ChkSum = *pLDIR_Chksum(pSecBuf);
					*LDSecNum = RootSec, *LDentNum = (pSecBuf-SecBuf)/FAT_DIRENT_SIZE;
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
					if (hai_ShortFileNameChkSum(pSecBuf) != ChkSum)
						tmpname[0] = '\0';
				}
				else
				{
					hai_GetFileNameFromDent(pSecBuf, tmpname);
					*LDSecNum = RootSec, *LDentNum = (pSecBuf-SecBuf)/FAT_DIRENT_SIZE;
				}
				if (!hai_stricmp(DentName, tmpname))
				{
					hai_memcpy(DentBuf, pSecBuf, FAT_DIRENT_SIZE);
					*DentNum = (pSecBuf-SecBuf)/FAT_DIRENT_SIZE;
					*DSecNum = RootSec;
					return FATERR_NO_ERROR;
				}
				LDirOrd = DELETED_DIR_FLAG;
			}
//			else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_DIRECTORY)// find a directory
//			{
//			}
			else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_VOLUME_ID)// find a volume lab
				LDirOrd = DELETED_DIR_FLAG;
			else
				LDirOrd = DELETED_DIR_FLAG;// illegal dir entry
		}
	}

	return FATERR_INVALID_FILENAME;
}

int hai_LocationDent(int dev, S_DWORD clust, const S_BYTE *DentName, S_DWORD *DSecNum, S_WORD *DentNum, S_DWORD *LDSecNum, S_WORD *LDentNum, S_BYTE *DentBuf)
{
	S_BYTE ChkSum, LDirOrd = DELETED_DIR_FLAG;
	S_BYTE *pSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	S_DWORD SecStart, SecEnd;
	S_BYTE tmpname[HAI_FILENAME_MAX+4];

	if (DevInf_FatType(dev) == FAT_TYPE_16 && clust == 0)
		return hai_LocationDentOnFAT16Root(dev, DentName, DSecNum, DentNum, LDSecNum, LDentNum, DentBuf);
	
	if (clust == 0) // FAT32 rootclust
		clust = DevInf_RootClus(dev);

	hai_memset(tmpname, 0x00, sizeof(tmpname));

	while (DevInf_FatType(dev)==FAT_TYPE_16?IS_FAT16_INUSE_CLUST(clust):IS_FAT32_INUSE_CLUST(clust))
	{
		SecStart = DevInf_FirstDatSec(dev) + (clust-2)*DevInf_SecPerClust(dev);//get first sec of clust;
		for (SecEnd = SecStart+DevInf_SecPerClust(dev); SecStart < SecEnd; SecStart++)
		{
			if (_hai_FatDevRead(dev, SecStart, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			for (pSecBuf = SecBuf; (pSecBuf-SecBuf)<DevInf_BytsPerSec(dev);pSecBuf += FAT_DIRENT_SIZE)
			{
				if (*pDIR_Name(pSecBuf)==EMPTY_DIR_FLAG)//finish
					return FATERR_INVALID_FILENAME;
				else if (*pDIR_Name(pSecBuf)==DELETED_DIR_FLAG)//invalid dir entry
					LDirOrd = DELETED_DIR_FLAG;
				else if ((*pLDIR_Attr(pSecBuf)&FILE_ATTR_LONG_NAME_MASK)==FILE_ATTR_LONG_NAME)//long name sub-component
				{
					if (*pLDIR_Ord(pSecBuf)&LDIR_LAST_LONG_ENTRY)
					{
						LDirOrd = (*pLDIR_Ord(pSecBuf)&~LDIR_LAST_LONG_ENTRY)-1;
						ChkSum = *pLDIR_Chksum(pSecBuf);
						*LDSecNum = SecStart, *LDentNum = (pSecBuf-SecBuf)/FAT_DIRENT_SIZE;
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
						if (hai_ShortFileNameChkSum(pSecBuf) != ChkSum)
							tmpname[0] = '\0';
					}
					else
					{
						hai_GetFileNameFromDent(pSecBuf, tmpname);
						*LDSecNum = SecStart, *LDentNum = (pSecBuf-SecBuf)/FAT_DIRENT_SIZE;
					}
					if (!hai_stricmp(DentName, tmpname))
					{
						hai_memcpy(DentBuf, pSecBuf, FAT_DIRENT_SIZE);
						*DentNum = (pSecBuf-SecBuf)/FAT_DIRENT_SIZE;
						*DSecNum = SecStart;
						return FATERR_NO_ERROR;
					}
					LDirOrd = DELETED_DIR_FLAG;
				}
//				else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_DIRECTORY)// find a directory
//				{
//				}
				else if ((*pDIR_Attr(pSecBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == FILE_ATTR_VOLUME_ID)// find a volume lab
					LDirOrd = DELETED_DIR_FLAG;
				else
					LDirOrd = DELETED_DIR_FLAG;// illegal dir entry
			}
		}
		clust = hai_GetNextCluster(dev, clust);
	}

	return FATERR_INVALID_FILENAME;
}

int hai_FindDent(int dev, S_DWORD clust, const S_BYTE *DentName, S_BYTE *DentBuf)
{
	S_WORD DentNum, LDentNum;
	S_DWORD DSecNum, LDSecNum;

	return hai_LocationDent(dev, clust, DentName, &DSecNum, &DentNum, &LDSecNum, &LDentNum, DentBuf);
}

int hai_RemoveDentOnFat16Root(int dev, S_DWORD DSecNum, S_WORD DentNum, S_DWORD LDSecNum, S_WORD LDentNum, S_BYTE *DentBuf)
{
	S_BYTE *pSecBuf, *bSecBuf, SecBuf[FAT_SEC_BUF_MAX];

	if (_hai_FatDevRead(dev, LDSecNum, SecBuf))
		return FATERR_DEV_OPERATE_ERR;

	bSecBuf = SecBuf+DentNum*FAT_DIRENT_SIZE;
	pSecBuf = SecBuf+LDentNum*FAT_DIRENT_SIZE;
	while (1)
	{
		if (pSecBuf == SecBuf+DevInf_BytsPerSec(dev))
		{
			if (_hai_FatDevWrite(dev, LDSecNum, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			if (_hai_FatDevRead(dev, ++LDSecNum, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			pSecBuf = SecBuf;
		}
		*pDIR_Name(pSecBuf) = DELETED_DIR_FLAG;
		if (LDSecNum==DSecNum && pSecBuf==bSecBuf)
		{
			hai_memcpy(DentBuf, pSecBuf, FAT_DIRENT_SIZE);
			break;
		}
		pSecBuf  += FAT_DIRENT_SIZE;
	}
	if (_hai_FatDevWrite(dev, LDSecNum, SecBuf))
		return FATERR_DEV_OPERATE_ERR;

	return FATERR_NO_ERROR;
}

int hai_RemoveDent(int dev, S_DWORD DSecNum, S_WORD DentNum, S_DWORD LDSecNum, S_WORD LDentNum, S_BYTE *DentBuf)
{
	S_BYTE *pSecBuf, *bSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	S_DWORD FirstSec, SecOff, TmpClust;

	if (DevInf_FatType(dev) == FAT_TYPE_16)
	{
		FirstSec  = DevInf_RsvdSec(dev)+DevInf_NumFats(dev)*DevInf_FatSize(dev);
		FirstSec += ((DevInf_RootEntCnt(dev)*FAT_DIRENT_SIZE)+(DevInf_BytsPerSec(dev)-1))/DevInf_BytsPerSec(dev);
		if (LDSecNum < FirstSec)
			return hai_RemoveDentOnFat16Root(dev, DSecNum, DentNum, LDSecNum, LDentNum, DentBuf);
	}
	
	TmpClust = (LDSecNum-DevInf_FirstDatSec(dev))/DevInf_SecPerClust(dev) + 2;
	FirstSec = DevInf_FirstDatSec(dev)+(TmpClust-2)*DevInf_SecPerClust(dev);
	SecOff   = (LDSecNum-DevInf_FirstDatSec(dev))%DevInf_SecPerClust(dev);
	
	if (_hai_FatDevRead(dev, FirstSec+SecOff, SecBuf))
		return FATERR_DEV_OPERATE_ERR;
	pSecBuf = SecBuf+LDentNum*FAT_DIRENT_SIZE;
	bSecBuf = SecBuf+DentNum*FAT_DIRENT_SIZE;

	while (1)
	{
		if (pSecBuf == SecBuf+DevInf_BytsPerSec(dev))
		{
			if (_hai_FatDevWrite(dev, FirstSec+SecOff, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			if (SecOff+1 == DevInf_SecPerClust(dev))//clust end
			{
				TmpClust = hai_GetNextCluster(dev, TmpClust);
				FirstSec = DevInf_FirstDatSec(dev)+(TmpClust-2)*DevInf_SecPerClust(dev);
				SecOff   = 0;
			}
			else
				SecOff++;
			if (_hai_FatDevRead(dev, FirstSec+SecOff, SecBuf))
				return FATERR_DEV_OPERATE_ERR;
			pSecBuf = SecBuf;
		}
		*pDIR_Name(pSecBuf) = DELETED_DIR_FLAG;
		if (FirstSec+SecOff==DSecNum && pSecBuf==bSecBuf)
		{
			hai_memcpy(DentBuf, pSecBuf, FAT_DIRENT_SIZE);
			break;
		}
		pSecBuf  += FAT_DIRENT_SIZE;
	}
	if (_hai_FatDevWrite(dev, FirstSec+SecOff, SecBuf))
		return FATERR_DEV_OPERATE_ERR;
	
	return FATERR_NO_ERROR;
}


