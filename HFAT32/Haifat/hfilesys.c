//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfilesys.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"
#include "hfilesys.h"
#include "hfatsys.h"
#include "haiclib.h"
#include "hfatutil.h"
#include "hfatstr.h"
#include "hfatdent.h"
#include "hfileapi.h"
#include "hfatfind.h"
#include "hfatfile.h"

S_FILE _gFILE[FS_FOPEN_MAX];

SH_FILE _hai_AllocFileBlock(S_VOID)
{
	int i;
	SH_FILE hfile = S_NULL;

	hai_FileBlockLock();

	for (i = 0; i < FS_FOPEN_MAX; i++)
	{
		if (_gFILE[i].fileinf == S_NULL)
		{
			hfile = _gFILE+i;
			break;
		}
	}
		
	hai_FileBlockUnlock();

	return hfile;
}

S_VOID _hai_FreeFileBlock(SH_FILE hfile)
{
	hai_FileBlockLock();

	if (hfile)
		hfile->fileinf = S_NULL;

	hai_FileBlockUnlock();
}

int _hai_CountFileOpen(S_BYTE disk)
{
	int i, cnt = 0;

	hai_FileBlockLock();

	for (i = 0; i < FS_FOPEN_MAX; i++)
		if (_gFILE[i].disk == disk && _gFILE[i].fileinf != S_NULL)
			cnt ++;

	hai_FileBlockUnlock();

	return cnt;
}

void *_hai_CheckFileIsOpened(int dev, S_DWORD SecNum, S_WORD DentNum, S_BYTE *DentBuf)
{
	int i;
	void *p = S_NULL;

	hai_FileBlockLock();

	for (i = 0; i < FS_FOPEN_MAX; i++)
		if (_gFILE[i].fileinf != S_NULL)
		{
			if (_hai_CheckFileOpenOnFat(dev, _gFILE[i].fileinf, SecNum, DentNum, DentBuf))
			{
				p = _gFILE[i].fileinf;
				break;
			}
		}

	hai_FileBlockUnlock();

	return p;
}

int _hai_SaveCWD(SP_SAVECWD cwdBuf)
{
	cwdBuf->DirClust = _hai_CwdOperate(CWDOPERATE_GETCLUS, (S_BYTE*)0x00);
	_hai_CwdOperate(CWDOPERATE_COPYTO, cwdBuf->cwd);
	return 0;
}

int _hai_RestoreCWD(SP_SAVECWD cwdBuf)
{
	_hai_CwdOperate(CWDOPERATE_SETCLUS, (S_BYTE*)cwdBuf->DirClust);
	_hai_CwdOperate(CWDOPERATE_COPYFROM, cwdBuf->cwd);
	return 0;
}

int _hai_CheckCurrDir(SP_SAVECWD cwdBuf)
{
	S_BYTE cwd[HAI_PATH_MAX+4];

	hai_memset(cwd, 0x00, sizeof(cwd));
	_hai_CwdOperate(CWDOPERATE_COPYTO, cwd);
	
	return hai_stricmp(cwd, cwdBuf->cwd);
}

int _hai_CountCurrDirFile(void)
{
	int err, cnt=0;
	long handle;
	struct finddata fd;

	handle = hai_findfirst("", &fd);
	if (!handle)
		return 0;

	do {
		if (hai_strcmp(fd.name,".")&&hai_strcmp(fd.name, ".."))
			cnt ++;
		err = hai_findnext(handle, &fd);
	}while (!err);
	
	hai_findclose(handle);
	
	return cnt;
}

int _hai_InitFileSys(void)
{
	//TODO:
	_hai_CwdOperate(CWDOPERATE_CLRPATH, S_NULL);
	hai_memset(_gFILE, 0x00, sizeof(_gFILE));
	
	return _hai_InitFatSys();	
}

