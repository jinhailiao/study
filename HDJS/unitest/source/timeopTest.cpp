//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			timeopTest.cpp
// Description:		timeop.cpp cppunit test
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-26  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "timeopTest.h"
#include "timeop.h"


CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(C_TIME_TestCase, "MOD_BASE"); 

void C_TIME_TestCase::testGetTimeLogFormat(void)
{
	char *pBuff, Buff[20];

	pBuff = C_TIME::GetTimeLogFormat(NULL, 1);
	CPPUNIT_ASSERT(pBuff == NULL);
	pBuff = C_TIME::GetTimeLogFormat(NULL, 14);
	CPPUNIT_ASSERT(pBuff == NULL);
	pBuff = C_TIME::GetTimeLogFormat(Buff, 1);
	CPPUNIT_ASSERT(pBuff == NULL);
	pBuff = C_TIME::GetTimeLogFormat(Buff, 13);
	CPPUNIT_ASSERT(pBuff == NULL);
	pBuff = C_TIME::GetTimeLogFormat(Buff, 14);
	CPPUNIT_ASSERT(pBuff == Buff);
	pBuff = C_TIME::GetTimeLogFormat(Buff, 15);
	CPPUNIT_ASSERT(pBuff == Buff);
	pBuff = C_TIME::GetTimeLogFormat(Buff, 20);
	CPPUNIT_ASSERT(pBuff == Buff);
}

void C_TIME_TestCase::testGetTimeString(void)
{
	char *pBuff, Buff[20];

	pBuff = C_TIME::GetTimeString(NULL, 1);
	CPPUNIT_ASSERT(pBuff == NULL);
	pBuff = C_TIME::GetTimeString(NULL, 16);
	CPPUNIT_ASSERT(pBuff == NULL);
	pBuff = C_TIME::GetTimeString(Buff, 1);
	CPPUNIT_ASSERT(pBuff == NULL);
	pBuff = C_TIME::GetTimeString(Buff, 11);
	CPPUNIT_ASSERT(pBuff == NULL);
	pBuff = C_TIME::GetTimeString(Buff, 12);
	CPPUNIT_ASSERT(pBuff == Buff);
	pBuff = C_TIME::GetTimeString(Buff, 13);
	CPPUNIT_ASSERT(pBuff == Buff);
	pBuff = C_TIME::GetTimeString(Buff, 20);
	CPPUNIT_ASSERT(pBuff == Buff);
}

