//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			Haitrace
// File:			haitrace.cpp
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-05-23  Create                  Kingsea
// 1.0.1		2009-08-20  Add debug level         Kingsea
//                          Add operator()
//---------------------------------------------------------------------------------
#include "htrace.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdarg.h>

using std::cout;
using std::ofstream;
using std::ios;


S_TRACESET *C_HTRACE::m_psysset = NULL;

char *GetTraceTitle(void)
{
	S_TRACESET *pset = C_HTRACE::GetSystemTraceSetting();
	if (pset) return (char *)pset->GetTitle();
	return NULL;
}

S_TRACESET::S_TRACESET(unsigned char *ptitle, int level, int way)
:m_level(level),m_way(way)
{
	m_FileSiz  = TRACE_FILE_SIZE_MAX;
	m_FileTtl  = TRACE_FILE_TOTAL_MAX;
	if (ptitle == NULL)
		memset(m_title, 0x00, sizeof(m_title));
	else
		strncpy((char *)m_title, (const char *)ptitle, 10);
	m_Reserved = 0;
}

S_TRACESET::S_TRACESET(const S_TRACESET &rTraceSet)
:m_level(rTraceSet.m_level), m_way(rTraceSet.m_way)
{
	m_FileSiz  = rTraceSet.m_FileSiz;
	m_FileTtl  = rTraceSet.m_FileTtl;
	strncpy((char *)m_title, (const char *)rTraceSet.m_title, 10);
	m_Reserved = rTraceSet.m_Reserved;
}

S_TRACESET::~S_TRACESET()
{
}

bool S_TRACESET::SetLevel(int level)
{
	if (level>=L_DISABLED && level<L_MAXLEVEL)
	{
		m_level = (unsigned char)level;
		return true;
	}
	return false;
}

bool S_TRACESET::SetWay(int way)
{
	if (way >= WAY_NONE && way < WAY_MAX)
	{
		m_way = (unsigned char)way;
		return true;
	}
	return false;
}

bool S_TRACESET::SetFileSize(unsigned int FileSiz)
{
	if (FileSiz >= TRACE_FILE_SIZE_MIN && FileSiz <= TRACE_FILE_SIZE_MAX)
	{
		m_FileSiz = FileSiz;
		return true;
	}
	return false;
}

bool S_TRACESET::SetFileTotal(unsigned char FileTtl)
{
	if (FileTtl >= TRACE_FILE_TOTAL_MIN && FileTtl <= TRACE_FILE_TOTAL_MAX)
	{
		m_FileTtl = FileTtl;
		return true;
	}
	return false;
}

S_TRACESET & S_TRACESET::operator=(const S_TRACESET &rTraceSet)
{
	if (this != &rTraceSet)
	{
		strncpy((char *)m_title, (const char *)rTraceSet.m_title, 10);
		m_level = rTraceSet.m_level;
		m_way = rTraceSet.m_way;
	}

	return *this;
}


// C_TRACE function begin
//
static const char *sTraceType[] =
{
	"", "[E]", "[W]", "[I]", "[V]", "[D]"
};

C_HTRACE::C_HTRACE(unsigned char *ptitle, int level, int way):m_curset(ptitle, level, way)
{
//	assert(level > S_TRACESET::L_DISABLED);
	m_operate = 0;
	m_context = sTraceType[m_curset.GetLevel()];
}

C_HTRACE::~C_HTRACE()
{
}

C_HTRACE & C_HTRACE::operator<<(const string &rstr)
{
	unsigned long op = m_operate & TRACEOP_JZMSK;

	if (op == TRACEOP_HEX || op == TRACEOP_hex)//十六进制输出串
	{
		for (string::const_iterator iter = rstr.begin(); iter < rstr.end(); iter++)
		{
			_ToHexString(*iter, m_context, (op == TRACEOP_HEX));
			m_context.push_back(' ');
		}
	}
	else
	{
		m_context.append(rstr);//字符形式输出串
	}

	return *this;
}