int _hai_format(char disk, char *fat)
{
	int err = FATERR_UNKNOWN_FILESYS;
	S_BYTE cwd[HAI_PATH_MAX+4];
	S_BYTE FatType;

	_hai_CwdOperate(CWDOPERATE_COPYTO, cwd);//check current dir
	if (cwd[0] == disk && cwd[2])
		return -1;
	
	if (fat == S_NULL)
		FatType = FAT_TYPE_NO;
	else if (hai_memcmp(fat, FatFlag32, 5)==0)
		FatType = FAT_TYPE_32;
	else if (hai_memcmp(fat, FatFlag16, 5)==0)
		FatType = FAT_TYPE_16;
	else
		return -1;//unsupport other filesys
	
	if (_hai_CountFileOpen(disk))
		err = FATERR_DISK_INUSE;
	else
		err = _hai_formatfat(disk, FatType);

	return err;
}

//dir functions
//
int _hai_chdir(const char *dirname)
{
	int err = 0;
	S_BYTE disk, *pSlash;
	S_BYTE cwd[HAI_PATH_MAX+4];
	S_SAVECWD cwdBuf;

	hai_memset(cwd, 0x00, sizeof(cwd));
	_hai_CwdOperate(CWDOPERATE_COPYTO, cwd);
	if (!dirname || *dirname == '\0')
	{
		//TODO:clear other info if support
	}
	else if (!hai_stricmp(cwd, dirname))
	{
		//TODO:as same as current dir
	}
	else
	{
		pSlash = hai_FirstStriStr(dirname, cwd);//cur path contain cwd
		if (pSlash && *pSlash == '\\')
			pSlash++;
		else
			pSlash = (S_BYTE *)dirname;

		if (pSlash[1]==':')
			disk = *pSlash;
		else
			disk = cwd[0];
		_hai_SaveCWD(&cwdBuf);
		if (hai_IsFATDisk(disk))
			err = hai_ChangeDirOnFAT(disk, pSlash);
		else//or other filesys
			err = FATERR_UNKNOWN_FILESYS;
		if (err)
			_hai_RestoreCWD(&cwdBuf);
	}

	return err;
}

char *_hai_getcwd(char *buffer, int maxlen)
{
	int len = _hai_CwdOperate(CWDOPERATE_GETLENG, S_NULL);

	if (maxlen <= len)
		return S_NULL;

	_hai_CwdOperate(CWDOPERATE_COPYTO, buffer);

	return buffer;
}

int _hai_mkdir(const char *dirname)
{
	int err = 0;
	S_BYTE cwd[HAI_PATH_MAX+4];
	S_BYTE filename[HAI_FILENAME_MAX+4];
	S_BYTE *pSlash;
	S_SAVECWD savecwd;

	if (!dirname || *dirname == '\0')
		return -1;

	hai_memset(cwd, 0x00, sizeof(cwd));
	hai_memset(filename, 0x00, sizeof(filename));
	hai_strcpy(cwd, dirname);
	pSlash = hai_FindTailChar(cwd, '\\');
	if (pSlash)
	{
		hai_strcpy(filename, pSlash+1);
		*pSlash = 0;
	}
	else
	{
		hai_strcpy(filename, cwd);
		hai_memset(cwd, 0x00, sizeof(cwd));
	}

	_hai_SaveCWD(&savecwd);
	err = _hai_chdir(cwd);
	if (!err)
	{
		_hai_CwdOperate(CWDOPERATE_COPYTO, cwd);
		if ((hai_CountChar(cwd)+hai_CountChar(filename)+SHORTNAME_NORMAL_LENG) > (HAI_PATH_MAX>>1))
			err = FATERR_PATH_TOO_LENGTH;
		else if (hai_IsFATDisk(*cwd))
			err = _hai_mkdirOnFat(*cwd, filename);//if disk is FAT
		else//or other filesys
			err = FATERR_UNKNOWN_FILESYS;
	}
	_hai_RestoreCWD(&savecwd);

	return err;
}

