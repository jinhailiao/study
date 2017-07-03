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
#ifndef __H_FILESYS_H__
#define __H_FILESYS_H__

#include "hfileapi.h"

#define FS_FOPEN_MAX		20

typedef struct tagSaveCWD
{
	S_DWORD DirClust;
	S_BYTE cwd[HAI_PATH_MAX+4];
}
S_SAVECWD, *SP_SAVECWD;

typedef struct tagFindInfo
{
	S_BYTE disk;
	void *findinf;
}
S_FINEINFO, *SP_FINDINFO;

typedef struct tagHFile
{
	S_BYTE disk;
	S_VOID *fileinf;
}
S_FILE, *SH_FILE;



void *_hai_CheckFileIsOpened(int dev, S_DWORD SecNum, S_WORD DentNum, S_BYTE *DentBuf);


int _hai_SaveCWD(SP_SAVECWD cwdBuf);
int _hai_RestoreCWD(SP_SAVECWD cwdBuf);
int _hai_chdir(const char *dirname);
char *_hai_getcwd(char *buffer, int maxlen);
int _hai_InitFileSys(void);
int _hai_format(char disk, char *fat);
int _hai_mkdir(const char *dirname);
int _hai_rmdir(const char *dirname);

HFILE _hai_fopen(const char *filename, const char *mode);
int _hai_fclose(HFILE hfile);
int _hai_fcloseall(void);

size_t _hai_fwrite(const void *buffer, size_t size, size_t count, HFILE hfile);
size_t _hai_fread(void *buffer, size_t size, size_t count, HFILE hfile);

int  _hai_remove(const char *pfilename);
int  _hai_rename(const char *oldname, const char *newname);
int  _hai_fseek(HFILE hfile, long offset, int origin);
int  _hai_ferror(HFILE hfile);
int  _hai_feof(HFILE hfile);
long _hai_ftell(HFILE hfile);
void _hai_clearerr(HFILE hfile);


long _hai_findfirst(char *filespec, struct finddata *fileinfo);
int _hai_findnext(long handle, struct finddata *fileinfo);
int _hai_findclose(long handle);

int _hai_fflush(HFILE hfile);
int _hai_flushall(void);

#endif //__H_FILESYS_H__