void C_HTRACE::operator()(const char *pszformat, ...)
{
	char buffer[2048];
	va_list arg;

	memset(buffer, 0x00, sizeof(buffer));
	va_start(arg, pszformat);
	vsprintf(buffer, pszformat, arg);
	va_end(arg);

	(*this)<<tag<<buffer<<endl;
}

C_HTRACE & endl(C_HTRACE & v)
{
	S_TRACESET *pset;

	if (C_HTRACE::m_psysset && (v.m_curset.GetLevel() > C_HTRACE::m_psysset->GetLevel()))
		goto end; //  当前跟踪级别高于系统设置的级别，退出

	if (C_HTRACE::m_psysset)
		pset = C_HTRACE::m_psysset;
	else
		pset = &v.m_curset;

	v.m_context += '\n';

	if (pset->GetWay() == S_TRACESET::WAY_NONE)
		;
	else if (pset->GetWay() == S_TRACESET::WAY_STDOUT)
		cout<<v.m_context;
	else
	{
		char filename[260];
		memset(filename, 0x00, sizeof(filename));
		v.GetOutfileName(*pset, filename);

		if (pset->GetWay() == S_TRACESET::WAY_LOGFILE)
		{
			ofstream outfile(filename, ios::app);
			if (!outfile)//handle dir
			{
				MakeLogDir(filename);
				outfile.open(filename, ios::app);
				if (!outfile)
					goto end;
			}
			outfile<<v.m_context;//write log file
			outfile.seekp(0x00, std::ios::end);//handle logfile size and total
			if (outfile.tellp() >= (int)pset->GetFileSize())
			{
				char backfile[260];
				memset(backfile, 0x00, sizeof(backfile));
				v.GetBackfileName(*pset, backfile);

				outfile.close();
				if (v.RenameLogFile(filename, backfile) == true)
					v.DelLogFile(*pset);
			}
		}
		else//for linux tty
		{
			ofstream outfile(filename, ios::out);
			if (!outfile)
				goto end;
			outfile<<v.m_context;//write
		}
	}

end:
	v.m_operate = 0;
	v.m_context = sTraceType[v.m_curset.GetLevel()];

	return v;
}

C_HTRACE & time(C_HTRACE & v)
{
	string timestr;

	GetTimeStamp(timestr);

	v.m_context.append("(");
	v.m_context.append(timestr);
	v.m_context.append(") ");

	return v;
}

C_HTRACE & title(C_HTRACE & v)
{
	if (C_HTRACE::m_psysset && C_HTRACE::m_psysset->GetTitle())
	{
		v.m_context.append((char *)C_HTRACE::m_psysset->GetTitle());
		v.m_context.append(": ");
	}
	else if (v.m_curset.GetTitle())
	{
        v.m_context.append((char *)v.m_curset.GetTitle());
		v.m_context.append(": ");
	}

	return v;
}

C_HTRACE & tag(C_HTRACE & v)
{
	if (C_HTRACE::m_psysset && C_HTRACE::m_psysset->GetTitle())
	{
		v.m_context.append((char *)C_HTRACE::m_psysset->GetTitle());
	}
	else if (v.m_curset.GetTitle())
	{
		v.m_context.append((char *)v.m_curset.GetTitle());
	}

	return time(v);
}

C_HTRACE & HEX(C_HTRACE & v)
{
	v.m_operate &= ~TRACEOP_JZMSK;
	v.m_operate |= TRACEOP_HEX;

	return v;
}

C_HTRACE & hex(C_HTRACE & v)
{
	v.m_operate &= ~TRACEOP_JZMSK;
	v.m_operate |= TRACEOP_hex;

	return v;
}

C_HTRACE & dec(C_HTRACE & v)
{
	v.m_operate &= ~TRACEOP_JZMSK;
	v.m_operate |= TRACEOP_DEC;

	return v;
}

C_HTRACE & oct(C_HTRACE & v)
{
	v.m_operate &= ~TRACEOP_JZMSK;
	v.m_operate |= TRACEOP_OCT;

	return v;
}

