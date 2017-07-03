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
// 1.0.0		2010-04-15  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "appbase.h"

unsigned char Char2Hex(char ch)
{
	if (ch >= '0' && ch <= '9')
		return (ch - '0');
	else if (ch >= 'A' && ch <= 'F')
		return (ch - 'A' + 0x0A);
	else if (ch >= 'a' && ch <= 'f')
		return (ch - 'a' + 0x0A);
	return 0;
}

char Hex2Char(unsigned char hex)
{
	if (hex <= 0x09)
		return ('0' + hex);
	else if (hex <= 0x0F)
		return ('A' + (hex - 0x0A));
	return '0';
}

C_MSTIMOUT::C_MSTIMOUT(S_DWORD ms)
{
	m_start = time(NULL);
	m_timout = ms/1000 + ((ms%1000)? 1:0);
}

C_MSTIMOUT::~C_MSTIMOUT()
{
}

S_BOOL C_MSTIMOUT::timout(void)
{
	time_t cur = time(NULL);

	if (cur > m_start)
		return (cur - m_start) > m_timout;
	else if (cur < m_start)
		return (m_start - cur) > m_timout;

	return false;
}


