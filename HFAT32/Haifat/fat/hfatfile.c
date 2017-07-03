//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatfile.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-26  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"
#include "hfileapi.h"
#include "hfilesys.h"
#include "hfatfile.h"
#include "hfatsys.h"
#include "haiclib.h"
#include "hfatstr.h"
#include "hfatdent.h"

const struct FILECONST fileconst[] =
{
	{"a"  , _H_O_WRONLY|_H_O_CREAT|_H_O_APPEND|_H_O_TEXT},
	{"r"  , _H_O_RDONLY|_H_O_TEXT},
	{"w"  , _H_O_WRONLY|_H_O_CREAT|_H_O_TRUNC|_H_O_TEXT},
	{"ab" , _H_O_WRONLY|_H_O_CREAT|_H_O_APPEND|_H_O_BINARY},
	{"rb" , _H_O_RDONLY|_H_O_BINARY},
	{"wb" , _H_O_WRONLY|_H_O_CREAT|_H_O_TRUNC|_H_O_BINARY},
	{"a+" , _H_O_RDWR|_H_O_CREAT|_H_O_APPEND|_H_O_TEXT},
	{"r+" , _H_O_RDWR|_H_O_TEXT},
	{"w+" , _H_O_RDWR|_H_O_CREAT|_H_O_TRUNC|_H_O_TEXT},
	{"a+b", _H_O_RDWR|_H_O_CREAT|_H_O_APPEND|_H_O_BINARY},
	{"r+b", _H_O_RDWR|_H_O_BINARY},
	{"w+b", _H_O_RDWR|_H_O_CREAT|_H_O_TRUNC|_H_O_BINARY},
	{"ab+", _H_O_RDWR|_H_O_CREAT|_H_O_APPEND|_H_O_BINARY},
	{"rb+", _H_O_RDWR|_H_O_BINARY},
	{"wb+", _H_O_RDWR|_H_O_CREAT|_H_O_TRUNC|_H_O_BINARY}
};

void _hai_GetFileInfo(SP_FATFILEINF pfileinf, S_BYTE *DentBuf)
{
	S_WORD ClustHi = HAI_MAKEWORD(pDIR_FstClusHI(DentBuf));
	S_WORD ClustLo = HAI_MAKEWORD(pDIR_FstClusLO(DentBuf));

	hai_memcpy(pfileinf->DirName, DentBuf, sDIR_Name);
	pfileinf->CrtTimeTenth = *pDIR_CrtTimeTenth(DentBuf);
	pfileinf->CrtTime = HAI_MAKEWORD(pDIR_CrtTime(DentBuf));
	pfileinf->CrtDate = HAI_MAKEWORD(pDIR_CrtDate(DentBuf));
	pfileinf->LastAccDate = HAI_MAKEWORD(pDIR_LastAccDate(DentBuf));
	pfileinf->WrtTime = HAI_MAKEWORD(pDIR_WrtTime(DentBuf));
	pfileinf->WrtDate = HAI_MAKEWORD(pDIR_WrtDate(DentBuf));
	pfileinf->FirstClust = HAI_COMBDWORD(ClustHi, ClustLo);
	pfileinf->FileSize = HAI_MAKEDWORD(pDIR_FileSize(DentBuf));
}

int _hai_CheckFileOpenOnFat(int dev, void *fileop, S_DWORD DentSec, S_WORD DentNum, S_BYTE *DentBuf)
{
	SH_FATFILEOP hFileOp = (SH_FATFILEOP)fileop;
	SP_FATFILEINF pfileinf = hFileOp->pfileinf;
	S_WORD ClustHi = HAI_MAKEWORD(pDIR_FstClusHI(DentBuf));
	S_WORD ClustLo = HAI_MAKEWORD(pDIR_FstClusLO(DentBuf));

	if (pfileinf->dev == dev && pfileinf->DentSec == DentSec && pfileinf->DentNum == DentNum
		&& pfileinf->attrib == *pDIR_Attr(DentBuf) && pfileinf->FirstClust == HAI_COMBDWORD(ClustHi, ClustLo))
		return 1;
	return 0;
}

