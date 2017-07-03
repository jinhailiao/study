/** @file
 *  @brief 时间操作集
 *  @author jinhailiao
 *  @date 2011/11/07
 *  @version 0.1
 *  1、时间操作类-符合国网规约格式ssmmhhDDMMYYYY
 *  2、时间操作类-符合南网规约格式YYYYMMDDhhmmss
 */
#include "timeop.h"
#include <time.h>

/** 取当前时钟(封装time(NULL)) */
S_TIME C_TIME::TimeNow(void)
{
	return (S_TIME)time(NULL);
}

S_TIME C_TIME::MakeTime(const S_DATE &date)
{
	struct tm tim;
	memset(&tim, 0x00, sizeof(tim));
	tim.tm_year = date.m_Year - 1900;
	tim.tm_mon  = date.m_Mon - 1;
	tim.tm_mday = date.m_Day;
	tim.tm_hour = date.m_Hour;
	tim.tm_min  = date.m_Min;
	tim.tm_sec  = date.m_Sec;
	return  (S_TIME)mktime(&tim);
}

S_DATE *C_TIME::LocalTime(S_TIME mytime, S_DATE &date)
{
	time_t needtime = (time_t)mytime;
	struct tm *tim = localtime(&needtime);
	
	date.m_Year = tim->tm_year + 1900;
	date.m_Mon  = tim->tm_mon + 1;
	date.m_Day  = tim->tm_mday;
	date.m_Hour = tim->tm_hour;
	date.m_Min  = tim->tm_min;
	date.m_Sec  = tim->tm_sec;
	date.m_WeekDay = tim->tm_wday;
	date.m_YearDay = tim->tm_yday + 1;
	
	return &date;
}

/** BCD串的格式为:ssmmhhDDMMYYYY 符合国网规约*/
char * C_TIME::Time2BCDStr(S_TIME time, char *bcdStr7B)
{
	time_t mytime = (S_TIME)time;
	struct tm *curtime = localtime(&mytime);

	bcdStr7B[0] = kge_cbcd(curtime->tm_sec);
	bcdStr7B[1] = kge_cbcd(curtime->tm_min);
	bcdStr7B[2] = kge_cbcd(curtime->tm_hour);
	bcdStr7B[3] = kge_cbcd(curtime->tm_mday);
	bcdStr7B[4] = kge_cbcd(curtime->tm_mon+1);
	bcdStr7B[5] = kge_cbcd((curtime->tm_year+1900)%100);
	bcdStr7B[6] = kge_cbcd((curtime->tm_year+1900)/100);
	return bcdStr7B;
}

/** BCD串的格式为:ssmmhhDDMMYYYY 符合国网规约*/
S_TIME C_TIME::BcdStr2Time(const char *bcdStr7B)
{
	struct tm curtime;
	memset(&curtime,0x00,sizeof(struct tm));

	curtime.tm_year = kge_bcdc(bcdStr7B[5]) + kge_bcdc(bcdStr7B[6])*100 - 1900;
	curtime.tm_mon  = kge_bcdc(bcdStr7B[4]) - 1;
	curtime.tm_mday = kge_bcdc(bcdStr7B[3]);
	curtime.tm_hour = kge_bcdc(bcdStr7B[2]);
	curtime.tm_min = kge_bcdc(bcdStr7B[1]);
	curtime.tm_sec = kge_bcdc(bcdStr7B[0]);
	curtime.tm_wday = 0;
	curtime.tm_yday = 0;
	curtime.tm_isdst = -1;

	return (S_TIME)mktime(&curtime);
}

S_TIME C_TIME::BcddateAdd(S_TIME_UNIT interUnit, long offset, const char *srcBcd7B, char *dstBcd7B)
{
	S_TIME date = DateAdd(interUnit, offset, BcdStr2Time(srcBcd7B));
	Time2BCDStr(date, dstBcd7B);
	return date;
}

/** BCD串的格式为:ssmmhhDDMMYYYY 符合国网规约*/
int C_TIME::SetSysHWClock(const char *bcdStr7B)
{
	int err = 0;
	char cmd[64];
	memset(cmd, 0x00, sizeof(cmd));
	sprintf(cmd, "date %02x%02x%02x%02x%02x%02x.%02x", bcdStr7B[4], bcdStr7B[3], bcdStr7B[2], bcdStr7B[1], bcdStr7B[6], bcdStr7B[5], bcdStr7B[0]);

#ifdef WIN32
//	cout<<"Set Hardware Time to "<<cmd<<endl;
	return err;
#else
	if ((err = system(cmd)) == 0)
		err = system("hwclock -w");
	return err;
#endif
}

