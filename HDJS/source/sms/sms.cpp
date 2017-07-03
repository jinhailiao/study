//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			sms.cpp
// Description:		sms pdu decode/encode
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-10-26  create                  Kingsea
//---------------------------------------------------------------------------------
#include "sms.h"

unsigned short hai_GB2Unicode(unsigned short GBcode);
unsigned short hai_Unicode2GB(unsigned short unicode);

//
//
//
C_SMS::C_SMS(void)
{
	m_ScaType = SMS_SCA_TYPE_INTERNATIONAL;///< 短消息服务中心地址类型
	m_PduType = 0x11; ///< PDU类型
	m_MR      = 0x00; ///< 消息参考
	m_PID     = 0x00; ///< PDU ID
	m_VP      = 0xAA; ///< 信息有效期
	m_DCS    = SMS_DCS_UCS2; ///< 数据编码方案
	m_OADA_Type  = SMS_SCA_TYPE_INTERNATIONAL; ///< 发送号码和输出号码类型
	memset(m_SCTS, 0x00, sizeof(m_SCTS));
}

C_SMS::~C_SMS()
{
}

/**  @fn int C_SmsCoder::SmsCode(const string &strSCA, const string &strDA, const string &strUD, string &strSubmit)
 *  @brief 短消息编码函数
 *  @param[in] strSCA 短消息中心号码，如：13800000501
 *  @param[in] strDA 目标地址号码，如：13866668888
 *  @param[in] strUD 用户数据,如：你好！
 *  @param[out] strSubmit 编码完成的字符串
 *  @return PDU数据长度
 */
int C_SmsCoder::SmsEncode(const string &strSCA, const string &strDA, const string &strUD, string &strSubmit)
{//SCA PDUType MR DA PID DCS VP UDL UD
 	if (ScaEncode(strSCA, strSubmit) != 0)//SCA
		return -1;
	size_t ScaLen = strSubmit.size();//record SCA length

	PushBack(m_SmsParam.m_PduType, strSubmit);//PDUType
	PushBack(m_SmsParam.m_MR, strSubmit);//MR
	if (DaEncode(strDA, strSubmit) != 0)//DA
		return -1;
	PushBack(m_SmsParam.m_PID, strSubmit);//PID
	PushBack(m_SmsParam.m_DCS, strSubmit);//DCS
	PushBack(m_SmsParam.m_VP, strSubmit);//VP
	if (UdEncode(strUD, strSubmit) != 0)
		return -1;
	
	return (int)(strSubmit.size() - ScaLen) / 2;
}

/**  @fn int SmsDecode(const string &strDeliver, string &strOA, string &strUD, string &strSCTS);
 *  @brief 短消息解码函数
 *  @param[in] strDeliver  短消息串
 *  @param[in] strOA 发送方地址号码，如：13866668888
 *  @param[in] strUD 用户数据,如：你好！
 *  @param[out] strSCTS 服务中心时间戳
 *  @return strUD数据长度
 */
int C_SmsCoder::SmsDecode(const string &strDeliver, string &strOA, string &strUD, string &strSCTS)
{
	int num, start = 0;
	string strSrc, strNum;
	
	string strSCA;
	strNum.append(strDeliver, start, 2);
	sscanf(strNum.c_str(), "%x", &num);
	strSrc.assign(strDeliver, start, num*2+2);
	ScaDecode(strSrc, strSCA);

	start += num*2+2;
	strNum.assign(strDeliver, start, 2);
	sscanf(strNum.c_str(), "%x", &num);
	m_SmsParam.m_PduType = (S_BYTE)num;

	start += 2;
	strNum.assign(strDeliver, start, 2);
	sscanf(strNum.c_str(), "%x", &num);
	if (num % 2 == 1) num += 1;
	strSrc.assign(strDeliver, start, 2 + 2 + num);
	OaDecode(strSrc, strOA);

	start += 2 + 2 + num;
	strNum.assign(strDeliver, start, 2);
	sscanf(strNum.c_str(), "%x", &num);
	m_SmsParam.m_PID = (S_BYTE)num;

	start += 2;
	strNum.assign(strDeliver, start, 2);
	sscanf(strNum.c_str(), "%x", &num);
	m_SmsParam.m_DCS = (S_BYTE)num;

	start += 2;
	strSrc.assign(strDeliver, start, 7 * 2);
	SctsDecode(strSrc, strSCTS);

	start += 7 * 2;
	strSrc.assign(strDeliver, start, strDeliver.size() - start);
	return UdDecode(strSrc, strUD);
}