int _hai_FlushFileBuf(int dev, SP_FILEBUF pFileBuf)
{
	int err;
	S_DWORD SecNum;

	if (pFileBuf->flush == 0)
		return 0;
	SecNum = DevInf_FirstDatSec(dev) + (pFileBuf->Clust-2)*DevInf_SecPerClust(dev) + pFileBuf->SecNum;
	if ((err = _hai_FatDevWrite(dev, SecNum, pFileBuf->SecBuf)) == 0)
		pFileBuf->flush = 0;

	return err;
}

int _hai_Flush2ReadFileBuf(int dev, S_DWORD Clust, S_BYTE SecNum, SP_FILEBUF pFileBuf)
{
	int err;
	S_DWORD Secuter;

	if (Clust==pFileBuf->Clust && SecNum==pFileBuf->SecNum)
		return 0;
	if (err = _hai_FlushFileBuf(dev, pFileBuf))
		return err;

	Secuter = DevInf_FirstDatSec(dev) + (Clust-2)*DevInf_SecPerClust(dev) + SecNum;
	if (err = _hai_FatDevRead(dev, Secuter, pFileBuf->SecBuf)) 
	{
		hai_memset(pFileBuf, 0x00, sizeof(*pFileBuf));
	}
	else
	{
		pFileBuf->flush = 0;
		pFileBuf->Clust = Clust;
		pFileBuf->SecNum = SecNum;
	}

	return err;
}


SP_FATFILEINF _hai_OpenFileOnFat(int dev, const S_BYTE *file, S_WORD mode)
{
	int err;
	S_BYTE ShareOpen = 0;
	S_WORD DentNum, LDentNum;
	S_DWORD DSecNum = 0, LDSecNum = 0;
	S_DWORD clust;
	S_BYTE DentBuf[FAT_DIRENT_SIZE];
	SP_FATFILEINF pfileinf;
	SH_FATFILEOP hfileop;

	if ((pfileinf = hai_FatMemAlloc(sizeof(*pfileinf))) == S_NULL)
		return S_NULL;
	hai_memset(pfileinf, 0x00, sizeof(*pfileinf));
	clust = _hai_CwdOperate(CWDOPERATE_GETCLUS, S_NULL);
	err = hai_LocationDent(dev, clust, file, &DSecNum, &DentNum, &LDSecNum, &LDentNum, DentBuf);
	if (err == FATERR_INVALID_FILENAME)
	{
		if (mode&_H_O_CREAT)
		{
			DSecNum = hai_CreateFileDent(dev, clust, file, &DentNum, DentBuf);
			err = hai_LocationDent(dev, clust, file, &DSecNum, &DentNum, &LDSecNum, &LDentNum, DentBuf);
		}
		if (err)
		{
			hai_FatMemFree(pfileinf);
			pfileinf = S_NULL;
		}
	}
	else if (err == FATERR_NO_ERROR)
	{
		if (IS_FILE_ATTR(DentBuf, FILE_ATTR_DIRECTORY))
		{
			hai_FatMemFree(pfileinf);
			pfileinf = S_NULL;
		}
		else if (hfileop = (SH_FATFILEOP)_hai_CheckFileIsOpened(dev, DSecNum, DentNum, DentBuf))
		{
			hai_FatMemFree(pfileinf);
			pfileinf = S_NULL;
			if (!(mode&_H_O_WRONLY))
			{
				pfileinf = hfileop->pfileinf;
				pfileinf->OpenCnt += 1;
				ShareOpen = 1;
			}
		}
		else if ((mode&_H_O_WRONLY)&&(IS_FILE_ATTR(DentBuf, FILE_ATTR_READ_ONLY)))
		{
			hai_FatMemFree(pfileinf);
			pfileinf = S_NULL;
		}
		else if (mode&_H_O_TRUNC)
		{
			if (_hai_TruncateFile(dev, DSecNum, DentNum))
			{
				hai_FatMemFree(pfileinf);
				pfileinf = S_NULL;
			}
			else
			{
				HAI_WRITEWORD(pDIR_FstClusHI(DentBuf), 0x00);
				HAI_WRITEWORD(pDIR_FstClusLO(DentBuf), 0x00);
				HAI_WRITEDWORD(pDIR_FileSize(DentBuf), 0x00);
			}
		}
	}
	else
	{
		hai_FatMemFree(pfileinf);
		pfileinf = S_NULL;
	}

	if(pfileinf && !ShareOpen)
	{
		pfileinf->dev = dev;
		pfileinf->DentNum = DentNum;
		pfileinf->DentSec = DSecNum;
		pfileinf->OpenCnt = 1;
		_hai_GetFileInfo(pfileinf, DentBuf);
		pfileinf->pSecbuf = hai_FatMemAlloc(sizeof(S_FILEBUF));
		if (pfileinf->pSecbuf)
			hai_memset(pfileinf->pSecbuf, 0x00, sizeof(S_FILEBUF));
	}

	return pfileinf;
}