C_HTRACE & bin(C_HTRACE & v)
{
	v.m_operate &= ~TRACEOP_JZMSK;
	v.m_operate |= TRACEOP_BIN;

	return v;
}

static const char *spHexDigit = "0123456789ABCDEF";
static const char *sphexDigit = "0123456789abcdef";
void C_HTRACE::_ToHexString(unsigned char val, string &strDigit, bool upcase)
{
	const char *pdigit = (upcase==true? spHexDigit:sphexDigit); 

	strDigit += pdigit[val>>4];
	strDigit += pdigit[val&0x0F];
}

void C_HTRACE::ToHexString(unsigned char val, string &strDigit, bool upcase)
{
	strDigit += "0x";
	_ToHexString(val, strDigit, upcase);
}

void C_HTRACE::ToHexString(char val, string &strDigit, bool upcase)
{
	ToHexString((unsigned char)val, strDigit, upcase);
}

void C_HTRACE::ToHexString(unsigned short val, string &strDigit, bool upcase)
{
	strDigit += "0x";
	_ToHexString((unsigned char)(val>>8), strDigit, upcase);
	_ToHexString((unsigned char)(val>>0), strDigit, upcase);
}

void C_HTRACE::ToHexString(short val, string &strDigit, bool upcase)
{
	ToHexString((unsigned short)val, strDigit, upcase);
}

void C_HTRACE::ToHexString(unsigned long val, string &strDigit, bool upcase)
{
	strDigit += "0x";
	_ToHexString((unsigned char)(val>>24), strDigit, upcase);
	_ToHexString((unsigned char)(val>>16), strDigit, upcase);
	_ToHexString((unsigned char)(val>> 8), strDigit, upcase);
	_ToHexString((unsigned char)(val>> 0), strDigit, upcase);
}

void C_HTRACE::ToHexString(long val, string &strDigit, bool upcase)
{
	ToHexString((unsigned long)val, strDigit, upcase);
}

void C_HTRACE::ToHexString(unsigned int val, string &strDigit, bool upcase)
{
	if (sizeof(int) == 2)
		ToHexString((unsigned short)val, strDigit, upcase);
	else
		ToHexString((unsigned long)val, strDigit, upcase);
}

void C_HTRACE::ToHexString(int val, string &strDigit, bool upcase)
{
	ToHexString((unsigned int)val, strDigit, upcase);
}

void C_HTRACE::ToDecString(long val, string &strDigit)
{
	int power, tmp;
	bool sign = (val<0? true:false);

	if (sign == true)
	{
		val = -val;
		strDigit += '-';
	}

	for (tmp = val, power = 1; tmp >= 10; tmp /= 10)
		power *= 10;

	for (tmp = 0; power > 0; power /= 10,tmp++)
	{
		strDigit += '0' + (char)(val/power);
		val %= power;
	}
}

void C_HTRACE::ToDecString(unsigned long val, string &strDigit)
{
	int power, tmp;

	for (tmp = val, power = 1; tmp >= 10; tmp /= 10)
		power *= 10;

	for (tmp = 0; power > 0; power /= 10,tmp++)
	{
		strDigit += '0' + (char)(val/power);
		val %= power;
	}
}

char *C_HTRACE::GetOutfileName(S_TRACESET &rSet, char *Outfile)
{
	if (rSet.GetWay() == S_TRACESET::WAY_LOGFILE)
	{
		sprintf(Outfile, "%s%s%s%s", TRACE_LOG_BASEPATH, GetLogPrivateDir(), GetLogfileName(), GetLogfileSuffix());
	}
	else if (rSet.GetWay()>=S_TRACESET::WAY_TTYP0 && rSet.GetWay()<S_TRACESET::WAY_MAX)
	{//for Linux tty
		const char *ttydir = "/dev/pts/";
		unsigned char tty = rSet.GetWay() - S_TRACESET::WAY_TTYP0;

		sprintf(Outfile, "%s%d", ttydir, tty);
	}

	return Outfile;
}

