//---------------------------------------------------------------------------------
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			sms.h
// Description:		sms pdu decode/encode
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-10-26  create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __HDJS_SMS_H__
#define __HDJS_SMS_H__

#include "haidef.h"

//*****************************SMS PDU 基本组成元素***********************************
//元素 名称 长度 描述
//SCA      Service Center Adress   1-12 服务中心的电话号码
//PDU-Type Protocol Data Unit Type 1    协议数据单元类型
//MR       Message Reference       1    所有成功的SMS-SUBMIT参考数目(0..255)
//OA       Originator Adress       2-12 发送方SME的地址
//DA       Destination Adress      2-12 接收方SME的地址
//PID      Protocol Identifier     1    参数显示SMSC以何种方式处理SM (比如FAX,、Voice等)
//DCS      Data Coding Scheme      1    参数表示用户数据(UD)采用什么编码方案
//SCTS     Service Center Time Stamp 7  参数表示SMSC接收到消息时的时间戳
//VP       Validity Period         0,1,7 参数表示消息在SMSC中不再有效的时长
//UDL      User Data Length        1     用户数据段长度
//UD       User Data               0-140 SM数据
//***************************************************************************************

//**************************SCA 短消息服务中心地址格式***********************************
//服务中心地址包含三个部分：
//SCA组成：1-12个八位位组
//例：0891683108200505F0
//
//SCA长度 SCA类型 SCA地址
//08      91      683108200505F0
//
//Len：短消息中心地址长度。指（91）+（68 31 08 20 05 05 F0）的八位位组数目。
//如果Len被设置为00&h，并不提供后面的部分，那么终端设备将读取SIM中设置的SCA填充到SMS-PUD中，
//通过“AT+CSCA=xxxxx”指令可以设置SIM卡中存储的SCA值。
//Type：短消息中心地址的类型，是国际的号码还是国内的号码（81&h表示国内，91&h表示国际的）。
//91&h是TON/NPI遵守International/E.164标准，指在号码前需加‘+’号；此外还有其它数值，但91&h最常用。
//例：91 &h = 1001 0001 &b
//
// 7 6 5 4    3 2 1 0
// 1 数据类型 号码鉴别
// 
//第7位永远置为1
//数值类型（Type of Number）：
//000—未知，001—国际（以 + 开始的地址），010—国内，111—留作扩展； 
//号码鉴别（Numbering plan identification）：
//0000—未知，0001—ISDN/电话号码(E.164/E.163)，1111—留作扩展；
//Add：依照ISDN编号方式的短消息中心地址。
//
//示例：
//服务中心号码    PDU格式编码
//+8613800250500  08 91 86 31 08 20 05 05 F0
//13800512500     07 81 31 08 50 21 05 F0
//123456          04 81 21 43 65
//*************************************************************************

