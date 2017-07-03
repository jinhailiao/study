//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haitime.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#ifndef _HAITIME_H_
#define _HAITIME_H_

#include "haitype.h"



S_VOID  hai_Delay(S_DWORD MS);
S_WORD  hai_GetYear(S_VOID);
S_BYTE  hai_GetMonth(S_VOID);
S_BYTE  hai_GetDay(S_VOID);
S_BYTE  hai_GetHour(S_VOID);
S_BYTE  hai_GetMinute(S_VOID);
S_BYTE  hai_GetSecond(S_VOID);
S_BYTE  hai_GetDayOfWeek(S_VOID);
S_VOID  hai_SetYear(S_WORD NewYear);
S_VOID  hai_SetMonth(S_BYTE NewMonth);
S_VOID  hai_SetDay(S_BYTE NewDay);
S_VOID  hai_SetHour(S_BYTE NewHour);
S_VOID  hai_SetMinute(S_BYTE NewMinute);
S_VOID  hai_SetSecond(S_BYTE NewSecond);
S_VOID  hai_GetDate(S_BYTE *pszDate);//format: 20060101
S_VOID  hai_GetTime(S_BYTE *pszTime);//format: 00:00:00
S_DWORD hai_GetTimeTick(S_VOID);
S_DWORD hai_GetTimeSpan(S_DWORD TickNum);



#endif /* _HAITIME_H_ */