SH_FATFILEOP _hai_fopenOnFat(S_BYTE disk, const S_BYTE *file, const char *mode)
{
	int i;
	SH_FATFILEOP hfile;

	if ((hfile = hai_FatMemAlloc(sizeof(*hfile))) == S_NULL)
		return S_NULL;
	hai_memset(hfile, 0x00, sizeof(*hfile));
	hfile->dev = hai_GetDeviceNum(disk);
	for (i = 0; i < sizeof(fileconst)/sizeof(fileconst[0]); i++)
	{
		if (!hai_strcmp(fileconst[i].mode, mode))
		{
			hfile->mode = fileconst[i].val;
			break;
		}
	}
	if (hfile->mode == 0)
	{
		hai_FatMemFree(hfile);
		return S_NULL;
	}

	hai_DiskOperateLock(hfile->dev);
	hfile->pfileinf = _hai_OpenFileOnFat(hfile->dev, file, hfile->mode);
	hai_DiskOperateUnlock(hfile->dev);
	if (hfile->pfileinf == S_NULL)
	{
		hai_FatMemFree(hfile);
		hfile = S_NULL;
	}

	return hfile;
}

int _hai_fcloseOnFat(S_BYTE disk, SH_FATFILEOP hFileOp)
{
	int err = FATERR_INVALID_DISK;
	S_BYTE *pSecBuf, SecBuf[FAT_SEC_BUF_MAX];
	SP_FATFILEINF pFileInf;

	if (hFileOp->dev != hai_GetDeviceNum(disk) || hFileOp->pfileinf == S_NULL)
		goto FCLOSE_ERR;
	if (!hai_FatDevExist(hFileOp->dev))
		goto FCLOSE_ERR;

	err = FATERR_NO_ERROR;
	pFileInf = hFileOp->pfileinf;
	if (--pFileInf->OpenCnt == 0)
	{
		hai_DiskOperateLock(hFileOp->dev);
		if (err = _hai_FatDevRead(hFileOp->dev, pFileInf->DentSec, SecBuf))
			goto FCLOSE_ERR;
		pSecBuf = SecBuf + pFileInf->DentNum*FAT_DIRENT_SIZE;
		if (hai_memcmp(pSecBuf, pFileInf->DirName, sDIR_Name))
		{
			err = FATERR_FILE_NOFIND;
			goto FCLOSE_ERR;
		}
		hai_UpdateDent(pSecBuf, (S_BYTE)((hFileOp->mode&_H_O_WRONLY)==_H_O_WRONLY));
		HAI_WRITEWORD(pDIR_FstClusHI(pSecBuf), pFileInf->FirstClust>>16);
		HAI_WRITEWORD(pDIR_FstClusLO(pSecBuf), pFileInf->FirstClust);
		HAI_WRITEDWORD(pDIR_FileSize(pSecBuf), pFileInf->FileSize);
		if (err = _hai_FatDevWrite(hFileOp->dev, pFileInf->DentSec, SecBuf))
			goto FCLOSE_ERR;
		if (pFileInf->pSecbuf)
		{
			if (err = _hai_FlushFileBuf(pFileInf->dev, pFileInf->pSecbuf))
				goto FCLOSE_ERR;
			hai_FatMemFree(pFileInf->pSecbuf);
		}
		hai_DiskOperateUnlock(hFileOp->dev);
		hai_FatMemFree(pFileInf);
	}
	hai_FatMemFree(hFileOp);

FCLOSE_ERR:
	hFileOp->err = err;
	return err;
}

