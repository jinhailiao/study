//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HFAT16/32
// File:			hfileapi.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2008-06-01  create                  Kingsea
//---------------------------------------------------------------------------------
#include "hfstype.h"
#include "hfileapi.h"
#include "hfilesys.h"


//system functions
//
int hai_InitFileSys(void)
{
	int err;

	err = _hai_InitFileSys();
	
	return err;
}

int hai_format(char *disk, char *fat)
{
	S_BYTE Disk = (((S_BYTE)*disk++)&~0x20);//upper

	if (*disk==':'&& Disk>='C' && Disk <= 'Z')
		return _hai_format(Disk, fat);

	return FATERR_INVALID_DISK;
}

//dir functions
//
int hai_chdir(const char *dirname)
{
	return _hai_chdir(dirname);
}

char *hai_getcwd(char *buffer, int maxlen)
{
	//TODO:
	return _hai_getcwd(buffer, maxlen);
}

int hai_mkdir(const char *dirname)
{
	return _hai_mkdir(dirname);
}

int hai_rmdir(const char *dirname)
{
	return _hai_rmdir(dirname);
}

//file functions
//
HFILE hai_fopen(const char *filename, const char *mode)
{
	return _hai_fopen(filename, mode);
}

int hai_fclose(HFILE hfile)
{
	return _hai_fclose(hfile);
}

int hai_fcloseall(void)
{
	return _hai_fcloseall();
}

int hai_fgetc(HFILE hfile)
{
	char buffer[2];
	
	if (_hai_fread(buffer, 1, 1, hfile) == 1)
		return (int)buffer[0];
	return FATERR_END_OF_FILE;
}

int hai_fputc(int c, HFILE hfile)
{
	char buffer = (char)c;

	if (_hai_fwrite(&buffer, 1, 1, hfile) == 1)
		return (int)buffer;
	return FATERR_END_OF_FILE;
}

size_t hai_fwrite(const void *buffer, size_t size, size_t count, HFILE hfile)
{
	return _hai_fwrite(buffer, size, count, hfile);
}

size_t hai_fread(void *buffer, size_t size, size_t count, HFILE hfile)
{
	return _hai_fread(buffer, size, count, hfile);
}

int  hai_remove(const char *filename)
{
	return _hai_remove(filename);
}

int  hai_rename(const char *oldname, const char *newname)
{
	return _hai_rename(oldname, newname);
}

int  hai_fseek(HFILE hfile, long offset, int origin)
{
	return _hai_fseek(hfile, offset, origin);
}

long hai_ftell(HFILE hfile)
{
	return _hai_ftell(hfile);
}

void hai_clearerr(HFILE hfile)
{
	_hai_clearerr(hfile);
}

int  hai_ferror(HFILE hfile)
{
	return _hai_ferror(hfile);
}

int  hai_feof(HFILE hfile)
{
	return _hai_feof(hfile);
}


//find functions
//
long hai_findfirst(char *filespec, struct finddata *fileinfo)
{
	return _hai_findfirst(filespec, fileinfo);
}

int hai_findnext(long handle, struct finddata *fileinfo)
{
	return _hai_findnext(handle, fileinfo);
}

int hai_findclose(long handle)
{
	return _hai_findclose(handle);
}

//flush functions
//
int hai_fflush(HFILE hfile)
{
	return _hai_fflush(hfile);
}

int hai_flushall(void)
{
	return _hai_flushall();
}


