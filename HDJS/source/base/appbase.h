//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			appbase.h
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-04-15  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __APPBASE_H__
#define __APPBASE_H__

#include "haidef.h"
#include "timeop.h"

static char ByteBitCnt[] =
{
	0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

/** @brief 位处理类
 *  @note
 *  与STL bitset区别:1,类型参数是字节数; 2,可以返回位数据; 3,缓冲按BYTE存储
 */
template <int ByteCnt>
class C_BITSET
{
public:
	C_BITSET(void){ClearBit();}
	C_BITSET(const unsigned char *pString){ClearBit();ParseStr(pString);}
	C_BITSET(const unsigned char *pData, int Bytes){ClearBit();SetBit(pData, Bytes);}
	C_BITSET(C_BITSET &r){if (this != &r) SetBit(r.m_data, sizeof(r.m_data));}
	~C_BITSET(){}

	int GetByteCnt(void){return ByteCnt;}
	int GetBitSize(void){return GetByteCnt()*8;}
	unsigned char *GetData(void){return m_data;}

	void SetBit(const unsigned char *pData, int Bytes){memcpy(m_data, pData, Bytes);}
	void SetBit(int pos){m_data[pos/8] |= (unsigned char)(1<<(pos%8));}
	void SetBit(void){memset(m_data, 0xFF, sizeof(m_data));}
	void ClearBit(int pos){m_data[pos/8] &= (unsigned char)(~(1<<(pos%8)));}
	void ClearBit(void){memset(m_data, 0x00, sizeof(m_data));}
	bool TestBit(int pos){return (m_data[pos/8] & (1<<(pos%8)))?true:false;}
	void PrintStr(unsigned char *pstr)
	{
		int i;
		for (i = 0; i < ByteCnt*8; i++)
		{
			if (TestBit(i)) pstr[i]='1';
			else pstr[i]='0';
		}
		pstr[i] = '\0';
	}
	void ParseStr(const unsigned char *pstr)
	{
		for (int i = 0; pstr[i]!='\0' && i<ByteCnt*8; i++)
		{
			if (pstr[i] == '1') SetBit(i);
		}
	}
	int CountBit(void)
	{
		int AllBit = 0;
		for (int i = 0; i < ByteCnt; i++)
			AllBit += ByteBitCnt[m_data[i]];
		return AllBit;
	}

private:
	unsigned char m_data[ByteCnt];
};

unsigned char Char2Hex(char ch);
char Hex2Char(unsigned char hex);

class C_MSTIMOUT
{
public:
	C_MSTIMOUT(S_DWORD ms);
	~C_MSTIMOUT();

	S_BOOL timout(void);

private:
	time_t m_start;
	S_LONG m_timout;
};

#endif //__APPBASE_H__