S_DWORD _hai_FindFileLastClust(SP_FATFILEINF pFileInf)
{
	int dev = pFileInf->dev;
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];
	S_DWORD Sec, BakSec = 0x00;
	S_DWORD Offset, CurClust, NxtClust;

	if (pFileInf->FirstClust == 0)//still not alloc clust
		return 0;
	CurClust = pFileInf->FirstClust;
	while (DevInf_FatType(dev)==FAT_TYPE_16?IS_FAT16_INUSE_CLUST(CurClust):IS_FAT32_INUSE_CLUST(CurClust))
	{
		if (DevInf_FatType(dev) == FAT_TYPE_32)
		{
			Sec = (CurClust<<2)/DevInf_BytsPerSec(dev) + DevInf_RsvdSec(dev);
			if (Sec != BakSec)
			{
				BakSec = Sec;
				if (_hai_FatDevRead(dev, Sec, SecBuf))
					return 0xFFFFFFFF;
			}
			Offset = (CurClust<<2)%DevInf_BytsPerSec(dev);
			NxtClust = HAI_MAKEDWORD(SecBuf+Offset)&FAT32_CLUST_VALIDBIT_MASK;
			if (IS_FAT32_INUSE_CLUST(NxtClust))
				CurClust = NxtClust;
			else
				break;
		}
		else
		{
			Sec = (CurClust<<1)/DevInf_BytsPerSec(dev) + DevInf_RsvdSec(dev);
			if (Sec != BakSec)
			{
				BakSec = Sec;
				if (_hai_FatDevRead(dev, Sec, SecBuf))
					return 0xFFFFFFFF;
			}
			Offset = (CurClust<<1)%DevInf_BytsPerSec(dev);
			NxtClust = HAI_MAKEWORD(SecBuf+Offset);
			if (IS_FAT16_INUSE_CLUST(NxtClust))
				CurClust = NxtClust;
			else
				break;
		}
	}

	return CurClust;
}

int _hai_AllocDiskSpace4File(SP_FATFILEINF pFileInf, S_DWORD total)
{
	int dev = pFileInf->dev;
	int ClustCnt, NeedClust;
	S_DWORD CurClust, num = DevInf_BytsPerSec(dev)*DevInf_SecPerClust(dev);

	ClustCnt = (pFileInf->FileSize + (num-1))/num;
	NeedClust = (total + (num-1))/num;
	if (NeedClust <= ClustCnt)
		return 0;
	if (pFileInf->LastClust == 0)
		pFileInf->LastClust = _hai_FindFileLastClust(pFileInf);
	if (pFileInf->LastClust == 0xFFFFFFFF)
	{
		pFileInf->LastClust = 0x00;
		return FATERR_DEV_OPERATE_ERR;
	}

	CurClust = pFileInf->LastClust;
	while (ClustCnt++ < NeedClust)
	{
		num = hai_AllocIdleClust(dev);
		if (num == 0)
			return FATERR_NO_DISK_SPACE;
		if (pFileInf->FirstClust == 0x00)
		{
			pFileInf->FirstClust = num;
			CurClust = num;
		}
		else
		{
			if (hai_LinkClust(dev, CurClust, num))
				return FATERR_DEV_OPERATE_ERR;
			CurClust = num;
		}
	}
	pFileInf->LastClust = num;
	
	return 0;
}

