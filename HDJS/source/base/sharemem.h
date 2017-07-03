//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			sharemem.h
// Description:		share memory operate
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-16  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __SHOREMEM_H__
#define __SHOREMEM_H__
#include "haidef.h"

//
//memory sign
//
class C_MEMSIGN
{
public:
	C_MEMSIGN(const char *pstr = NULL);
	C_MEMSIGN(C_MEMSIGN &r);
	~C_MEMSIGN();
	bool operator==(C_MEMSIGN &r);

public:
	char m_version[20];
};

//
//share memory
//
#ifdef WIN32
#include <windows.h>
#endif

class C_SHAREMEM
{
public:
	C_SHAREMEM(void);
	C_SHAREMEM(const char *pFile, long size);
	~C_SHAREMEM();

	bool Create(const char *pFile, long size);
	bool Destroy(void);
	bool Save(void *pstart, long size);
	bool Save(void);

	void *GetShareMem(void);

private:
	bool EnsureFile(const char *pFile, long size);

private:
	long  m_size;
	void *m_pmem;
#ifdef WIN32
	HANDLE m_hFile;
	HANDLE m_hFileMap;
#endif
};

inline void *C_SHAREMEM::GetShareMem(void)
{
	return m_pmem;
}

#endif //__SHOREMEM_H__


