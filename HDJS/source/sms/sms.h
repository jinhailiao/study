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

//*****************************SMS PDU �������Ԫ��***********************************
//Ԫ�� ���� ���� ����
//SCA      Service Center Adress   1-12 �������ĵĵ绰����
//PDU-Type Protocol Data Unit Type 1    Э�����ݵ�Ԫ����
//MR       Message Reference       1    ���гɹ���SMS-SUBMIT�ο���Ŀ(0..255)
//OA       Originator Adress       2-12 ���ͷ�SME�ĵ�ַ
//DA       Destination Adress      2-12 ���շ�SME�ĵ�ַ
//PID      Protocol Identifier     1    ������ʾSMSC�Ժ��ַ�ʽ����SM (����FAX,��Voice��)
//DCS      Data Coding Scheme      1    ������ʾ�û�����(UD)����ʲô���뷽��
//SCTS     Service Center Time Stamp 7  ������ʾSMSC���յ���Ϣʱ��ʱ���
//VP       Validity Period         0,1,7 ������ʾ��Ϣ��SMSC�в�����Ч��ʱ��
//UDL      User Data Length        1     �û����ݶγ���
//UD       User Data               0-140 SM����
//***************************************************************************************

//**************************SCA ����Ϣ�������ĵ�ַ��ʽ***********************************
//�������ĵ�ַ�����������֣�
//SCA��ɣ�1-12����λλ��
//����0891683108200505F0
//
//SCA���� SCA���� SCA��ַ
//08      91      683108200505F0
//
//Len������Ϣ���ĵ�ַ���ȡ�ָ��91��+��68 31 08 20 05 05 F0���İ�λλ����Ŀ��
//���Len������Ϊ00&h�������ṩ����Ĳ��֣���ô�ն��豸����ȡSIM�����õ�SCA��䵽SMS-PUD�У�
//ͨ����AT+CSCA=xxxxx��ָ���������SIM���д洢��SCAֵ��
//Type������Ϣ���ĵ�ַ�����ͣ��ǹ��ʵĺ��뻹�ǹ��ڵĺ��루81&h��ʾ���ڣ�91&h��ʾ���ʵģ���
//91&h��TON/NPI����International/E.164��׼��ָ�ں���ǰ��ӡ�+���ţ����⻹��������ֵ����91&h��á�
//����91 &h = 1001 0001 &b
//
// 7 6 5 4    3 2 1 0
// 1 �������� �������
// 
//��7λ��Զ��Ϊ1
//��ֵ���ͣ�Type of Number����
//000��δ֪��001�����ʣ��� + ��ʼ�ĵ�ַ����010�����ڣ�111��������չ�� 
//�������Numbering plan identification����
//0000��δ֪��0001��ISDN/�绰����(E.164/E.163)��1111��������չ��
//Add������ISDN��ŷ�ʽ�Ķ���Ϣ���ĵ�ַ��
//
//ʾ����
//�������ĺ���    PDU��ʽ����
//+8613800250500  08 91 86 31 08 20 05 05 F0
//13800512500     07 81 31 08 50 21 05 F0
//123456          04 81 21 43 65
//*************************************************************************