S_DWORD hai_FindSpecifyClust(int dev, S_DWORD StartClust, S_DWORD NoClust)
{
	S_BYTE SecBuf[FAT_SEC_BUF_MAX];
	S_DWORD Sec, BakSec = 0x00;
	S_DWORD Offset, NxtClust, ClustCnt = 0;

	if (NoClust < 1)
		return StartClust;
	while ((ClustCnt++<NoClust)&&(DevInf_FatType(dev)==FAT_TYPE_16?IS_FAT16_INUSE_CLUST(StartClust):IS_FAT32_INUSE_CLUST(StartClust)))
	{
		if (DevInf_FatType(dev) == FAT_TYPE_32)
		{
			Sec = (StartClust<<2)/DevInf_BytsPerSec(dev) + DevInf_RsvdSec(dev);
			if (Sec != BakSec)
			{
				BakSec = Sec;
				if (_hai_FatDevRead(dev, Sec, SecBuf))
					return 0xFFFFFFFF;
			}
			Offset = (StartClust<<2)%DevInf_BytsPerSec(dev);
			NxtClust = HAI_MAKEDWORD(SecBuf+Offset)&FAT32_CLUST_VALIDBIT_MASK;
			if (IS_FAT32_INUSE_CLUST(NxtClust))
				StartClust = NxtClust;
			else
				break;
		}
		else
		{
			Sec = (StartClust<<1)/DevInf_BytsPerSec(dev) + DevInf_RsvdSec(dev);
			if (Sec != BakSec)
			{
				BakSec = Sec;
				if (_hai_FatDevRead(dev, Sec, SecBuf))
					return 0xFFFFFFFF;
			}
			Offset = (StartClust<<1)%DevInf_BytsPerSec(dev);
			NxtClust = HAI_MAKEWORD(SecBuf+Offset);
			if (IS_FAT16_INUSE_CLUST(NxtClust))
				StartClust = NxtClust;
			else
				break;
		}
	}

	return StartClust;
}

int _hai_FileWriteOnFat(const void *buffer, size_t size, size_t count, S_DWORD CurOffset, size_t *RealCnt, SP_FATFILEINF pFileInf)
{
	int dev = pFileInf->dev;
	int err = FATERR_NO_ERROR;
	size_t tmpsize, writesize;
	const S_BYTE *pBuf = buffer;
	S_FILEBUF *pFileBuf, FileBuf;
	S_DWORD CurClust, OffInClust, OffInSec, NoClust;
	S_DWORD total, SecNum;

	if (pFileInf->attrib & FILE_ATTR_READ_ONLY)
		return FATERR_READONLY_FILE;

	if (CurOffset == 0xFFFFFFFF)
		CurOffset = pFileInf->FileSize;
	total = size*count+CurOffset;
	if (total > pFileInf->FileSize)
	{
		if (err = _hai_AllocDiskSpace4File(pFileInf, total))
			return err;
	}

	if (pFileInf->pSecbuf)
		pFileBuf = pFileInf->pSecbuf;
	else
	{
		pFileBuf = &FileBuf;
		hai_memset(pFileBuf, 0x00, sizeof(*pFileBuf));
	}

	NoClust = CurOffset/(DevInf_BytsPerSec(dev)*DevInf_SecPerClust(dev));
	if (pFileInf->CurWorkClust == 0)
		pFileInf->CurWorkClust = hai_FindSpecifyClust(dev, pFileInf->FirstClust, NoClust);
	else
	{
		if (NoClust >= pFileInf->WorkClu2FstClu)
			pFileInf->CurWorkClust = hai_FindSpecifyClust(dev, pFileInf->CurWorkClust, NoClust - pFileInf->WorkClu2FstClu);
		else
			pFileInf->CurWorkClust = hai_FindSpecifyClust(dev, pFileInf->FirstClust, NoClust);
	}
	pFileInf->WorkClu2FstClu = NoClust;

	if (pFileInf->CurWorkClust == 0x00)
		return FATERR_DEV_OPERATE_ERR;
	CurClust = pFileInf->CurWorkClust;
	OffInClust = CurOffset%(DevInf_BytsPerSec(dev)*DevInf_SecPerClust(dev));
	SecNum = OffInClust/DevInf_BytsPerSec(dev);
	OffInSec = OffInClust%DevInf_BytsPerSec(dev);
	
	if (err = _hai_Flush2ReadFileBuf(dev, CurClust, (S_BYTE)SecNum, pFileBuf))
		return err;

	for (*RealCnt = 0; *RealCnt < count; (*RealCnt)++)
	{
		for (tmpsize = size; tmpsize; )
		{
			if (tmpsize > DevInf_BytsPerSec(dev)-OffInSec)
				writesize = DevInf_BytsPerSec(dev)-OffInSec;
			else
				writesize = tmpsize;
			tmpsize -= writesize;
			pFileBuf->flush = 1;
			hai_memcpy(pFileBuf->SecBuf+OffInSec, pBuf, writesize);
			pBuf += writesize;
			if (OffInSec+writesize >= DevInf_BytsPerSec(dev))
			{
				if (SecNum+1 >= DevInf_SecPerClust(dev))
				{
					CurClust = hai_GetNextCluster(dev, CurClust);
					if (CurClust == 0xFFFFFFFF)
					{
						if (tmpsize == 0)//just complete
							break;
						err = FATERR_DEVOP_ABORT;
						goto FILEWRITE_ERR;
					}
					SecNum = 0;
				}
				else
					SecNum++;
				if (err = _hai_Flush2ReadFileBuf(dev, CurClust, (S_BYTE)SecNum, pFileBuf))
				{
					err = FATERR_DEVOP_ABORT;
					goto FILEWRITE_ERR;
				}
				OffInSec = 0;
			}
			else
			{
				OffInSec += writesize;
			}
		}
	}

	if (_hai_FlushFileBuf(dev, pFileBuf))
		err = FATERR_DEVOP_ABORT;
FILEWRITE_ERR:
	if (pFileInf->FileSize < CurOffset+*RealCnt*size)
		pFileInf->FileSize = CurOffset+*RealCnt*size;
	
	return err;
}

