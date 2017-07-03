//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			Htrace
// File:			htrace.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-05-23  Create                  Kingsea
// 1.0.1		2009-08-20  Add debug level         Kingsea
//                          Add operator()
//---------------------------------------------------------------------------------
#ifndef __HAITRACE_H__
#define __HAITRACE_H__

/** 错误信息级别定义：
 * 1.error:严重错误，可能导致程序不能正常运行。
 * 2.warning:与正常运行结果相反的，但不影响程序正常运行。
 * 3.information:比较重要的程序运行信息。
 * 4.verbose:一般的程序运行信息
 * 5.debug:程序调试信息。
 */

#include <string>
using std::string;

//以下请根据具体情况修改

#include "haicfg.h"
#define TRACE_LOG_BASEPATH		CFG_FILE_BASE_PATH"data/log/"
#define TRACE_FILE_SIZE_MAX		1000000
#define TRACE_FILE_TOTAL_MAX	20
#define TRACE_FILE_SIZE_MIN		20000
#define TRACE_FILE_TOTAL_MIN	2/*最少2个*/


//以下内联函数请根据需要修改
inline char *GetLogfileName(void)
{//必须ASCII码形式的名字
	extern char *GetTraceTitle(void);
	char *pStr = GetTraceTitle();

	if (pStr) 
		return pStr;
	return "hdjs";
}
inline char *GetLogfileSuffix(void)
{
	return ".log";
}
inline char *GetLogPrivateDir(void)
{
	char *pStr;
	extern char *GetTraceTitle(void);
	static char PrivateDir[12] = {0};

	if (PrivateDir[0])
		return PrivateDir;
	else if ((pStr = GetTraceTitle()) != NULL) 
	{
		strncpy(PrivateDir, pStr, sizeof(PrivateDir)-4);
		strcat(PrivateDir, "/");
		return PrivateDir;
	}
	return "hdjs/";
}
#include "timeop.h"
inline char *GetBackupLogName(void)
{//格式必须为:YYYYMMDDHHmm
	static char BakLogName[14];
	memset(BakLogName, 0x00, sizeof(BakLogName));
	C_TIME::GetTimeString(BakLogName, sizeof(BakLogName));
	return BakLogName;
}
inline void GetTimeStamp(string &tmstr)
{
	char tm[14];

	C_TIME::GetTimeLogFormat(tm, sizeof(tm));
	tmstr.append(tm, sizeof(tm));
}

#include "fileop.h"
inline int MakeLogDir(const char *path)
{
	return C_FILE::MakeDir(path);
}

struct S_TRACESET
{
public:
	enum{L_DISABLED, L_ERROR, L_WARNING, L_INFO, L_VERBOSE, L_DEBUG, L_MAXLEVEL};
	enum{WAY_NONE, WAY_STDOUT, WAY_LOGFILE, WAY_TTYP0, WAY_TTYP1, WAY_TTYP2, WAY_TTYP3, WAY_TTYP4, WAY_TTYP5, WAY_TTYP6, WAY_MAX};

	S_TRACESET(unsigned char *ptitle = NULL, int level=L_INFO, int way=WAY_STDOUT);
	S_TRACESET(const S_TRACESET &rTraceSet);
	~S_TRACESET();

	S_TRACESET & operator=(const S_TRACESET &rTraceSet);

	unsigned char GetLevel(void);
	unsigned char GetWay(void);
	unsigned int  GetFileSize(void);
	unsigned char GetFileTotal(void);
	bool SetLevel(int level);
	bool SetWay(int way);
	bool SetFileSize(unsigned int FileSiz);
	bool SetFileTotal(unsigned char FileTtl);
	unsigned char *GetTitle(void);
	bool SetTitle(const unsigned char *ptitle);

public:
	unsigned char m_level;
	unsigned char m_way;
	unsigned char m_FileTtl;
	unsigned char m_Reserved;
	unsigned int  m_FileSiz;
	unsigned char m_title[12];
};

inline unsigned char S_TRACESET::GetLevel(void)
{
	return m_level;
}

inline unsigned char S_TRACESET::GetWay(void)
{
	return m_way;
}

inline unsigned char * S_TRACESET::GetTitle(void)
{
	if (m_title[0] == '\0')
		return NULL;
	return m_title;
}

inline unsigned int  S_TRACESET::GetFileSize(void)
{
	return m_FileSiz;
}

inline unsigned char S_TRACESET::GetFileTotal(void)
{
	return m_FileTtl;
}

inline bool S_TRACESET::SetTitle(const unsigned char *ptitle)
{
	if (ptitle == NULL)
		memset(m_title, 0x00, sizeof(m_title));
	else
		strncpy((char *)m_title, (const char *)ptitle, 10);
	return true;
}



#define 	TRACEOP_DEC  0x00000000UL
#define		TRACEOP_HEX  0x00000100UL
#define		TRACEOP_hex  0x00000200UL
#define		TRACEOP_OCT  0x00000300UL
#define		TRACEOP_BIN  0x00000400UL
#define		TRACEOP_JZMSK  0x00000700UL

class C_HTRACE;
typedef C_HTRACE & TRACEOP(C_HTRACE & v);