//**********************PDU Type（第一个八位位组）******************************
//PDUType是SMS-SUBMIT、SMS-DELIVER的第一个八位位组。它分别由下面几个部分组成：
//PDUType组成：1个八位位组
//发送方：SMS-SUBMIT
//例：31&h = 0011 0001&b
//Bit No. 7  6    5   4 3 2  1 0
//        RP UDHI SRR VPF RD MTI
//        0  0    1   1 0 0  0 1
//接收方：SMS-DELIVER
//例：04&h = 0000 0100&b
//Bit No. 7  6    5   4 3 2   1 0
//        RP UDHI SRI     MMS MTI
//        0  0    0   0 0 1   0 0
//
//RP：      应答路径（Reply Paht），
// 0 – 未设置；
// 1 – 设置
//UDHI：  用户数据头标识（User Data Header Indicator），
// 0 – 用户数据（UD）部分不包含头信息；
// 1 – 用户数据（UD）开始部分包含用户头信息
//SRR：    请求状态报告（Status Report Request），
// 0 – 不需要报告；
// 1 – 需要报告
//SRI：     状态报告指示（Status Report Indication），此值仅被短消息服务中心（SMSC）设置
// 0 – 状态报告将不会返回给短消息实体（SME）
// 1 – 状态报告将返回给短消息实体（SME）
//VPF：    有效期格式（Validity Period Format），
//00 – VP 段没有提供（长度为 0 ），
//01 – 保留，
//10 – VP 段以整型形式提供（相对的），
//11 – VP 段以8位组的一半(semi-octet)形式提供（绝对的）
//RD：      拒绝复本（Reject Duplicate），
// 0 – 通知 短消息服务中心（SMSC）接受一个SMS-SUBMIT，即使该消息是先前已提交过的，并还存在于SMSC中未发送出去。MS重复的条件是：消息参考(MR)、接收方地址(DA)及发送方地址(OA)相同
// 1 – 通知 SMSC 拒绝一个重复的 SMS
//MMS：   有更多的信息需要发送（More Messages to Send），此值仅被 SMSC 设置
// 0 – 在 SMSC 中有更多的信息等待 MS
// 1 – 在 SMSC 中没有更多的信息等待 MS
//MTI：    信息类型指示（Message Type Indicator）
//00 – SMS-DELIVER（SMSCèMS）手机支持
//00 – SMS-DELIVER REPORT（MS è SMSC）手机支持
//当手机接收到 SMS-DELIVER 时自动产生
//01 – SMS-SUBMIT（MS è SMSC）手机支持
//01 – SMS- SUBMIT REPORT（SMSC è MS）
//10 – SMS-STATUS REPORT （SMSC è MS）
//10 – SMS-COMMAND（MS è SMSC）手机支持
//11 – 保留
//注意：PDUType是PDU的第一个八位位组，而并不是服务中心号码后的第一个字节！
//********************************************************************************

//*****************MR 信息参考 (Message Reference)********************************
//设为 00 即可
//********************************************************************************

//*****************************DA/OA 接收方地址与发送方地址****************************
//OA与DA格式是一样的，如下所示：
//DA组成：2-12个八位位组
//例：0D91683158714209F8
//
//长度 类型 地址
//0D   91   683158714209F8
// 
//Len：地址长度。指8613851724908 的长度。这与SCA中的定义不一样！
//Type：短消息中心地址的类型，是国际的号码还是国内的号码（81&h表示国内，91&h表示国际的）。
//Add：依照ISDN编号方式的短消息中心地址。
//
//示例：
//电话号码          PDU格式编码
//+8613851724908    0D 91 86 31 58 71 42 09 F8
//13851724908       0B 81 31 58 71 42 09 F8
//106051268812345*) 0F A1 01 06 15 62 88 21 43 F5
//123456            06 81 21 43 65
//*) 小灵通号码
//*************************************************************************************

//*************************PID 协议标识 (Protocol-Identifier)**************************
//对于标准情况下的下 MS-to-SC 短消息传送，只需设置 PID 为 00
//*************************************************************************************

//*****************DCS 数据编码方案 (DataCoding-Scheme)*******************
//Bit No. 7 6 5 4 3 2 1 0 描述
//示例：  0 0 0 0 0 0 0 0 = 00&h  7 bit 数据编码 默认字符集
//        1 1 1 1 0 1 1 0 = F6&h  8 bit 数据编码 Class 1
//        0 0 0 0 1 0 0 0 = 08&h  USC2(16bit)双字节字符集
// 
//Bit No.7与Bit No.6：
//一般设置为 00；
//Bit No.5：
// 0 – 文本未压缩，1 – 文本用GSM标准压缩算法压缩；
//Bit No.4：
// 0 – 表示 Bit No.1、Bit No.0 为保留位，不含信息类型信息，
// 1 – 表示 Bit No.1、Bit No.0 含有信息类型信息；
//Bit No.3与Bit No.2：
//00 – 默认的字母表，每个字符占用 7 比特位，此时最大可以传送 160 字符
//01 – 8bit，此时最大只能传送 140 个字符
//10 – USC2（16bit）， 传送双字节字符集
//11 – 预留；
//Bit No.1与Bit No.0：
//00 – Class 0，短消息直接显示在屏幕上
//01 – Class 1，
//10 – Class 2（SIM卡特定信息），
//11 – Class 3。
//示例：
//
//DSC 字符集          信息Class
//00  default (7-bit) No Class
//F0  default (7-bit) Class 0 (immediate display)
//F1  default (7-bit) Class 1 (Mobile Equipment- specific)
//F2  default (7-bit) Class 2 (SIM specific message)
//F3  default (7-bit) Class 3 (Class3 Terminate Equipment- specific)
//F4  8-bit           Class 0 (immediate display)
//F5  8-bit           Class 1 (Mobile Equipment- specific)
//F6  8-bit           Class 2 (SIM specific message)
//F7  8-bit           Class 3 (Class3 Terminate Equipment- specific)
//08  16-bit          No Class
//18  16-bit          Class 0 (immediate display)
//*************************************************************************** 

