//
//
//
#ifndef __GW007_FRAME_H__
#define __GW007_FRAME_H__
#include "basedef.h"
#include "inirule.h"

struct S_FrameData
{
	int m_AfnSN;
	string m_strSEQ;
	string m_strDelay;
	string m_strMaster;
	string m_strTerm;
	string m_strDA;
	string m_strDT;
	string m_strPW;
	string m_strData;
};

class C_String2Char
{
public:
	int operator()(char ch);

public:
	string m_strAscii;
};

// for_each.  Apply a function to every element of a range.
template <class _InputIter, class _Stat>
void for_Stat(_InputIter __first, _InputIter __last, _Stat &__S) {
//  __STL_REQUIRES(_InputIter, _InputIterator);
  for ( ; __first != __last; ++__first)
    __S(*__first);
}

class C_FRAME
{
public:
	static int MakeFrame(const S_RULE &rule, const S_FrameData &FrameData, string &strFrame);
	static int Ascii2Data(const string &strAscii, string &strData);
};

#endif//__GW007_FRAME_H__

