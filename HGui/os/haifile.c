//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haifile.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 3.0.0		2008-02-06  create                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#include "haifile.h"

#ifdef __HAVE_STDIO__

SH_FILE hai_fopen(const S_BYTE *filename, const S_BYTE *mode)
{
	FILE *pfile = fopen((char *)filename, (char *)mode);

	return (SH_FILE)pfile;
}

S_INT hai_fclose(SH_FILE hFile)
{
	return fclose((FILE *)hFile);
}

S_UINT hai_fread(S_VOID *buffer, S_UINT size, S_UINT count, SH_FILE hFile)
{
	return fread(buffer, size, count, (FILE *)hFile);
}

S_UINT hai_fwrite(const S_VOID *buffer, S_UINT size, S_UINT count, SH_FILE hFile)
{
	return fwrite(buffer, size, count, (FILE *)hFile);
}

S_INT hai_fseek(SH_FILE hFile, S_LONG offset, S_INT origin)
{
	return fseek((FILE *)hFile, offset, origin);
}

S_INT hai_feof(SH_FILE hFile)
{
	return feof((FILE *)hFile);
}

S_LONG hai_ftell(SH_FILE hFile)
{
	return ftell((FILE *)hFile);
}


#else

#error still define stdio!!!

#endif


