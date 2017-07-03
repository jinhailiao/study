//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfatfile.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-26  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __HFATFILE_H__
#define __HFATFILE_H__
#include "hfstype.h"
#include "hfatutil.h"

struct FILECONST
{
	S_BYTE *mode;
	S_WORD val;
};

typedef struct tagFILEBUF
{
	S_BYTE flush;
	S_BYTE SecNum;
	S_WORD reserved;
	S_DWORD Clust;
	S_BYTE  SecBuf[FAT_SEC_BUF_MAX];
}
S_FILEBUF, *SP_FILEBUF;

typedef struct tagFATFILEINF
{
	int dev;
	int OpenCnt;
	S_BYTE DirName[11];
	S_BYTE CrtTimeTenth;
	S_WORD CrtTime;
	S_WORD CrtDate;
	S_WORD LastAccDate;
	S_WORD WrtTime;
	S_WORD WrtDate;
	S_WORD attrib;
	S_WORD  DentNum;
	S_DWORD DentSec;
	S_DWORD FirstClust;
	S_DWORD LastClust;
	S_DWORD WorkClu2FstClu;
	S_DWORD CurWorkClust;
	S_DWORD FileSize;
	SP_FILEBUF pSecbuf;
}
S_FATFILEINF, *SP_FATFILEINF;

typedef struct tagFATFILEOP
{
	int dev;
	S_BYTE err;
	S_WORD mode;
	S_DWORD curoffset;
	SP_FATFILEINF pfileinf;
}
S_FATFILEOP, *SH_FATFILEOP;



#define _H_O_APPEND		0x0001
#define _H_O_CREAT		0x0002
#define _H_O_EXCL		0x0004
#define _H_O_RDONLY		0x0008
#define _H_O_TRUNC		0x0010
#define _H_O_WRONLY		0x0020
#define _H_O_TEXT		0x0040
#define _H_O_BINARY		0x0080
#define _H_O_RDWR		_H_O_WRONLY|_H_O_RDONLY


int _hai_CheckFileOpenOnFat(int dev, void *fileop, S_DWORD DentSec, S_WORD DentNum, S_BYTE *DentBuf);
int _hai_FlushFileBuf(int dev, SP_FILEBUF pFileBuf);
int _hai_fcloseOnFat(S_BYTE disk, SH_FATFILEOP hFileOp);
int _hai_fseekOnFat(S_BYTE disk, SH_FATFILEOP hFileOp, long offset, int origin);
int _hai_ferrorOnFat(S_BYTE disk, SH_FATFILEOP hFileOp);
int _hai_feofOnFat(S_BYTE disk, SH_FATFILEOP hFileOp);
int _hai_rmfileOnFat(S_BYTE disk, const S_BYTE *filename);
long _hai_ftellOnFat(S_BYTE disk, SH_FATFILEOP hFileOp);
void _hai_clearerrOnFat(S_BYTE disk, SH_FATFILEOP hFileOp);
size_t _hai_fwriteOnFat(S_BYTE disk, const void *buffer, size_t size, size_t count, SH_FATFILEOP hFileOp);
size_t _hai_freadOnFat(S_BYTE disk, void *buffer, size_t size, size_t count, SH_FATFILEOP hFileOp);
SH_FATFILEOP _hai_fopenOnFat(S_BYTE disk, const S_BYTE *file, const char *mode);
SP_FATFILEINF _hai_OpenFileOnFat(int dev, const S_BYTE *file, S_WORD mode);



#endif //__HFATFILE_H__