//*****************VP 信息有效期（Validity Period）**************************
//第一种情况（相对的）：     VPF = 10  VP = AA H  (四天)
//第二种情况（绝对的）：     VPF = 11
//
//年 月 日 时 分 秒 时区
//30 80 02 90 54 33 20
//表示 03-08-20 09:45:33
//
//VP 段以整型或半个8位组（semi-octet）形式提供。
//第一种情况，VP为一个8位组，给定有效期的长度，
//从 SMS-SUBMIT 被 SMSC 接受开始计算。
//第二种情况，VP为七个8位组，给定有效期终止的绝对时间。
//
//在第一种情况下，有效期表示格式如下所示：
//VP 相应的有效期
//00 – 8F (VF+1)*5 分钟  从 5 分钟间隔到 12 个小时
//90 – A7 12 小时 + (VF – 143)*30 分钟
//A8 – C4  (VP – 166) * 1 天
//C5 – FF  (VP – 192) * 1 周
//
//第二种情况下的时间形式与 SCTS（Service Center TimeStamp）形式一致。
//*************************************************************************

//*********SCTS 服务中心时间戳（Service Center TimeStamp）*****************
//占用七个8位组，格式如 VP 的第二种情况所示
//*************************************************************************

//********UDL 用户数据长度 (User-Data-Length (Amount of Characters))*******
//UDL 以整型形式提供，表示后面用户数据段的数据长度。
//数据格式有3种： 7bit，8bit，16bit。 
//7bit采用GSM字符集，UDL表示7bit的字符个数
//8bit采用ASCII字符集，UDL表示用户数据区的字节数
//16bit采用Unicode字符集。UDL表示用户数据区的字节数
//
// 1 octet 0 .. 140 octets
// UDL     UD
// 05      E8 32 9B FD 06
// 
//如：7bit数据 "Test" 
//UDL = 04, UD = D4 F2 9C 0E
//又如：16bit数据 "Test" 
//UDL = 08, UD = 00 54 00 65 00 73 00 74
//*************************************************************************

//
//PDU 类型
//common
#define SMS_PDU_TYPE_RP		0x80
#define SMS_PDU_TYPE_UDHI		0x40
//submit
#define SMS_PDU_TYPE_SRR		0x20
#define SMS_PDU_TYPE_VPF_NO			0x00
#define SMS_PDU_TYPE_VPF_RESERVE		0x08
#define SMS_PDU_TYPE_VPF_RELATIVE		0x10
#define SMS_PDU_TYPE_VPF_ABSOLUTE	0x18
#define SMS_PDU_TYPE_RD		0x04
#define SMS_PDU_TYPE_SUBMIT	0x01
//deliver
#define SMS_PDU_TYPE_SRI		0x20
#define SMS_PDU_TYPE_MMS		0x04
#define SMS_PDU_TYPE_DELIVER	0x00
//
//数据编码方案
#define SMS_DCS_GSM_COMPRESS		0x20
#define SMS_DCS_7BIT	0x00
#define SMS_DCS_8BIT	0x04
#define SMS_DCS_UCS2	0x08
//
//短消息服务中心地址类型
#define SMS_SCA_TYPE_INTERNATIONAL		0x91
#define SMS_SCA_TYPE_NATIONAL			0x81