//**********************PDU Type����һ����λλ�飩******************************
//PDUType��SMS-SUBMIT��SMS-DELIVER�ĵ�һ����λλ�顣���ֱ������漸��������ɣ�
//PDUType��ɣ�1����λλ��
//���ͷ���SMS-SUBMIT
//����31&h = 0011 0001&b
//Bit No. 7  6    5   4 3 2  1 0
//        RP UDHI SRR VPF RD MTI
//        0  0    1   1 0 0  0 1
//���շ���SMS-DELIVER
//����04&h = 0000 0100&b
//Bit No. 7  6    5   4 3 2   1 0
//        RP UDHI SRI     MMS MTI
//        0  0    0   0 0 1   0 0
//
//RP��      Ӧ��·����Reply Paht����
// 0 �C δ���ã�
// 1 �C ����
//UDHI��  �û�����ͷ��ʶ��User Data Header Indicator����
// 0 �C �û����ݣ�UD�����ֲ�����ͷ��Ϣ��
// 1 �C �û����ݣ�UD����ʼ���ְ����û�ͷ��Ϣ
//SRR��    ����״̬���棨Status Report Request����
// 0 �C ����Ҫ���棻
// 1 �C ��Ҫ����
//SRI��     ״̬����ָʾ��Status Report Indication������ֵ��������Ϣ�������ģ�SMSC������
// 0 �C ״̬���潫���᷵�ظ�����Ϣʵ�壨SME��
// 1 �C ״̬���潫���ظ�����Ϣʵ�壨SME��
//VPF��    ��Ч�ڸ�ʽ��Validity Period Format����
//00 �C VP ��û���ṩ������Ϊ 0 ����
//01 �C ������
//10 �C VP ����������ʽ�ṩ����Եģ���
//11 �C VP ����8λ���һ��(semi-octet)��ʽ�ṩ�����Եģ�
//RD��      �ܾ�������Reject Duplicate����
// 0 �C ֪ͨ ����Ϣ�������ģ�SMSC������һ��SMS-SUBMIT����ʹ����Ϣ����ǰ���ύ���ģ�����������SMSC��δ���ͳ�ȥ��MS�ظ��������ǣ���Ϣ�ο�(MR)�����շ���ַ(DA)�����ͷ���ַ(OA)��ͬ
// 1 �C ֪ͨ SMSC �ܾ�һ���ظ��� SMS
//MMS��   �и������Ϣ��Ҫ���ͣ�More Messages to Send������ֵ���� SMSC ����
// 0 �C �� SMSC ���и������Ϣ�ȴ� MS
// 1 �C �� SMSC ��û�и������Ϣ�ȴ� MS
//MTI��    ��Ϣ����ָʾ��Message Type Indicator��
//00 �C SMS-DELIVER��SMSC��MS���ֻ�֧��
//00 �C SMS-DELIVER REPORT��MS �� SMSC���ֻ�֧��
//���ֻ����յ� SMS-DELIVER ʱ�Զ�����
//01 �C SMS-SUBMIT��MS �� SMSC���ֻ�֧��
//01 �C SMS- SUBMIT REPORT��SMSC �� MS��
//10 �C SMS-STATUS REPORT ��SMSC �� MS��
//10 �C SMS-COMMAND��MS �� SMSC���ֻ�֧��
//11 �C ����
//ע�⣺PDUType��PDU�ĵ�һ����λλ�飬�������Ƿ������ĺ����ĵ�һ���ֽڣ�
//********************************************************************************

//*****************MR ��Ϣ�ο� (Message Reference)********************************
//��Ϊ 00 ����
//********************************************************************************

//*****************************DA/OA ���շ���ַ�뷢�ͷ���ַ****************************
//OA��DA��ʽ��һ���ģ�������ʾ��
//DA��ɣ�2-12����λλ��
//����0D91683158714209F8
//
//���� ���� ��ַ
//0D   91   683158714209F8
// 
//Len����ַ���ȡ�ָ8613851724908 �ĳ��ȡ�����SCA�еĶ��岻һ����
//Type������Ϣ���ĵ�ַ�����ͣ��ǹ��ʵĺ��뻹�ǹ��ڵĺ��루81&h��ʾ���ڣ�91&h��ʾ���ʵģ���
//Add������ISDN��ŷ�ʽ�Ķ���Ϣ���ĵ�ַ��
//
//ʾ����
//�绰����          PDU��ʽ����
//+8613851724908    0D 91 86 31 58 71 42 09 F8
//13851724908       0B 81 31 58 71 42 09 F8
//106051268812345*) 0F A1 01 06 15 62 88 21 43 F5
//123456            06 81 21 43 65
//*) С��ͨ����
//*************************************************************************************

//*************************PID Э���ʶ (Protocol-Identifier)**************************
//���ڱ�׼����µ��� MS-to-SC ����Ϣ���ͣ�ֻ������ PID Ϊ 00
//*************************************************************************************

//*****************DCS ���ݱ��뷽�� (DataCoding-Scheme)*******************
//Bit No. 7 6 5 4 3 2 1 0 ����
//ʾ����  0 0 0 0 0 0 0 0 = 00&h  7 bit ���ݱ��� Ĭ���ַ���
//        1 1 1 1 0 1 1 0 = F6&h  8 bit ���ݱ��� Class 1
//        0 0 0 0 1 0 0 0 = 08&h  USC2(16bit)˫�ֽ��ַ���
// 
//Bit No.7��Bit No.6��
//һ������Ϊ 00��
//Bit No.5��
// 0 �C �ı�δѹ����1 �C �ı���GSM��׼ѹ���㷨ѹ����
//Bit No.4��
// 0 �C ��ʾ Bit No.1��Bit No.0 Ϊ����λ��������Ϣ������Ϣ��
// 1 �C ��ʾ Bit No.1��Bit No.0 ������Ϣ������Ϣ��
//Bit No.3��Bit No.2��
//00 �C Ĭ�ϵ���ĸ��ÿ���ַ�ռ�� 7 ����λ����ʱ�����Դ��� 160 �ַ�
//01 �C 8bit����ʱ���ֻ�ܴ��� 140 ���ַ�
//10 �C USC2��16bit���� ����˫�ֽ��ַ���
//11 �C Ԥ����
//Bit No.1��Bit No.0��
//00 �C Class 0������Ϣֱ����ʾ����Ļ��
//01 �C Class 1��
//10 �C Class 2��SIM���ض���Ϣ����
//11 �C Class 3��
//ʾ����
//
//DSC �ַ���          ��ϢClass
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