int C_SmsCoder::ScaEncode(const string &strSCA, string &strSubmit)
{
	if (strSCA.empty())
	{
		strSubmit.append("00");
	}
	else if (m_SmsParam.m_ScaType == SMS_SCA_TYPE_INTERNATIONAL)
	{
		string strMySCA;
		PhoneNumberReverse(strSCA, strMySCA);
		S_BYTE ScaLen = (S_BYTE)strMySCA.size()/2 + 2;// 2 0x91 and 0x86
		PushBack(ScaLen, strSubmit); //put length
		PushBack(SMS_SCA_TYPE_INTERNATIONAL, strSubmit);//put SCA type
		PushBackReverse(0x86, strSubmit);//put 0x86
		strSubmit.append(strMySCA);
	}
	else if (m_SmsParam.m_ScaType == SMS_SCA_TYPE_NATIONAL)
	{
		string strMySCA;
		PhoneNumberReverse(strSCA, strMySCA);
		S_BYTE ScaLen = (S_BYTE)strMySCA.size()/2 + 1;// 1 0x81
		PushBack(ScaLen, strSubmit); //put length
		PushBack(SMS_SCA_TYPE_NATIONAL, strSubmit);//put SCA type
		strSubmit.append(strMySCA);
	}
	else
		return -1;
	return 0;
}

int C_SmsCoder::DaEncode(const string &strDA, string &strSubmit)
{
	string strMyDA;
	PhoneNumberReverse(strDA, strMyDA);
	if (m_SmsParam.m_ScaType == SMS_SCA_TYPE_INTERNATIONAL)
	{
		S_BYTE ScaLen = (S_BYTE)strDA.size() + 2;// 2 0x86
		PushBack(ScaLen, strSubmit); //put length
		PushBack(SMS_SCA_TYPE_INTERNATIONAL, strSubmit);//put SCA type
		PushBackReverse(0x86, strSubmit);//put 0x86
		strSubmit.append(strMyDA);
	}
	else if (m_SmsParam.m_ScaType == SMS_SCA_TYPE_NATIONAL)
	{
		S_BYTE ScaLen = (S_BYTE)strDA.size();
		PushBack(ScaLen, strSubmit); //put length
		PushBack(SMS_SCA_TYPE_NATIONAL, strSubmit);//put SCA type
		strSubmit.append(strMyDA);
	}
	else
		return -1;
	return 0;
}

int C_SmsCoder::UdEncode(const string &strUD, string &strSubmit)
{
	if ((m_SmsParam.m_DCS & SMS_DCS_UCS2) == SMS_DCS_UCS2)
		return DcsUcs2Encode(strUD, strSubmit);
	else if ((m_SmsParam.m_DCS & SMS_DCS_8BIT) == SMS_DCS_8BIT)
		return Dcs8BitEncode(strUD, strSubmit);
	else if ((m_SmsParam.m_DCS & SMS_DCS_7BIT) == SMS_DCS_7BIT)
		return Dcs7BitEncode(strUD, strSubmit);
	return -1;
}

int C_SmsCoder::DcsUcs2Encode(const string &strUD, string &strSubmit)
{
	string strUnicode;
	GbString2UnicodeString(strUD, strUnicode);
	PushBack((S_BYTE)strUnicode.size(), strSubmit);
	for (string::iterator iter = strUnicode.begin(); iter != strUnicode.end(); iter++)
		PushBack(*iter, strSubmit);
	return 0;
}

int C_SmsCoder::Dcs7BitEncode(const string &strUD, string &strSubmit)
{
	C_BITCODE my7BitCoder;
	PushBack((S_BYTE)strUD.size(), strSubmit);
	for (size_t cnt = 0; cnt < strUD.size(); cnt++)
	{
		my7BitCoder.SetBitData(strUD[cnt], 7);
	}
	string strBitCode;
	my7BitCoder.GetAllBitCode(strBitCode, 0x00);
	for (size_t idx = 0; idx < strBitCode.size(); idx++)
		PushBack(strBitCode[idx], strSubmit);
	return 0;
}

int C_SmsCoder::Dcs8BitEncode(const string &strUD, string &strSubmit)
{
	PushBack((S_BYTE)strUD.size(), strSubmit);
	for (string::const_iterator iter = strUD.begin(); iter != strUD.end(); iter++)
		PushBack(*iter, strSubmit);
	return 0;
}

