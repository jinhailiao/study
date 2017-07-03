//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haitime.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06	update					Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#include "haitype.h"

S_VOID _hai_TimerEventISR(S_VOID);

typedef struct tagCALENDAR
{
	S_WORD year;
	S_BYTE mon;
	S_BYTE day;
}S_CALENDAR;

typedef struct tagTIME 
{
	S_BYTE hour;
	S_BYTE minute;
	S_BYTE second;
}S_TIME;


S_DWORD volatile	TimeTick = 0;
static  S_CHAR	TimeFlag = 10;//count second, because time interrupt come  per 1/10 second
static  S_TIME		CurTime  = {0,0,0};
static  S_CALENDAR	Calendar = {2008,1,1};

#ifdef PCSIM
int year, mon, day, hour, minute, second;
#endif


S_ROMDATA S_CHAR LeapYear[12] = 
{
	31,29,31,30,31,30,31,31,30,31,30,31,
};

S_ROMDATA S_CHAR NotLeapYear[12] = 
{
	31,28,31,30,31,30,31,31,30,31,30,31,
};

static S_VOID hai_UpdateCalendar(S_VOID)
{
	S_BOOL leap = 0;
	S_CHAR LimitDay;

	if (Calendar.year % 4 == 0) 
	{
		if (Calendar.year % 100 != 0)
			leap = 1;
		else if (Calendar.year % 400 == 0)
			leap = 1;
	}

	LimitDay = leap? LeapYear[Calendar.mon - 1]:NotLeapYear[Calendar.mon - 1];

	if (Calendar.day == LimitDay) 
	{
		Calendar.day = 1;
		if (Calendar.mon == 12) 
		{
			Calendar.mon = 1;
			Calendar.year++;
		} 
		else 
		{
			Calendar.mon++;
		}
	} 
	else 
	{
		Calendar.day++;	
	}
}

static S_VOID hai_UpdateTime(S_VOID)
{
	S_BOOL update = 0;

	if (CurTime.second == 59) 
	{
		CurTime.second = 0;
		if (CurTime.minute == 59) 
		{
			CurTime.minute = 0;
			if (CurTime.hour == 23) 
			{
				CurTime.hour = 0;
				update = 1;
			} 
			else 
				CurTime.hour++;
		} 
		else 
			CurTime.minute++;
	} 
	else 
		CurTime.second++;

	if (update) 
		hai_UpdateCalendar();
}
S_VOID hai_InitDateTime(S_VOID)
{
	TimeTick		= 0;
	TimeFlag		= 10;
	Calendar.year	= 2005;
	Calendar.mon	= 1;
	Calendar.day	= 1;
	CurTime.hour	= 0;
	CurTime.minute	= 0;
	CurTime.second	= 0;
#ifdef PCSIM
	{
	Calendar.year	= year;
	Calendar.mon	= mon;
	Calendar.day	= day;
	CurTime.hour	= hour;
	CurTime.minute	= minute;
	CurTime.second	= second;
	}
#endif
}
S_VOID hai_DoTimeISR(S_VOID)
{
	TimeTick++;

	_hai_TimerEventISR();

	if (!(--TimeFlag))
	{
		hai_UpdateTime();
		TimeFlag = 10;
	}
}

S_WORD hai_GetYear(S_VOID)
{
	return Calendar.year;
}

S_BYTE hai_GetMonth(S_VOID)
{
	return Calendar.mon;
}

S_BYTE hai_GetDay(S_VOID)
{
	return Calendar.day;
}

S_BYTE hai_GetHour(S_VOID)
{
	return CurTime.hour;
}

S_BYTE hai_GetMinute(S_VOID)
{
	return CurTime.minute;
}

S_BYTE hai_GetSecond(S_VOID)
{
	return CurTime.second;
}

S_BYTE hai_GetDayOfWeek(S_VOID)
{
	S_BYTE  Week; 
	S_WORD  year = Calendar.year; /* FIXME 2000 ~ 2099 */
	S_BYTE  mon = Calendar.mon;
	S_BYTE  day = Calendar.day;

	if((mon == 1) || (mon == 2))
	{
		mon += 12;
		year--;
	}
	Week = (day + 2*mon + 3*(mon+1)/5 + year + year/4 - year/100 + year/400) % 7; 
	return (Week + 1);
}

S_VOID hai_SetYear(S_WORD NewYear)
{
	Calendar.year = NewYear;
}

S_VOID hai_SetMonth(S_BYTE NewMon)
{
	Calendar.mon  = NewMon;
}

S_VOID hai_SetDay(S_BYTE NewDay)
{
	Calendar.day = NewDay;
}

S_VOID hai_SetHour(S_BYTE NewHour)
{
	CurTime.hour = NewHour;
}

S_VOID hai_SetMinute(S_BYTE NewMinute)
{
	CurTime.minute = NewMinute;
}

S_VOID hai_SetSecond(S_BYTE NewSecond)
{
	CurTime.second = NewSecond;
}

S_VOID  hai_GetDate(S_BYTE *pszDate)//format: 20060101
{
	pszDate[0] = Calendar.year/1000 + 0x30;
	pszDate[1] = Calendar.year/100%10 + 0x30;
	pszDate[2] = Calendar.year/10%10 + 0x30;
	pszDate[3] = Calendar.year%10 + 0x30;
	pszDate[4] = Calendar.mon/10 + 0x30;
	pszDate[5] = Calendar.mon%10 + 0x30;
	pszDate[6] = Calendar.day/10 + 0x30;
	pszDate[7] = Calendar.day%10 + 0x30;
	pszDate[8] = 0;
}

S_VOID  hai_GetTime(S_BYTE *pszTime)
{
	pszTime[0] = CurTime.hour/10 + 0x30;
	pszTime[1] = CurTime.hour%10 + 0x30;
	pszTime[2] = ':';
	pszTime[3] = CurTime.minute/10 + 0x30;
	pszTime[4] = CurTime.minute%10 + 0x30;
	pszTime[5] = ':';
	pszTime[6] = CurTime.second/10 + 0x30;
	pszTime[7] = CurTime.second%10 + 0x30;
	pszTime[8] = 0;
}

S_DWORD hai_GetTimeTick(S_VOID)
{
	S_DWORD CurTick;

	do{
		CurTick = TimeTick;
	}while (CurTick != TimeTick);

	return CurTick;
}

S_DWORD hai_GetTimeSpan(S_DWORD TickNum)
{
	S_DWORD CurTick = hai_GetTimeTick();

	return (CurTick-TickNum);
}

S_VOID hai_Delay(S_DWORD MS)
{
	S_DWORD tick = hai_GetTimeTick();

	MS += tick;

	while (tick < MS)
		tick = hai_GetTimeTick();
}


