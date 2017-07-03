//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			modem.h
// Description:		modem class
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-10-19  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __MODEM_H__
#define __MODEM_H__

#include "haidef.h"
#include "channel.h"
#include "connect.h"
#include "sms.h"

enum
{
	MODEM_TYPE_UNKNOW,
	MODEM_TYPE_GPRS,
	MODEM_TYPE_CDMA
};

class C_MODEM
{
public:
	C_MODEM(S_CHANNEL &channel);
	virtual ~C_MODEM();

public:
	virtual int PowerOff(void);
	virtual int PowerOn(void);
	virtual int ModemTest(void);
	virtual int ModemInit(void);
	virtual int ModemManufacturer(string &strManufacturer);
	virtual int ModemModelCheck(string &strModel);
	virtual int SimCheck(string &strSimCard);
	virtual int SignalDetect(int &Signal);

	virtual int GetModemTestCmd(string &strAtCmd);
	virtual int GetModemInitCmd(int step, string &strAtCmd);
	virtual int GetModemManufacturerCmd(string &strAtCmd);
	virtual int GetModemModelCheckCmd(string &strAtCmd);
	virtual int GetSimCheckCmd(string &strAtCmd);
	virtual int GetSignalDetectCmd(string &strAtCmd);

public:
	virtual int InitSmsMode(void);
	virtual int PrepareSms_Text(void);
	virtual int PrepareSms_Pdu(void);
	virtual int SmsDelete(S_BOOL fRead, S_BOOL fAll);

	virtual int SmsSubmit_Text(const string &strSCA, const string &strDA, const string &strUD);
	virtual int SmsSubmit_PDU(const string &strSCA, const string &strDA, const string &strUD, const C_SMS &smsSet);
	virtual int SmsDeliver_PDU(string &strOA, string &strUD, string &strSCTS, C_SMS &MySms);
	virtual int ExtractSmsInfo(const string &strAllAnswer, string &strDeliver);

	virtual int GetSmsStoryCmd(string &strAtCmd, string &strParam);
	virtual int GetSmsModeCmd(string &strAtCmd, string &strParam);
	virtual int GetSmsCharSetCmd(string &strAtCmd, string &strParam);
	virtual int GetSmsSendCmd(string &strAtCmd, string &strParam);
	virtual int GetSmsReadCmd(string &strAtCmd, int SmsIdx);
	virtual int GetSmsListCmd(string &strAtCmd, int stat);
	virtual int GetSmsDeleteCmd(string &strAtCmd, int SmsIdx, int DelFlag);
	virtual int GetSmsInfoPrefix(string& strInfo);


public:
	virtual int ParseSignal(const string &strSignal, int &Signal);

public:
	int ModemComm(const string& strAtCmd, string &strAnswer, int WaitMS = 0);
	int ClearModemBuffer(void);
	S_BOOL ModemConnect(void);
	int ParseAnswerCommon(const string &strAnswer);
	int ParseSpecialString(const string &strAnswer, const string &strFind);
	int ParseSpecialString(const string &strAnswer, const char *pFind);
	int ExtractAnswerInfo(const string &strAnswer, string &strInfo);

protected:
	C_CONNECT *m_pConnect;
};

class C_GprsModem:public C_MODEM
{
public:
	C_GprsModem(S_CHANNEL &channel):C_MODEM(channel){}
	~C_GprsModem(){}
};

class C_ModemFact
{
public:
	static C_MODEM *CreateModem(int ModemType, S_CHANNEL &channel);
	static void DestroyModem(C_MODEM *pModem);
};

#endif //__MODEM_H__


