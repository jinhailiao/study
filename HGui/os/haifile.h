//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haifile.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 2.0.0		2008-02-06  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------
#ifndef  _HAIFILE_H_
#define  _HAIFILE_H_

#include "haitype.h"
#include "haihandle.h"
#ifdef __HAVE_STDIO__
#include "stdio.h"
#endif

#ifndef  SEEK_CUR
#define SEEK_CUR    1
#endif
#ifndef  SEEK_END
#define SEEK_END    2
#endif
#ifndef  SEEK_SET
#define SEEK_SET    0
#endif



SH_FILE hai_fopen(const S_BYTE *filename, const S_BYTE *mode);
S_INT hai_fclose(SH_FILE hFile);
S_UINT hai_fread(S_VOID *buffer, S_UINT size, S_UINT count, SH_FILE hFile);
S_UINT hai_fwrite(const S_VOID *buffer, S_UINT size, S_UINT count, SH_FILE hFile);
S_INT hai_fseek(SH_FILE hFile, S_LONG offset, S_INT origin);
S_INT hai_feof(SH_FILE hFile);
S_LONG hai_ftell(SH_FILE hFile);



#endif