class C_HTRACE
{
public:
	C_HTRACE(unsigned char *ptitle = NULL, int level = S_TRACESET::L_INFO, int way = S_TRACESET::WAY_STDOUT);
	~C_HTRACE();

public:
	static void SetSystemTraceSetting(S_TRACESET *psysset);
	static S_TRACESET* GetSystemTraceSetting(void);

public:
	C_HTRACE & operator<<(TRACEOP *pfnop);
	C_HTRACE & operator<<(const char *pstr);
	C_HTRACE & operator<<(const unsigned char *pstr);
	C_HTRACE & operator<<(char *pstr);
	C_HTRACE & operator<<(unsigned char *pstr);
	C_HTRACE & operator<<(const string &rstr);
	template <class baseT> C_HTRACE & operator<<(baseT val);

	void operator()(const char *pszformat, ...);

public:
	friend C_HTRACE & endl(C_HTRACE & v);
	friend C_HTRACE & time(C_HTRACE & v);
	friend C_HTRACE & title(C_HTRACE & v);
	friend C_HTRACE & tag(C_HTRACE & v);
	friend C_HTRACE & HEX(C_HTRACE & v);
	friend C_HTRACE & hex(C_HTRACE & v);
	friend C_HTRACE & dec(C_HTRACE & v);
	friend C_HTRACE & oct(C_HTRACE & v);
	friend C_HTRACE & bin(C_HTRACE & v);

private:
	void _ToHexString(unsigned char val, string &strDigit, bool upcase);
	void ToHexString(unsigned char val, string &strDigit, bool upcase);
	void ToHexString(char val, string &strDigit, bool upcase);
	void ToHexString(unsigned short val, string &strDigit, bool upcase);
	void ToHexString(short val, string &strDigit, bool upcase);
	void ToHexString(unsigned long val, string &strDigit, bool upcase);
	void ToHexString(long val, string &strDigit, bool upcase);
	void ToHexString(unsigned int val, string &strDigit, bool upcase);
	void ToHexString(int val, string &strDigit, bool upcase);

	void ToDecString(long val, string &strDigit);
	void ToDecString(unsigned long val, string &strDigit);
	void ToDecString(char val, string &strDigit);
	void ToDecString(unsigned char val, string &strDigit);
	void ToDecString(short val, string &strDigit);
	void ToDecString(unsigned short val, string &strDigit);
	void ToDecString(int val, string &strDigit);
	void ToDecString(unsigned int val, string &strDigit);

	void ToOctString(int val, string &strDigit);

	char *GetOutfileName(S_TRACESET &rSet, char *Outfile);
	char *GetBackfileName(S_TRACESET &rSet, char *OutDir);

	bool RenameLogFile(const char *SrcFile, const char *DstFile);
	bool DelLogFile(S_TRACESET &rSet);
	bool DelOneLogFile(S_TRACESET &rSet);

private:
	unsigned long m_operate;
	string m_context;
	S_TRACESET m_curset;
	static S_TRACESET *m_psysset;
};

inline void C_HTRACE::SetSystemTraceSetting(S_TRACESET *psysset)
{
	C_HTRACE::m_psysset = psysset;
}

inline S_TRACESET* C_HTRACE::GetSystemTraceSetting(void)
{
	return C_HTRACE::m_psysset;
}

inline C_HTRACE & C_HTRACE::operator<<(TRACEOP *pfnop)
{
	return pfnop(*this);
}

inline void C_HTRACE::ToDecString(char val, string &strDigit)
{
	ToDecString((long)val, strDigit);
}

inline void C_HTRACE::ToDecString(unsigned char val, string &strDigit)
{
	ToDecString((unsigned long)val, strDigit);
}

inline void C_HTRACE::ToDecString(short val, string &strDigit)
{
	ToDecString((long)val, strDigit);
}

inline void C_HTRACE::ToDecString(unsigned short val, string &strDigit)
{
	ToDecString((unsigned long)val, strDigit);
}

inline void C_HTRACE::ToDecString(int val, string &strDigit)
{
	ToDecString((long)val, strDigit);
}

inline void C_HTRACE::ToDecString(unsigned int val, string &strDigit)
{
	ToDecString((unsigned long)val, strDigit);
}

inline C_HTRACE & C_HTRACE::operator<<(const char *pstr)
{
	m_context.append(pstr);

	return *this;
}

inline C_HTRACE & C_HTRACE::operator<<(const unsigned char *pstr)
{
	return operator<<((const char *)pstr);
}

inline C_HTRACE & C_HTRACE::operator<<(char *pstr)
{
	return operator<<((const char *)pstr);
}

inline C_HTRACE & C_HTRACE::operator<<(unsigned char *pstr)
{
	return operator<<((const char *)pstr);
}

template <class baseT>
C_HTRACE & C_HTRACE::operator<<(baseT val)
{
	unsigned long op = m_operate & TRACEOP_JZMSK;
	string strDigit;

	if (op == TRACEOP_HEX)
		ToHexString(val, strDigit, true);
	else if (op == TRACEOP_hex)
		ToHexString(val, strDigit, false);
	else
		ToDecString(val, strDigit);

	m_context.append(strDigit);

	return *this;
}



extern C_HTRACE hai_logn;
extern C_HTRACE hai_loge;
extern C_HTRACE hai_logw;
extern C_HTRACE hai_logi;
extern C_HTRACE hai_logv;
extern C_HTRACE hai_logd;

#define logn hai_logn
#define loge hai_loge
#define logw hai_logw
#define logi hai_logi
#define logv hai_logv
#define logd hai_logd

#define loget hai_loge<<tag
#define logwt hai_logw<<tag
#define logit hai_logi<<tag
#define logvt hai_logv<<tag
#define logdt hai_logd<<tag

#endif //__HAITRACE_H__
