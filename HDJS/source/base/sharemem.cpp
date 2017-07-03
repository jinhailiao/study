//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			sharemem.cpp
// Description:		share memory operate
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-16  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "sharemem.h"
#include "fileop.h"
#include "osrelate.h"

//
//memory sign
//
C_MEMSIGN::C_MEMSIGN(const char *pstr)
{
	memset(m_version, 0x00, sizeof(m_version));
	if (pstr != NULL)
		strncpy(m_version, pstr, sizeof(m_version));
}

C_MEMSIGN::~C_MEMSIGN()
{
}

bool C_MEMSIGN::operator==(C_MEMSIGN &r)
{
	if (memcmp(m_version, r.m_version, sizeof(m_version)) == 0)
		return true;
	return false;
}

//
//share memory
//
C_SHAREMEM::C_SHAREMEM(void)
{
	m_size = 0;
	m_pmem = NULL;
#ifdef WIN32
	m_hFile = INVALID_HANDLE_VALUE;
	m_hFileMap = INVALID_HANDLE_VALUE;
#endif
}

C_SHAREMEM::C_SHAREMEM(const char *pFile, long size)
{
	Create(pFile, size);
}

C_SHAREMEM::~C_SHAREMEM()
{
	Destroy();
}

bool C_SHAREMEM::Create(const char *pFile, long size)
{
	if (EnsureFile(pFile, size) == false)
		return false;
#ifdef WIN32
	m_hFile = CreateFile(pFile,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return false;
	m_hFileMap=CreateFileMapping(m_hFile,NULL,PAGE_READWRITE,0,size,NULL);
	if (m_hFileMap == NULL)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		return false;
	}
	m_pmem = MapViewOfFile(m_hFileMap,FILE_MAP_WRITE,0,0,0);
	if (m_pmem == NULL)
	{
		CloseHandle(m_hFileMap);
		CloseHandle(m_hFile);
		m_hFileMap = INVALID_HANDLE_VALUE;
		m_hFile = INVALID_HANDLE_VALUE;
		return false;
	}
#else
	int fd;
	if ((fd = open(pFile,O_RDWR,00777)) == -1)
		return false;
	m_pmem = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	close(fd);
	if (m_pmem == (void*)-1)
	{
		m_pmem = NULL;
		return false;
	}
#endif
	m_size = size;
	return true;
}

bool C_SHAREMEM::Destroy(void)
{
	m_size = 0;
#ifdef WIN32
	if (m_pmem) UnmapViewOfFile(m_pmem);
	if (m_hFileMap) CloseHandle(m_hFileMap);
	if (m_hFile) CloseHandle(m_hFile);
	m_hFileMap = INVALID_HANDLE_VALUE;
	m_hFile = INVALID_HANDLE_VALUE;
#else
	if (m_pmem) munmap(m_pmem, m_size);
#endif
	m_pmem = NULL;

	return true;
}

bool C_SHAREMEM::Save(void *pstart, long size)
{
#ifdef WIN32
	return (bool)FlushViewOfFile(pstart, size);
#else
	if (msync(pstart, size, MS_SYNC|MS_INVALIDATE) == 0)
		return true;
	return false;
#endif
}

bool C_SHAREMEM::Save(void)
{
	return Save(m_pmem, m_size);
}

bool C_SHAREMEM::EnsureFile(const char *pFile, long size)
{
	FILE *fp = fopen(pFile, "rb");
	if (fp != NULL)
	{
		fseek(fp, 0x00, SEEK_END);
		long len = ftell(fp);
		fclose(fp);
		if (len == size)
			return true;
	}

	C_FILE::MakeDir(pFile);
	if ((fp=fopen(pFile, "wb")) == NULL)
		return false;

	long WriteBytes;
	char buffer[512];
	memset(buffer, 0xFF, sizeof(buffer));

	while (size)
	{
		WriteBytes = (size<(long)sizeof(buffer))? size:sizeof(buffer);
		fwrite(buffer, WriteBytes, 1, fp);
		size -= WriteBytes;
	}

	fclose(fp);

	return true;
}