int _hai_rmdir(const char *dirname)
{
	int err = -1;
	S_BYTE cwd[HAI_PATH_MAX+4];
	S_BYTE filename[HAI_FILENAME_MAX+4];
	S_BYTE *pSlash;
	S_SAVECWD savecwd;

	if (!dirname || *dirname == '\0')
		return -1;

	_hai_SaveCWD(&savecwd);
	if (err = _hai_chdir(dirname))
		goto RMDIR_ERR;
	if (_hai_CheckCurrDir(&savecwd) == 0)//can not remove current dir
	{
		err = FATERR_INVALID_PATH;
		goto RMDIR_ERR;
	}
	if (_hai_CountCurrDirFile())
	{
		err = FATERR_DIR_NOT_EMPTY;
		goto RMDIR_ERR;
	}
	if (err = _hai_chdir(".."))
		goto RMDIR_ERR;

	hai_memset(cwd, 0x00, sizeof(cwd));
	hai_memset(filename, 0x00, sizeof(filename));
	pSlash = hai_FindTailChar(dirname, '\\');
	if (pSlash)
		hai_strcpy(filename, pSlash+1);
	else
		hai_strcpy(filename, dirname);

	_hai_CwdOperate(CWDOPERATE_COPYTO, cwd);
	if (hai_IsFATDisk(*cwd))
		err = _hai_rmdirOnFat(*cwd, filename);//if disk is FAT
	else//or other filesys
		err = FATERR_UNKNOWN_FILESYS;

RMDIR_ERR:
	_hai_RestoreCWD(&savecwd);

	return err;
}


//file functions
//
HFILE _hai_fopen(const char *filename, const char *mode)
{
	SH_FILE hfile = S_NULL;
	S_BYTE cwd[HAI_PATH_MAX+4];
	S_BYTE file[HAI_FILENAME_MAX+4];
	S_BYTE *pSlash;
	S_SAVECWD savecwd;

	if (!filename || *filename == '\0' || !mode || *mode == '\0')
		return S_NULL;
	if ((hfile = _hai_AllocFileBlock()) == S_NULL)
		return S_NULL;

	hai_memset(cwd, 0x00, sizeof(cwd));
	hai_memset(file, 0x00, sizeof(file));
	hai_memset(hfile, 0x00, sizeof(*hfile));
	hai_strcpy(cwd, filename);
	pSlash = hai_FindTailChar(cwd, '\\');
	if (pSlash)
	{
		hai_strcpy(file, pSlash+1);
		*pSlash = '\0';
	}
	else
	{
		hai_strcpy(file, cwd);
		hai_memset(cwd, 0x00, sizeof(cwd));
	}

	_hai_SaveCWD(&savecwd);
	if (!_hai_chdir(cwd))
	{
		_hai_CwdOperate(CWDOPERATE_COPYTO, cwd);
		if (hai_CountChar(cwd)+hai_CountChar(file) < (HAI_PATH_MAX>>1))
		{
			hfile->disk = *cwd;
			if (hai_IsFATDisk(*cwd))
				hfile->fileinf = (S_VOID *)_hai_fopenOnFat(*cwd, file, mode);//if disk is FAT
			else//or other filesys
				hfile->fileinf = S_NULL;
		}
	}

	if (hfile->fileinf == S_NULL)
	{
		_hai_FreeFileBlock(hfile);
		hfile = S_NULL;
	}
	_hai_RestoreCWD(&savecwd);

	return (HFILE)hfile;
}

int _hai_fclose(HFILE hfile)
{
	int err;
	SH_FILE hFile = (SH_FILE)hfile;

	if (!hFile || hFile->fileinf == S_NULL)
		return 0;

	if (hai_IsFATDisk(hFile->disk))
	{
		if ((err = _hai_fcloseOnFat(hFile->disk, (SH_FATFILEOP)hFile->fileinf)) == 0)
			_hai_FreeFileBlock(hFile);
	}
	else//or other filesys
		err = FATERR_UNKNOWN_FILESYS;

	return err;
}