class C_SMS
{
public:
	C_SMS(void);
	~C_SMS();

public:
	S_BYTE m_ScaType; ///< 短消息服务中心地址类型
	S_BYTE m_PduType; ///< PDU类型
	S_BYTE m_MR; ///< 消息参考
	S_BYTE m_OADA_Type; ///< 发送号码和输出号码类型
	S_BYTE m_PID; ///< PDU ID
	S_BYTE m_DCS; ///< 数据编码方案
	S_BYTE m_VP; ///< 信息有效期
	S_BYTE m_SCTS[7]; ///< 服务中心时间戳
};

class C_SmsCoder
{
public:
	C_SmsCoder(void){}
	C_SmsCoder(const C_SMS &SmsParam){SetSmsCodeParam(SmsParam);}
	~C_SmsCoder(){}

	void SetSmsCodeParam(const C_SMS &SmsParam){m_SmsParam=SmsParam;}
	void GetSmsCodeParam(C_SMS &SmsParam){SmsParam=m_SmsParam;}

	/**  @fn int C_SmsCoder::SmsCode(const string &strSCA, const string &strDA, const string &strUD, string &strSubmit)
	 *  @brief 短消息编码函数
	 *  @param[in] strSCA 短消息中心号码，如：13800000501
	 *  @param[in] strDA 目标地址号码，如：13866668888
	 *  @param[in] strUD 用户数据,如：你好！
	 *  @param[out] strSubmit 编码完成的字符串
	 *  @return PDU数据长度
	 */
	int SmsEncode(const string &strSCA, const string &strDA, const string &strUD, string &strSubmit);

	/**  @fn int SmsDecode(const string &strDeliver, string &strOA, string &strUD, string &strSCTS);
	 *  @brief 短消息解码函数
	 *  @param[in] strDeliver  短消息串
	 *  @param[in] strOA 发送方地址号码，如：13866668888
	 *  @param[in] strUD 用户数据,如：你好！
	 *  @param[out] strSCTS 服务中心时间戳
	 *  @return strUD数据长度
	 */
	int SmsDecode(const string &strDeliver, string &strOA, string &strUD, string &strSCTS);

private:
	int ScaEncode(const string &strSCA, string &strSubmit);
	int DaEncode(const string &strDA, string &strSubmit);
	int UdEncode(const string &strUD, string &strSubmit);
	int DcsUcs2Encode(const string &strUD, string &strSubmit);
	int Dcs7BitEncode(const string &strUD, string &strSubmit);
	int Dcs8BitEncode(const string &strUD, string &strSubmit);

	int ScaDecode(const string &strSrc, string &strSCA);
	int OaDecode(const string &strSrc, string &strOA);
	int SctsDecode(const string &strSrc, string &strSCTS);
	int UdDecode(const string &strSrc, string &strUD);
	int DcsUcs2Decode(const string &strSrc, string &strUD);
	int Dcs7BitDecode(const string &strSrc, string &strUD);
	int Dcs8BitDecode(const string &strSrc, string &strUD);


	int GbString2UnicodeString(const string &strGB, string &strUnicode);
	int PhoneNumberReverse(const string &strSRC, string &strDST);
	int PushBack(S_BYTE val, string &strData);
	int PushBackReverse(S_BYTE val, string &strData);


private:
	C_SMS m_SmsParam;
};

class C_BITCODE
{
public:
	C_BITCODE(void){m_BitPos=0;m_BytePos=0;m_BitBuffer=0;}
	C_BITCODE(string &strData){m_strBitData = strData;m_BitPos=0;m_BytePos=0;m_BitBuffer=0;}
	~C_BITCODE(){}

public:
	int GetBitData(S_BYTE &BitData, S_BYTE BitCnt = 7);
	int SetBitData(S_BYTE BitData, S_BYTE BitCnt = 7);
	int GetAllBitCode(string &strBitCode, S_BYTE fill = 1);

private:
	int FillBitBuffer(void);
	int StoryBitBuffer(void);

private:
	int m_BitPos;
	int m_BytePos;
	S_DWORD m_BitBuffer;
	string m_strBitData;
};

#endif //__HDJS_SMS_H__


