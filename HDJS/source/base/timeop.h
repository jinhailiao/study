//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			timeop.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-14  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __TIMEOP_H__
#define __TIMEOP_H__
#include <time.h>

class C_TIME
{
public:
	enum TIME_UNIT{
		TIME_UNIT_UNKNOW,
		TIME_UNIT_SECOND,
		TIME_UNIT_MINUTE,
		TIME_UNIT_HOUR,
		TIME_UNIT_DAY,
		TIME_UNIT_MONTH,
		TIME_UNIT_YEAR
	};

	static char *GetTimeLogFormat(char *ptimeBuf, int size);
	static char *GetTimeString(char *ptimeBuf, int size);

	static void CurTime2BCDStr(char *bcdStr7B){Time2BCDStr(time(NULL), bcdStr7B);}
	static void Time2BCDStr(time_t time, char *bcdStr7B);
	static time_t BcdStr2Time(char *bcdStr7B);
	static int SetSysHWClock(char *bcdStr7B);

	static time_t DateTrim(TIME_UNIT interUnit, time_t date);
};

#endif //__TIMEOP_H__


