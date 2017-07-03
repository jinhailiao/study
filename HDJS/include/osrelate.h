// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			osrelate.h
// Description:		handle OS relation
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-16  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __OSRELATE_H__
#define __OSRELATE_H__

#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#endif


#ifdef WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

#ifdef WIN32
#define PID_T	int
#define getpid()	0xFFFF
#else
#define PID_T	pid_t
#endif

inline void SleepMS(int ms)
{
#ifdef WIN32
	Sleep(ms);
#else
	usleep(ms*1000);
#endif
}

inline void SleepSecond(int Second)
{
#ifdef WIN32
	Sleep(Second*1000);
#else
	sleep(Second);
#endif
	
}


#endif //__OSRELATE_H__