int C_SmsCoder::ScaDecode(const string &strSrc, string &strSCA)
{
	int num;
	string strNum;
	
	strNum.append(strSrc, 0, 2);
	sscanf(strNum.c_str(), "%x", &num);
	if (num == 0) return 0;
	strNum.append(strSrc, 2, 2);
	sscanf(strNum.c_str(), "%x", &num);
	m_SmsParam.m_ScaType = (S_BYTE)num;
	if (m_SmsParam.m_ScaType == SMS_SCA_TYPE_INTERNATIONAL)
		strSCA.push_back('+');
	for (num = 4; num < (int)strSrc.size(); num += 2)
	{
		strSCA.push_back(strSrc[num+1]);
		strSCA.push_back(strSrc[num+0]);
	}
	if (*(strSCA.end() - 1) == 'F' || *(strSCA.end() - 1) == 'f')
		strSCA.erase(strSCA.end() - 1);
	return 0;
}

int C_SmsCoder::OaDecode(const string &strSrc, string &strOA)
{
	int num;
	string strNum;
	
	strNum.append(strSrc, 0, 2);
	sscanf(strNum.c_str(), "%x", &num);
	if (num == 0) return -1;

	strNum.append(strSrc, 2, 2);
	sscanf(strNum.c_str(), "%x", &num);
	m_SmsParam.m_OADA_Type = (S_BYTE)num;

	if (m_SmsParam.m_OADA_Type == SMS_SCA_TYPE_INTERNATIONAL)
		strOA.push_back('+');
	for (num = 4; num < (int)strSrc.size(); num += 2)
	{
		strOA.push_back(strSrc[num+1]);
		strOA.push_back(strSrc[num+0]);
	}
	if (*(strOA.end() - 1) == 'F' || *(strOA.end() - 1) == 'f')
		strOA.erase(strOA.end() - 1);
	return 0;
}

int C_SmsCoder::SctsDecode(const string &strSrc, string &strSCTS)
{
	char sign[] = {'-','-',' ',':',':',' ',' ',' '};
	for (int i = 0, n = 0; i < (int)strSrc.size(); i += 2)
	{
		strSCTS.push_back(strSrc[i+1]);
		strSCTS.push_back(strSrc[i+0]);
		strSCTS.push_back(sign[n++]);
	}
	return 0;
}

int C_SmsCoder::UdDecode(const string &strSrc, string &strUD)
{
	if ((m_SmsParam.m_DCS & SMS_DCS_UCS2) == SMS_DCS_UCS2)
		return DcsUcs2Decode(strSrc, strUD);
	else if ((m_SmsParam.m_DCS & SMS_DCS_8BIT) == SMS_DCS_8BIT)
		return Dcs8BitDecode(strSrc, strUD);
	else if ((m_SmsParam.m_DCS & SMS_DCS_7BIT) == SMS_DCS_7BIT)
		return Dcs7BitDecode(strSrc, strUD);
	return -1;
}

int C_SmsCoder::DcsUcs2Decode(const string &strSrc, string &strUD)
{
	int UdLen = 0;
	string strLen(strSrc, 0, 2);
	sscanf(strLen.c_str(), "%x", &UdLen);
	if (UdLen == 0) return 0;
	if ((int)strSrc.size() < UdLen*2+2) return -1;

	for (int i = 2; (i+4) <= (UdLen*2+2); i += 4)
	{
		int UniCode = 0;
		string strUni(strSrc, i, 4);
		sscanf(strUni.c_str(), "%x", &UniCode);
		if (UniCode < 0x80)
		{
			strUD.push_back((char)UniCode);
		}
		else
		{
			INT16U GbCode = hai_Unicode2GB(UniCode);
			GbCode = (GbCode>>8) | (GbCode<<8);
			if (GbCode == 0) GbCode = 0x3F3F;
			strUD.append((char *)&GbCode, 2);
		}
	}
	return 0;
}

int C_SmsCoder::Dcs7BitDecode(const string &strSrc, string &strUD)
{
	int UdLen = 0;
	string strLen(strSrc, 0, 2);
	sscanf(strLen.c_str(), "%x", &UdLen);
	if (UdLen == 0) return 0;

	string strUdata;
	for (int i = 2; (i+2) <= (int)strSrc.size(); i += 2)
	{
		int OneByte = 0;
		string strData(strSrc, i, 2);
		sscanf(strData.c_str(), "%x", &OneByte);
		strUdata.push_back((char)OneByte);
	}

	C_BITCODE my7BitCoder(strUdata);
	for (int Cnt = 0; Cnt < UdLen; Cnt++)
	{
		S_BYTE Ascii = 0;
		if (my7BitCoder.GetBitData(Ascii, 7) == 0)
			strUD.push_back(Ascii);
	}

	return 0;
}

int C_SmsCoder::Dcs8BitDecode(const string &strSrc, string &strUD)
{
	int UdLen = 0;
	string strLen(strSrc, 0, 2);
	sscanf(strLen.c_str(), "%x", &UdLen);
	if (UdLen == 0) return 0;
	if ((int)strSrc.size() < UdLen*2+2) return -1;

	for (int i = 2; (i+2) <= (UdLen*2+2); i += 2)
	{
		int OneByte = 0;
		string strData(strSrc, i, 2);
		sscanf(strData.c_str(), "%x", &OneByte);
		strUD.push_back((char)OneByte);
	}
	return 0;
}