/** BCD串的格式为:YYYYMMDDhhmmss 符合南网规约*/
char *C_TIME::Time2BCDStr_Big(S_TIME time, char *bcdStr7B)
{
	Time2BCDStr(time, bcdStr7B);
	reverse(bcdStr7B, bcdStr7B+7);
	return bcdStr7B;
}
/** BCD串的格式为:YYYYMMDDhhmmss 符合南网规约*/
S_TIME C_TIME::BcdStr2Time_Big(const char *bcdStr7B)
{
	char mybcdStr7B[7];
	reverse_copy(bcdStr7B, bcdStr7B+7, mybcdStr7B);
	return BcdStr2Time(mybcdStr7B);
}
/** BCD串的格式为:YYYYMMDDhhmmss 符合南网规约*/
int C_TIME::SetSysHWClock_Big(const char *bcdStr7B)
{
	char mybcdStr7B[7];
	reverse_copy(bcdStr7B, bcdStr7B+7, mybcdStr7B);
	return SetSysHWClock(mybcdStr7B);
}

S_TIME C_TIME::BcdStr2Time5B_Big(const char *bcdStr5B)
{
	char bcdStr7B[7];
	reverse_copy(bcdStr5B, bcdStr5B+5, bcdStr7B+1);
	bcdStr7B[6] = 0x20;//YY
	bcdStr7B[0] = 0x00;//SS
	return BcdStr2Time(bcdStr7B);
}

char* C_TIME::Time2BcdStr5B_Big(S_TIME time, char *bcdStr5B)
{
	char bcdStr7B[7];
	Time2BCDStr(time, bcdStr7B);
	reverse_copy(bcdStr7B+1, bcdStr7B+1+5, bcdStr5B);
	return bcdStr5B;
}

S_TIME C_TIME::BcddateAdd_Big(S_TIME_UNIT interUnit, long offset, const char *srcBcd7B, char *dstBcd7B)
{
	S_TIME date = DateAdd(interUnit, offset, BcdStr2Time_Big(srcBcd7B));
	Time2BCDStr_Big(date, dstBcd7B);
	return date;
}


/** @brief 日期对齐
 *  @param[in] interUnit 时间单位
 *  @param[in] date 日期
 *  @return 规整后的日期
 */
S_TIME C_TIME::DateTrim(S_TIME_UNIT interUnit, S_TIME date)
{
	time_t mydate = (time_t)date;
	tm* t = localtime(&mydate);

	if (interUnit >= TIME_UNIT_MINUTE) t->tm_sec=0;
	if (interUnit >= TIME_UNIT_HOUR) t->tm_min=0;
	if (interUnit >= TIME_UNIT_DAY) t->tm_hour=0;
	if (interUnit >= TIME_UNIT_MONTH) t->tm_mday=1;
	if (interUnit >= TIME_UNIT_YEAR) t->tm_mon=0;
	t->tm_wday=0;
	t->tm_yday=0;
	t->tm_isdst=-1;

	return (S_TIME)mktime(t);
}

/** @breif 日期单位时间增减
 *  @param[in] interUnit 日期单位
 *  @param[in] offset 时间偏移
 *  @param[in] date 日期
 *  @return 日期
 */
S_TIME C_TIME::DateAdd(S_TIME_UNIT interUnit, long offset, S_TIME date)
{
	time_t mydate = (time_t)date;
	if(mydate>0 && offset!=0)
	{
		tm* t=localtime(&mydate);
		switch(interUnit)
		{
		case TIME_UNIT_SECOND: t->tm_sec  += offset; break;
		case TIME_UNIT_MINUTE: t->tm_min  += offset; break;
		case TIME_UNIT_HOUR:   t->tm_hour += offset; break;
		case TIME_UNIT_DAY:    t->tm_mday += offset; break;
		case TIME_UNIT_MONTH:  t->tm_mon  += offset; break;
		case TIME_UNIT_YEAR:   t->tm_year += offset; break;
		default:break;
		}
		t->tm_wday  = 0;
		t->tm_yday  = 0;
		t->tm_isdst = -1;
		return (S_TIME)mktime(t);
	}
	return date;
}

/** @brief 计算日期间的单位差
 *  @param[in] interUnit 时间单位
 *  @param[in] begin 开始时间
 *  @param[in] end 结束时间
 *  @return >= 0 (end>=begin)
 *  @return <0 (end<begin)
 */