size_t _hai_fwriteOnFat(S_BYTE disk, const void *buffer, size_t size, size_t count, SH_FATFILEOP hFileOp)
{
	int err = FATERR_INVALID_DISK;
	size_t RealCnt = 0;
	S_DWORD CurOffset;

	if (hFileOp->dev != hai_GetDeviceNum(disk) || hFileOp->pfileinf == S_NULL)
		goto FWRITE_ERR;
	if (!hai_FatDevExist(hFileOp->dev))
		goto FWRITE_ERR;

	if (hFileOp->mode & _H_O_APPEND)
		CurOffset = 0xFFFFFFFF;
	else
		CurOffset = hFileOp->curoffset;

	if (hFileOp->mode & _H_O_WRONLY)
		err = _hai_FileWriteOnFat(buffer, size, count, CurOffset, &RealCnt, hFileOp->pfileinf);
	else
		err = FATERR_READONLY_MODE;

	if (err == FATERR_NO_ERROR || err == FATERR_DEVOP_ABORT)
		hFileOp->curoffset += size*RealCnt;

FWRITE_ERR:
	hFileOp->err = err;
	return RealCnt;
}

int _hai_FileReadOnFat(void *buffer, size_t size, size_t count, S_DWORD CurOffset, size_t *RealCnt, SP_FATFILEINF pFileInf)
{
	int dev = pFileInf->dev;
	int err = FATERR_NO_ERROR;
	size_t tmpsize, readsize;
	S_BYTE *pBuf = buffer;
	S_FILEBUF *pFileBuf, FileBuf;
	S_DWORD CurClust, OffInClust, OffInSec, NoClust;
	S_DWORD SecNum;

	if (pFileInf->pSecbuf)
		pFileBuf = pFileInf->pSecbuf;
	else
	{
		pFileBuf = &FileBuf;
		hai_memset(pFileBuf, 0x00, sizeof(*pFileBuf));
	}

	NoClust = (CurOffset+(DevInf_BytsPerSec(dev)*DevInf_SecPerClust(dev)-1))/(DevInf_BytsPerSec(dev)*DevInf_SecPerClust(dev));
	CurClust = hai_FindSpecifyClust(dev, pFileInf->FirstClust, NoClust);
	if (CurClust == 0x00)
		return FATERR_DEV_OPERATE_ERR;
	OffInClust = CurOffset%(DevInf_BytsPerSec(dev)*DevInf_SecPerClust(dev));
	SecNum = OffInClust/DevInf_BytsPerSec(dev);
	OffInSec = OffInClust%DevInf_BytsPerSec(dev);
	
	if (err = _hai_Flush2ReadFileBuf(dev, CurClust, (S_BYTE)SecNum, pFileBuf))
		return err;

	for (*RealCnt = 0; *RealCnt < count; (*RealCnt)++)
	{
		for (tmpsize = size; tmpsize; )
		{
			if (tmpsize > DevInf_BytsPerSec(dev)-OffInSec)
				readsize = DevInf_BytsPerSec(dev)-OffInSec;
			else
				readsize = tmpsize;
			tmpsize -= readsize;
			hai_memcpy(pBuf, pFileBuf->SecBuf+OffInSec, readsize);
			pBuf += readsize;
			if (OffInSec+readsize >= DevInf_BytsPerSec(dev))
			{
				if (SecNum+1 >= DevInf_SecPerClust(dev))
				{
					CurClust = hai_GetNextCluster(dev, CurClust);
					if (CurClust == 0xFFFFFFFF)
					{
						err = FATERR_DEVOP_ABORT;
						goto FILEREAD_ERR;
					}
					SecNum = 0;
				}
				else
					SecNum++;
				if (err = _hai_Flush2ReadFileBuf(dev, CurClust, (S_BYTE)SecNum, pFileBuf))
				{
					err = FATERR_DEVOP_ABORT;
					goto FILEREAD_ERR;
				}
				OffInSec = 0;
			}
			else
			{
				OffInSec += readsize;
			}
		}
	}

FILEREAD_ERR:
	
	return err;
}


