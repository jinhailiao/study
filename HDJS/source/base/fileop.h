//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			fileop.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-15  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __FILEOP_H__
#define __FILEOP_H__

#include <stdio.h>

class C_FILE
{
public:
	C_FILE(const char *ppath = NULL, const char *mode = "wb");
	~C_FILE();

public:
	static int MakeDir(const char *ppath);
	static FILE *OpenFile(const char *ppth, const char *mode, bool IsMakeDir);
	static int CloseFile(FILE *pfp);

public:

private:
	FILE *m_fp;
};

#endif //__FILEOP_H__