//*****************VP ��Ϣ��Ч�ڣ�Validity Period��**************************
//��һ���������Եģ���     VPF = 10  VP = AA H  (����)
//�ڶ�����������Եģ���     VPF = 11
//
//�� �� �� ʱ �� �� ʱ��
//30 80 02 90 54 33 20
//��ʾ 03-08-20 09:45:33
//
//VP �������ͻ���8λ�飨semi-octet����ʽ�ṩ��
//��һ�������VPΪһ��8λ�飬������Ч�ڵĳ��ȣ�
//�� SMS-SUBMIT �� SMSC ���ܿ�ʼ���㡣
//�ڶ��������VPΪ�߸�8λ�飬������Ч����ֹ�ľ���ʱ�䡣
//
//�ڵ�һ������£���Ч�ڱ�ʾ��ʽ������ʾ��
//VP ��Ӧ����Ч��
//00 �C 8F (VF+1)*5 ����  �� 5 ���Ӽ���� 12 ��Сʱ
//90 �C A7 12 Сʱ + (VF �C 143)*30 ����
//A8 �C C4  (VP �C 166) * 1 ��
//C5 �C FF  (VP �C 192) * 1 ��
//
//�ڶ�������µ�ʱ����ʽ�� SCTS��Service Center TimeStamp����ʽһ�¡�
//*************************************************************************

//*********SCTS ��������ʱ�����Service Center TimeStamp��*****************
//ռ���߸�8λ�飬��ʽ�� VP �ĵڶ��������ʾ
//*************************************************************************

//********UDL �û����ݳ��� (User-Data-Length (Amount of Characters))*******
//UDL ��������ʽ�ṩ����ʾ�����û����ݶε����ݳ��ȡ�
//���ݸ�ʽ��3�֣� 7bit��8bit��16bit�� 
//7bit����GSM�ַ�����UDL��ʾ7bit���ַ�����
//8bit����ASCII�ַ�����UDL��ʾ�û����������ֽ���
//16bit����Unicode�ַ�����UDL��ʾ�û����������ֽ���
//
// 1 octet 0 .. 140 octets
// UDL     UD
// 05      E8 32 9B FD 06
// 
//�磺7bit���� "Test" 
//UDL = 04, UD = D4 F2 9C 0E
//���磺16bit���� "Test" 
//UDL = 08, UD = 00 54 00 65 00 73 00 74
//*************************************************************************

//
//PDU ����
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
//���ݱ��뷽��
#define SMS_DCS_GSM_COMPRESS		0x20
#define SMS_DCS_7BIT	0x00
#define SMS_DCS_8BIT	0x04
#define SMS_DCS_UCS2	0x08
//
//����Ϣ�������ĵ�ַ����
#define SMS_SCA_TYPE_INTERNATIONAL		0x91
#define SMS_SCA_TYPE_NATIONAL			0x81

class C_SMS
{
public:
	C_SMS(void);
	~C_SMS();

public:
	S_BYTE m_ScaType; ///< ����Ϣ�������ĵ�ַ����
	S_BYTE m_PduType; ///< PDU����
	S_BYTE m_MR; ///< ��Ϣ�ο�
	S_BYTE m_OADA_Type; ///< ���ͺ���������������
	S_BYTE m_PID; ///< PDU ID
	S_BYTE m_DCS; ///< ���ݱ��뷽��
	S_BYTE m_VP; ///< ��Ϣ��Ч��
	S_BYTE m_SCTS[7]; ///< ��������ʱ���
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
	 *  @brief ����Ϣ���뺯��
	 *  @param[in] strSCA ����Ϣ���ĺ��룬�磺13800000501
	 *  @param[in] strDA Ŀ���ַ���룬�磺13866668888
	 *  @param[in] strUD �û�����,�磺��ã�
	 *  @param[out] strSubmit ������ɵ��ַ���
	 *  @return PDU���ݳ���
	 */
	int SmsEncode(const string &strSCA, const string &strDA, const string &strUD, string &strSubmit);

	/**  @fn int SmsDecode(const string &strDeliver, string &strOA, string &strUD, string &strSCTS);
	 *  @brief ����Ϣ���뺯��
	 *  @param[in] strDeliver  ����Ϣ��
	 *  @param[in] strOA ���ͷ���ַ���룬�磺13866668888
	 *  @param[in] strUD �û�����,�磺��ã�
	 *  @param[out] strSCTS ��������ʱ���
	 *  @return strUD���ݳ���
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


