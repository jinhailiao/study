//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			fileop.cpp
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-15  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "haidef.h"
#include "fileop.h"

#ifdef WIN32
#include <io.h>
#include <direct.h>

#define access	_access
#define mkdir(path, mode)	_mkdir(path)
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <errno.h>
#endif

C_FILE::C_FILE(const char *ppath, const char *mode)
{
	m_fp = OpenFile(ppath, mode, true);
}

C_FILE::~C_FILE()
{
	CloseFile(m_fp);
}

int C_FILE::MakeDir(const char *ppath)
{
	char ch, PathBuf[260];

	strcpy(PathBuf, ppath);

	for(int i = 1; (ch=PathBuf[i])!='\0'; i++)//Ìø¹ýpath[0]×Ö·ûµÄÅÐ¶¨
	{
		if (ch=='\\' || ch=='/')
		{
			PathBuf[i] = '\0';
			if (access(PathBuf, 0) != 0)
			{
				if (mkdir(PathBuf,00777) != 0)
					return -1;
			}
			PathBuf[i] = ch;
		}
	}
	return 0;
}

FILE *C_FILE::OpenFile(const char *ppth, const char *mode, bool IsMakeDir)
{
	if (ppth == NULL || *ppth == '\0')
		return NULL;

	FILE* fp = fopen(ppth, mode);
	if (fp)
	{
		int fd = fileno(fp);
#ifndef WIN32
		flock(fd,LOCK_EX);
		fcntl(fd,O_FSYNC);
		if (fd > 20)
			printf("fd for file %s is %d\n",ppth, fd);
#endif
	}
	else if(errno == ENOENT && IsMakeDir == true)
	{
		if (MakeDir(ppth) != 0)
			printf("MakeDir for %s failed\n",ppth);
		else
            fp = OpenFile(ppth, mode, false);
	}

	return fp;
}

int C_FILE::CloseFile(FILE *pfp)
{
#ifndef WIN32
	if(pfp)
	{
		int fd = fileno(pfp);
		fsync(fd);
		flock(fd,LOCK_UN);
	}
#endif
	return fclose(pfp);
}