int _hai_fcloseall(void)
{
	int i, err = 0;

	for (i = 0; i < FS_FOPEN_MAX; i++)
	{
		if (_gFILE[i].fileinf != S_NULL)
			if (_hai_fclose((HFILE)(_gFILE+i)))
				err = -1;
	}
		
	return err;
}

size_t _hai_fwrite(const void *buffer, size_t size, size_t count, HFILE hfile)
{
	size_t WriteCount = 0;
	SH_FILE hFile = (SH_FILE)hfile;

	if (!hFile || hFile->fileinf == S_NULL)
		return 0;
	
	if (hai_IsFATDisk(hFile->disk))
		WriteCount = _hai_fwriteOnFat(hFile->disk, buffer, size, count, (SH_FATFILEOP)hFile->fileinf);
	else//or other filesys
		WriteCount = 0;

	return WriteCount;
}

size_t _hai_fread(void *buffer, size_t size, size_t count, HFILE hfile)
{
	size_t ReadCount = 0;
	SH_FILE hFile = (SH_FILE)hfile;

	if (!hFile || hFile->fileinf == S_NULL)
		return 0;

	if (hai_IsFATDisk(hFile->disk))
		ReadCount = _hai_freadOnFat(hFile->disk, buffer, size, count, (SH_FATFILEOP)hFile->fileinf);
	else//or other filesys
		ReadCount = 0;

	return ReadCount;
}

int _hai_remove(const char *pfilename)
{
	int err = -1;
	S_BYTE cwd[HAI_PATH_MAX+4];
	S_BYTE filename[HAI_FILENAME_MAX+4];
	S_BYTE *pSlash;
	S_SAVECWD savecwd;

	if (!pfilename || *pfilename == '\0')
		return -1;

	hai_memset(cwd, 0x00, sizeof(cwd));
	hai_memset(filename, 0x00, sizeof(filename));
	hai_strcpy(cwd, pfilename);
	pSlash = hai_FindTailChar(cwd, '\\');
	if (pSlash)
	{
		hai_strcpy(filename, pSlash+1);
		*pSlash = '\0';
	}
	else
	{
		hai_strcpy(filename, cwd);
		hai_memset(cwd, 0x00, sizeof(cwd));
	}

	_hai_SaveCWD(&savecwd);

	if ((err = _hai_chdir(cwd)) == 0)
	{
		_hai_CwdOperate(CWDOPERATE_COPYTO, cwd);
		if (hai_IsFATDisk(*cwd))
			err = _hai_rmfileOnFat(*cwd, filename);//if disk is FAT
		else//or other filesys
			err = FATERR_UNKNOWN_FILESYS;
	}

	_hai_RestoreCWD(&savecwd);

	return err;
}

int  _hai_rename(const char *oldname, const char *newname)
{
	return -1;
}

int  _hai_fseek(HFILE hfile, long offset, int origin)
{
	int err;
	SH_FILE hFile = (SH_FILE)hfile;

	if (!hFile || hFile->fileinf == S_NULL)
		return -1;

	if (hai_IsFATDisk(hFile->disk))
		err = _hai_fseekOnFat(hFile->disk, (SH_FATFILEOP)hFile->fileinf, offset, origin);
	else//or other filesys
		err = FATERR_UNKNOWN_FILESYS;

	return err;
}

long _hai_ftell(HFILE hfile)
{
	int err;
	SH_FILE hFile = (SH_FILE)hfile;

	if (!hFile || hFile->fileinf == S_NULL)
		return 0;

	if (hai_IsFATDisk(hFile->disk))
		err = _hai_ftellOnFat(hFile->disk, (SH_FATFILEOP)hFile->fileinf);
	else//or other filesys
		err = FATERR_UNKNOWN_FILESYS;

	return err;
}

