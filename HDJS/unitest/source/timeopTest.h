//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			timeopTest.h
// Description:		timeop.h cppunit test
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-26  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __TIMEOPTEST_H__
#define __TIMEOPTEST_H__

#include <cppunit/extensions/HelperMacros.h>

class C_TIME_TestCase:public CPPUNIT_NS::TestCase
{
	CPPUNIT_TEST_SUITE(C_TIME_TestCase);
	CPPUNIT_TEST(testGetTimeLogFormat);
	CPPUNIT_TEST(testGetTimeString);
	CPPUNIT_TEST_SUITE_END();

public:
	void testGetTimeLogFormat(void);
	void testGetTimeString(void);
};

#endif //__TIMEOPTEST_H__