size_t _hai_freadOnFat(S_BYTE disk, void *buffer, size_t size, size_t count, SH_FATFILEOP hFileOp)
{
	int err = FATERR_INVALID_DISK;
	size_t RealCnt = 0;

	if (hFileOp->dev != hai_GetDeviceNum(disk) || hFileOp->pfileinf == S_NULL)
		goto FREAD_ERR;
	if (!hai_FatDevExist(hFileOp->dev))
		goto FREAD_ERR;

	if (hFileOp->mode & _H_O_RDONLY)
	{
		if (hFileOp->pfileinf->FileSize-hFileOp->curoffset < size)
			err = FATERR_END_OF_FILE;
		else
		{
			if (hFileOp->pfileinf->FileSize-hFileOp->curoffset < size*count)
				count = (hFileOp->pfileinf->FileSize-hFileOp->curoffset)/size;
			err = _hai_FileReadOnFat(buffer, size, count, hFileOp->curoffset, &RealCnt, hFileOp->pfileinf);
		}
	}
	else
		err = FATERR_WRITEONLY_MODE;

	if (err == FATERR_NO_ERROR || err == FATERR_DEVOP_ABORT)
		hFileOp->curoffset += size*RealCnt;

FREAD_ERR:
	hFileOp->err = err;
	return RealCnt;
}

