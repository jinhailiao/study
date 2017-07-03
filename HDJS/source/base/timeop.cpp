//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			timeop.cpp
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-14  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "timeop.h"
#include "haidef.h"
#include "htrace.h"

char *C_TIME::GetTimeLogFormat(char *ptimeBuf, int size)
{
	if (ptimeBuf == NULL || size < 14)
		return NULL;

	time_t t = time(NULL);
	struct tm *curtm = localtime(&t);

	ptimeBuf[0] = (curtm->tm_mon+1)/10 + '0';
	ptimeBuf[1] = (curtm->tm_mon+1)%10 + '0';
	ptimeBuf[2] = '-';
	ptimeBuf[3] = curtm->tm_mday/10 + '0';
	ptimeBuf[4] = curtm->tm_mday%10 + '0';
	ptimeBuf[5] = ' ';
	ptimeBuf[6] = curtm->tm_hour/10 + '0';
	ptimeBuf[7] = curtm->tm_hour%10 + '0';
	ptimeBuf[8] = ':';
	ptimeBuf[9] = curtm->tm_min/10 + '0';
	ptimeBuf[10] = curtm->tm_min%10 + '0';
	ptimeBuf[11] = ':';
	ptimeBuf[12] = curtm->tm_sec/10 + '0';
	ptimeBuf[13] = curtm->tm_sec%10 + '0';

	return ptimeBuf;
}

char *C_TIME::GetTimeString(char *ptimeBuf, int size)
{
	if (ptimeBuf == NULL || size < 12)
		return NULL;

	time_t t = time(NULL);
	struct tm *curtm = localtime(&t);

	ptimeBuf[0] = (curtm->tm_year+1900)/1000 + '0';
	ptimeBuf[1] = (curtm->tm_year+1900)/100%10 + '0';
	ptimeBuf[2] = (curtm->tm_year+1900)%100/10 + '0';
	ptimeBuf[3] = (curtm->tm_year+1900)%10 + '0';
	ptimeBuf[4] = (curtm->tm_mon+1)/10 + '0';
	ptimeBuf[5] = (curtm->tm_mon+1)%10 + '0';
	ptimeBuf[6] = curtm->tm_mday/10 + '0';
	ptimeBuf[7] = curtm->tm_mday%10 + '0';
	ptimeBuf[8] = curtm->tm_hour/10 + '0';
	ptimeBuf[9] = curtm->tm_hour%10 + '0';
	ptimeBuf[10] = curtm->tm_min/10 + '0';
	ptimeBuf[11] = curtm->tm_min%10 + '0';

	return ptimeBuf;
}

//所有时间操作函数集,符合国网上行规约顺序
void C_TIME::Time2BCDStr(time_t time, char *bcdStr7B)
{
	struct tm *curtime = localtime(&time);

	bcdStr7B[0] = cbcd(curtime->tm_sec);
	bcdStr7B[1] = cbcd(curtime->tm_min);
	bcdStr7B[2] = cbcd(curtime->tm_hour);
	bcdStr7B[3] = cbcd(curtime->tm_mday);
	bcdStr7B[4] = cbcd(curtime->tm_mon+1);
	bcdStr7B[5] = cbcd((curtime->tm_year+1900)%100);
	bcdStr7B[6] = cbcd((curtime->tm_year+1900)/100);
}

time_t C_TIME::BcdStr2Time(char *bcdStr7B)
{
	struct tm curtime;
	memset(&curtime,0x00,sizeof(struct tm));

	curtime.tm_year = bcdc(bcdStr7B[5]) + bcdc(bcdStr7B[6])*100 - 1900;
	curtime.tm_mon = bcdc(bcdStr7B[4])-1;
	curtime.tm_mday = bcdc(bcdStr7B[3]);
	curtime.tm_hour = bcdc(bcdStr7B[2]);
	curtime.tm_min = bcdc(bcdStr7B[1]);
	curtime.tm_sec = bcdc(bcdStr7B[0]);
	curtime.tm_wday = 0;
	curtime.tm_yday = 0;
	curtime.tm_isdst = -1;

	return mktime(&curtime);
}

int C_TIME::SetSysHWClock(char *bcdStr7B)
{
	int err = 0;
	char cmd[64];
	memset(cmd, 0x00, sizeof(cmd));
	sprintf(cmd, "date %02x%02x%02x%02x%02x%02x.%02x", bcdStr7B[4], bcdStr7B[3], bcdStr7B[2], bcdStr7B[1], bcdStr7B[6], bcdStr7B[5], bcdStr7B[0]);

#ifdef WIN32
	logit<<"Set Hardware Time to "<<cmd<<endl;
	return err;
#else
	if ((err = system(cmd)) == 0)
		err = system("hwclock -w");
	return err;
#endif
}

time_t C_TIME::DateTrim(TIME_UNIT interUnit, time_t date)
{
	tm* t = localtime(&date);

	if (interUnit >= TIME_UNIT_MINUTE) t->tm_sec=0;
	if (interUnit >= TIME_UNIT_HOUR) t->tm_min=0;
	if (interUnit >= TIME_UNIT_DAY) t->tm_hour=0;
	if (interUnit >= TIME_UNIT_MONTH) t->tm_mday=1;
	if (interUnit >= TIME_UNIT_YEAR) t->tm_mon=0;
	t->tm_wday=0;
	t->tm_yday=0;
	t->tm_isdst=-1;

	return mktime(t);
}