char *C_HTRACE::GetBackfileName(S_TRACESET &rSet, char *OutDir)
{
	if (rSet.GetWay() == S_TRACESET::WAY_LOGFILE)
	{
		sprintf(OutDir, "%s%s%s%s", TRACE_LOG_BASEPATH, GetLogPrivateDir(), GetBackupLogName(), GetLogfileSuffix());
	}
	return OutDir;
}

bool C_HTRACE::RenameLogFile(const char *SrcFile, const char *DstFile)
{
	return (rename(SrcFile, DstFile) == 0);
}

bool C_HTRACE::DelLogFile(S_TRACESET &rSet)
{
	int MaxTimes = 0;// explicitly control delete times
	while ((MaxTimes++ < 3) && (DelOneLogFile(rSet) == false))
		;
	return true;
}

#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>
#endif

bool C_HTRACE::DelOneLogFile(S_TRACESET &rSet)
{
#ifdef WIN32
	intptr_t hfind;
	char dir[260];
	char fil[260];
	char cwd[260];
	char filter[32] = "*";
	struct _finddata_t findinfo;

	memset(dir, 0x00, sizeof(dir));
	memset(fil, 0x00, sizeof(fil));
	memset(cwd, 0x00, sizeof(cwd));

	sprintf(dir, "%s%s", TRACE_LOG_BASEPATH,  GetLogPrivateDir());
	strcat(filter, GetLogfileSuffix());

	_getcwd(cwd, sizeof(cwd));
	_chdir(dir);

	int LogFileCnt = 0;
	if (hfind = _findfirst(filter, &findinfo))
	{
		strcpy(fil, findinfo.name);
		LogFileCnt++;
		while (_findnext(hfind, &findinfo) == 0)
		{
			if (strcmp(findinfo.name, fil) < 0)
				strcpy(fil, findinfo.name);
			LogFileCnt++;
		}
        _findclose(hfind);
	}

	if (LogFileCnt > rSet.GetFileTotal())
		remove(fil);

	_chdir(cwd);

	return (LogFileCnt>(rSet.GetFileTotal()+1)? false:true);
#else
	char dir[260];
	char fil[260];
	int LogFileCnt = 0;
	DIR *pDIR;
	struct dirent *pdirent;

	memset(dir, 0x00, sizeof(dir));
	memset(fil, 0x00, sizeof(fil));
	sprintf(dir, "%s%s", TRACE_LOG_BASEPATH,  GetLogPrivateDir());

	if ((pDIR = opendir(dir)) == NULL)
		return true;//it's failed, don't repeat del

	while ((pdirent = readdir(pDIR)) != NULL)
	{
		if (strcmp(pdirent->d_name, ".")==0 || strcmp(pdirent->d_name, "..")==0)
			continue; //ignore dot and dot-dot
		if (strstr(pdirent->d_name, GetLogfileSuffix()) == NULL)
			continue; //ignore other file
		if (fil[0] == '\0')
			strcpy(fil, pdirent->d_name);
		else if (strcmp(pdirent->d_name, fil) < 0)
			strcpy(fil, pdirent->d_name);
		LogFileCnt++;
	}

	closedir(pDIR);
	
	if (LogFileCnt > rSet.GetFileTotal())
	{
		char rmfile[260];
		sprintf(rmfile, "%s%s", dir, fil);
		remove(rmfile);
	}
	return (LogFileCnt>(rSet.GetFileTotal()+1)? false:true);
#endif
}


C_HTRACE hai_logn((unsigned char *)"hdjs", S_TRACESET::L_DISABLED);
C_HTRACE hai_loge((unsigned char *)"hdjs", S_TRACESET::L_ERROR);
C_HTRACE hai_logw((unsigned char *)"hdjs", S_TRACESET::L_WARNING);
C_HTRACE hai_logi((unsigned char *)"hdjs", S_TRACESET::L_INFO);
C_HTRACE hai_logv((unsigned char *)"hdjs", S_TRACESET::L_VERBOSE);
C_HTRACE hai_logd((unsigned char *)"hdjs", S_TRACESET::L_DEBUG);