long C_TIME::DateDiff(S_TIME_UNIT interUnit, S_TIME begin, S_TIME end)
{
	time_t d1 = (time_t)DateTrim(interUnit, begin);
	time_t d2 = (time_t)DateTrim(interUnit, end);

	if (interUnit < TIME_UNIT_MONTH)//sec,min,hour,day
	{
		if (interUnit == TIME_UNIT_SECOND)
			return (long)difftime(d2,d1);
		else if (interUnit == TIME_UNIT_MINUTE)
			 return (long)difftime(d2,d1)/((long)60);
		else if(interUnit == TIME_UNIT_HOUR)
			return (long)difftime(d2,d1)/((long)3600);
		else if(interUnit == TIME_UNIT_DAY)
			return (long)difftime(d2,d1)/((long)86400);
		else
			return 0;
	}
	else
	{
		struct tm t1 = *localtime(&d1);
		struct tm t2 = *localtime(&d2);
		if (interUnit == TIME_UNIT_MONTH)
			return (t2.tm_year*12+t2.tm_mon)-(t1.tm_year*12+t1.tm_mon);
		else
			return t2.tm_year-t1.tm_year;
	}
	return 0;
}

/** @brief 取时间串，格式:MM-DD hh:mm:ss
 *  @param[out] ptimeBuf 格式串输出缓冲
 *  @param[in] size 缓冲大小
 *  @return NULL size太小
 *  @return ptimeBuf
 */
char *C_TIME::Time2Stamp(char *strAscii14B)
{
	time_t t = time(NULL);
	struct tm *curtm = localtime(&t);

	strAscii14B[0] = (curtm->tm_mon+1)/10 + '0';
	strAscii14B[1] = (curtm->tm_mon+1)%10 + '0';
	strAscii14B[2] = '-';
	strAscii14B[3] = curtm->tm_mday/10 + '0';
	strAscii14B[4] = curtm->tm_mday%10 + '0';
	strAscii14B[5] = ' ';
	strAscii14B[6] = curtm->tm_hour/10 + '0';
	strAscii14B[7] = curtm->tm_hour%10 + '0';
	strAscii14B[8] = ':';
	strAscii14B[9] = curtm->tm_min/10 + '0';
	strAscii14B[10] = curtm->tm_min%10 + '0';
	strAscii14B[11] = ':';
	strAscii14B[12] = curtm->tm_sec/10 + '0';
	strAscii14B[13] = curtm->tm_sec%10 + '0';

	return strAscii14B;
}

/** @brief 取时间串，格式:YYYYMMDDhhmm
 *  @param[out] ptimeBuf 格式串输出缓冲
 *  @param[in] size 缓冲大小
 *  @return NULL size太小
 *  @return ptimeBuf
 */
char *C_TIME::Now2Ascii12(char *strAscii12B)
{
	char strAscii16B[16];
	
	C_TIME::Time2Ascii14(TimeNow(), strAscii16B);
	memcpy(strAscii12B, strAscii16B, 12);

	return strAscii12B;
}

/** @brief 取时间串，格式:YYYYMMDDhhmmss
 *  @param[out] strAscii16B 格式串输出缓冲
 *  @param[in] time 时针
 *  @return strAscii16B
 */
char* C_TIME::Time2Ascii14(S_TIME time, char *strAscii16B)
{
	int i;
	char bcdStr7B[8];
	Time2BCDStr(time, bcdStr7B);

	for (i = 0; i < 7; i++)
	{
		strAscii16B[i*2+0] = ((bcdStr7B[6-i]>>4)&0x0F)+'0';
		strAscii16B[i*2+1] = ((bcdStr7B[6-i]>>0)&0x0F)+'0';
	}
	strAscii16B[i*2+0] = '\0';
	return strAscii16B;
}

/** @brief 超时类定义 */
C_MSTIMOUT::C_MSTIMOUT(S_DWORD ms)
{
	m_timeStart = C_TIME::TimeNow();
	m_timout = ms/1000 + ((ms%1000)? 1:0);
}

C_MSTIMOUT::~C_MSTIMOUT()
{
}

/** @brief 超时判断
 *  @return true 超时；false 未超时
 */
S_BOOL C_MSTIMOUT::timeout(void)
{
	S_TIME CurTime = C_TIME::TimeNow();

	if (CurTime < m_timeStart)
		m_timeStart = CurTime;
	return (CurTime - m_timeStart) > m_timout;
}