void _hai_clearerr(HFILE hfile)
{
	SH_FILE hFile = (SH_FILE)hfile;

	if (!hFile || hFile->fileinf == S_NULL)
		return;
	if (hai_IsFATDisk(hFile->disk))
		_hai_clearerrOnFat(hFile->disk, (SH_FATFILEOP)hFile->fileinf);
}

int  _hai_ferror(HFILE hfile)
{
	int err;
	SH_FILE hFile = (SH_FILE)hfile;

	if (!hFile || hFile->fileinf == S_NULL)
		return -1;
	
	if (hai_IsFATDisk(hFile->disk))
		err = _hai_ferrorOnFat(hFile->disk, (SH_FATFILEOP)hFile->fileinf);
	else//or other filesys
		err = FATERR_UNKNOWN_FILESYS;

	return err;
}

int  _hai_feof(HFILE hfile)
{
	int err;
	SH_FILE hFile = (SH_FILE)hfile;

	if (!hFile || hFile->fileinf == S_NULL)
		return -1;

	if (hai_IsFATDisk(hFile->disk))
		err = _hai_feofOnFat(hFile->disk, (SH_FATFILEOP)hFile->fileinf);
	else//or other filesys
		err = FATERR_UNKNOWN_FILESYS;

	return err;
}


//find functions
//
long _hai_findfirst(char *filespec, struct finddata *fileinfo)
{
	S_BYTE cwd[HAI_PATH_MAX+4];
	SP_FINDINFO pfindinf;

	if ((pfindinf = hai_FatMemAlloc(sizeof(*pfindinf)))==S_NULL)
		return 0;
	
	hai_memset(cwd, 0x00, sizeof(cwd));
	_hai_CwdOperate(CWDOPERATE_COPYTO, cwd);
	
	pfindinf->disk = *cwd;
	if (hai_IsFATDisk(*cwd))
		pfindinf->findinf = (void *)_hai_findfirstOnFat(*cwd, filespec, fileinfo);
	else//or other filesys
		pfindinf->findinf = S_NULL;

	if (pfindinf->findinf == S_NULL)
	{
		hai_FatMemFree(pfindinf);
		pfindinf = S_NULL;
	}

	return (long)pfindinf;
}

int _hai_findnext(long handle, struct finddata *fileinfo)
{
	int err;
	SP_FINDINFO pfindinf = (SP_FINDINFO)handle;
	
	if (hai_IsFATDisk(pfindinf->disk))
		err = _hai_findnextOnFat((long)pfindinf->findinf, fileinfo);
	else//or other filesys
		err = FATERR_UNKNOWN_FILESYS;

	return err;
}

int _hai_findclose(long handle)
{
	SP_FINDINFO pfindinf = (SP_FINDINFO)handle;

	if (pfindinf)
	{
		if (pfindinf->findinf)
			hai_FatMemFree(pfindinf->findinf);
		hai_FatMemFree(pfindinf);
	}

	return 0;
}

//flush functions
//
int _hai_fflush(HFILE hfile)
{
	int err;
	SH_FILE hFile = (SH_FILE)hfile;

	if (!hFile || hFile->fileinf == S_NULL)
		return 0;
	
	if (hai_IsFATDisk(hFile->disk))
		err = _hai_fflushOnFat(hFile->disk, (SH_FATFILEOP)hFile->fileinf);
	else//or other filesys
		err = FATERR_UNKNOWN_FILESYS;

	return err;
}

int _hai_flushall(void)
{
	int i, err = 0;

	for (i = 0; i < FS_FOPEN_MAX; i++)
	{
		if (_gFILE[i].fileinf != S_NULL)
			if (_hai_fflush((HFILE)(_gFILE+i)))
				err = -1;
	}
		
	if (hai_flushallOnFat())
		err = -1;
	return err;
}