int C_SmsCoder::GbString2UnicodeString(const string &strGB, string &strUnicode)
{
	S_WORD GbCode, UniCode;
	for (string::const_iterator iter = strGB.begin(); *iter != 0 && iter != strGB.end(); )
	{
		if (*iter & 0x80)
		{
			GbCode = (((S_WORD)*iter)<<8)|((*(iter+1))&0xFF);
			if ((UniCode = hai_GB2Unicode(GbCode)) == 0x00)
				UniCode = '?';//????
			iter += 2;
		}
		else
		{
			UniCode = *iter;
			iter += 1;
		}
		UniCode = (UniCode>>8) | (UniCode<<8);
		strUnicode.append((char *)&UniCode, 2);
	}
	return 0;
}

int C_SmsCoder::PhoneNumberReverse(const string &strSRC, string &strDST)
{
	string strData(strSRC);
	if ((strData.size() % 2) == 1)
		strData.push_back('F');//add 'F' 
	for (size_t i = 0; i < strData.size(); i+=2)
	{
		strDST.push_back(strData[i+1]);
		strDST.push_back(strData[i+0]);
	}
	return 0;
}

const char SmsCode_AsciiHex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
int C_SmsCoder::PushBack(S_BYTE val, string &strData)
{
	strData.push_back(SmsCode_AsciiHex[(val>>4)&0x0F]);
	strData.push_back(SmsCode_AsciiHex[(val>>0)&0x0F]);
	return 0;
}

int C_SmsCoder::PushBackReverse(S_BYTE val, string &strData)
{
	strData.push_back(SmsCode_AsciiHex[(val>>0)&0x0F]);
	strData.push_back(SmsCode_AsciiHex[(val>>4)&0x0F]);
	return 0;
}

//
//
//
int C_BITCODE::GetBitData(S_BYTE &BitData, S_BYTE BitCnt)
{
	if (BitCnt > sizeof(S_BYTE) * 8)
		BitCnt = sizeof(S_BYTE) * 8;
	if (m_BitPos < BitCnt)
		FillBitBuffer();
	if (m_BitPos < BitCnt)
		return -1;

	BitData = (S_BYTE)m_BitBuffer;
	BitData <<= sizeof(S_BYTE) * 8 - BitCnt;
	BitData >>= sizeof(S_BYTE) * 8 - BitCnt;
	m_BitBuffer >>= BitCnt;
	m_BitPos -= BitCnt;

	return 0;
}

int C_BITCODE::SetBitData(S_BYTE BitData, S_BYTE BitCnt)
{
	if (BitCnt > sizeof(S_BYTE) * 8)
		BitCnt = sizeof(S_BYTE) * 8;
	if (sizeof(S_DWORD) * 8 - m_BitPos < BitCnt)
		StoryBitBuffer();

	BitData <<= sizeof(S_BYTE) * 8 - BitCnt;
	BitData >>= sizeof(S_BYTE) * 8 - BitCnt;
	m_BitBuffer |= ((S_DWORD)BitData & 0xFF) << m_BitPos;
	m_BitPos += BitCnt;

	return 0;
}

int C_BITCODE::GetAllBitCode(string &strBitCode, S_BYTE fill)
{
	if (m_BitPos != 0)
	{
		if (fill == 0x01) SetBitData(0xFF, 8 - m_BitPos % 8);
		else SetBitData(0x00, 8 - m_BitPos % 8);
		StoryBitBuffer();
	}
	strBitCode.append(m_strBitData);
	return 0;
}

int C_BITCODE::FillBitBuffer(void)
{
	while (m_BytePos < (int)m_strBitData.size())
	{
		if ((sizeof(S_DWORD) * 8 - m_BitPos) < sizeof(S_BYTE) * 8)
			return 0;
		S_DWORD val = (S_DWORD)m_strBitData[m_BytePos++] & 0xFF;
		val <<= m_BitPos;
		m_BitBuffer |= val;
		m_BitPos += sizeof(S_BYTE) * 8;
	}
	return -1;
}

int C_BITCODE::StoryBitBuffer(void)
{
	while (m_BitPos >= (int)sizeof(S_BYTE) * 8)
	{
		m_strBitData.push_back((char)m_BitBuffer);
		m_BitBuffer >>= sizeof(S_BYTE) * 8;
		m_BitPos -= sizeof(S_BYTE) * 8;
	}
	return 0;
}