int _hai_rmfileOnFat(S_BYTE disk, const S_BYTE *filename)
{
	int err = -1;
	int dev = hai_GetDeviceNum(disk);
	S_BYTE DentBuf[FAT_DIRENT_SIZE];
	S_WORD DentNum, LDentNum;
	S_DWORD DSecNum, LDSecNum, clust;

	if (dev == -1)
		return FATERR_INVALID_DISK;
	if (!*filename||IS_FAT_CUR_DIR(filename)||IS_FAT_BACK_DIR(filename))
		return FATERR_INVALID_FILENAME;

	hai_DiskOperateLock(dev);

	clust = _hai_CwdOperate(CWDOPERATE_GETCLUS, S_NULL);
	err = hai_LocationDent(dev, clust, filename, &DSecNum, &DentNum, &LDSecNum, &LDentNum, DentBuf);
	if (err == FATERR_NO_ERROR)
	{
		if (_hai_CheckFileIsOpened(dev, DSecNum, DentNum, DentBuf))
		{
			err = FATERR_INUSE_FILE;
		}
		else if ((*pDIR_Attr(DentBuf)&(FILE_ATTR_DIRECTORY|FILE_ATTR_VOLUME_ID)) == 0x00)//is a file
		{
			err = hai_RemoveDent(dev, DSecNum, DentNum, LDSecNum, LDentNum, DentBuf);
			if (!err)
			{
				clust = GetDentClust(DentBuf);
				err = hai_UnlinkClust(dev, clust);
			}
		}
		else
		{
			err = FATERR_INVALID_FILENAME;
		}
	}

	hai_DiskOperateUnlock(dev);

	return err;
}

int _hai_fseekOnFat(S_BYTE disk, SH_FATFILEOP hFileOp, long offset, int origin)
{
	int err = FATERR_INVALID_DISK;
	SP_FATFILEINF pFileInf = hFileOp->pfileinf;

	if (hai_GetDeviceNum(disk)==hFileOp->dev && hai_FatDevExist(hFileOp->dev))
	{
		err = FATERR_NO_ERROR;
		if (origin == SEEK_SET)
		{
			if (offset < 0)
				err = FATERR_UNKNOWN_ERR2;
			else
				hFileOp->curoffset = offset;
		}
		else if (origin == SEEK_CUR)
		{
			if (offset<0 && offset+hFileOp->curoffset < 0)
				err = FATERR_UNKNOWN_ERR2;
			else
				hFileOp->curoffset += offset;
		}
		else if (origin == SEEK_END)
		{
			if (offset<0 && offset+pFileInf->FileSize < 0)
				err = FATERR_UNKNOWN_ERR2;
			else
				hFileOp->curoffset = (S_LONG)pFileInf->FileSize+offset;
		}
		else
			err = FATERR_UNKNOWN_ERR2;
	}

	hFileOp->err = err;
	return err;
}

long  _hai_ftellOnFat(S_BYTE disk, SH_FATFILEOP hFileOp)
{
	int err = FATERR_INVALID_DISK;
	long offset = 0;

	if (hai_GetDeviceNum(disk)==hFileOp->dev && hai_FatDevExist(hFileOp->dev))
	{
		offset = hFileOp->curoffset;
		err = FATERR_NO_ERROR;
	}

	hFileOp->err = err;
	return offset;
}

void  _hai_clearerrOnFat(S_BYTE disk, SH_FATFILEOP hFileOp)
{
	if (hai_GetDeviceNum(disk)==hFileOp->dev && hai_FatDevExist(hFileOp->dev))
		hFileOp->err = FATERR_NO_ERROR;
}

int  _hai_ferrorOnFat(S_BYTE disk, SH_FATFILEOP hFileOp)
{
	if (hai_GetDeviceNum(disk)==hFileOp->dev && hai_FatDevExist(hFileOp->dev))
		return hFileOp->err;
	return FATERR_UNKNOWN_ERR2;
}

int  _hai_feofOnFat(S_BYTE disk, SH_FATFILEOP hFileOp)
{
	int err = FATERR_UNKNOWN_ERR2;
	SP_FATFILEINF pFileInf;

	if (hai_GetDeviceNum(disk)==hFileOp->dev && hai_FatDevExist(hFileOp->dev))
	{
		pFileInf = hFileOp->pfileinf;
		if (pFileInf)
		{
			if (hFileOp->curoffset >= pFileInf->FileSize)
				err = FATERR_END_OF_FILE;
			else
				err = FATERR_NO_ERROR;
		}
	}

	return err;
}



