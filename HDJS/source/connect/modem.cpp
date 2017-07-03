//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			modem.cpp
// Description:		modem class
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-10-19  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "modem.h"
#include "connfact.h"
#include "htrace.h"

C_MODEM::C_MODEM(S_CHANNEL &channel)
{
	m_pConnect = C_CONNFACT::CreateConnection(channel);
}

C_MODEM::~C_MODEM()
{
	delete m_pConnect;
}

int C_MODEM::PowerOff(void)
{
	const int delayS = 30;
	extern int io_PowerOffGprsModem(int delayS);
	logit<<"Gprs Modem Power off, need about "<<delayS<<"'s......"<<endl;
	io_PowerOffGprsModem(delayS);

	return 0;
}

int C_MODEM::PowerOn(void)
{
	const int delayS = 30;
	extern int io_PowerOnGprsModem(int delayS);
	logit<<"Gprs Modem Power on, need about "<<delayS<<"'s......"<<endl;
	io_PowerOnGprsModem(delayS);

	return 0;
}

int C_MODEM::ModemTest(void)
{
	string strAtCmd, strAnswer;
	if (GetModemTestCmd(strAtCmd) != 0)
		return -1;

	for (int i = 0; i < 3; i++)
	{
		if (ModemComm(strAtCmd, strAnswer) == 0)
		{
			if (ParseAnswerCommon(strAnswer) == 0)
				return 0;
		}
		SleepMS(100);
	}
	return -1;
}

int C_MODEM::ModemInit(void)
{
	int step  = 0;
	string strAtCmd, strAnswer;

	while (GetModemInitCmd(++step, strAtCmd) == 0)
	{
		SleepMS(100);
		if (ModemComm(strAtCmd, strAnswer) == 0)
		{
			strAtCmd.erase(strAtCmd.end() - 1);
			if (ParseAnswerCommon(strAnswer) == 0)
				logit<<"Modem Cmd ("<<strAtCmd<<") init OK!!!"<<endl;
			else
				logwt<<"Modem Cmd ("<<strAtCmd<<") init Failed!!!"<<endl;
		}
		strAtCmd.clear(), strAnswer.clear();
	}

	return 0;
}

int C_MODEM::ModemManufacturer(string &strManufacturer)
{
	string strAtCmd, strAnswer;
	if (GetModemManufacturerCmd(strAtCmd) != 0)
		return -1;

	for (int i = 0; i < 3; i++)
	{
		if (ModemComm(strAtCmd, strAnswer) == 0)
		{
			if (ParseAnswerCommon(strAnswer) == 0)
				return ExtractAnswerInfo(strAnswer, strManufacturer);
		}
		SleepMS(100);
	}
	return -1;
}

int C_MODEM::ModemModelCheck(string &strModel)
{
	string strAtCmd, strAnswer;
	if (GetModemModelCheckCmd(strAtCmd) != 0)
		return -1;

	for (int i = 0; i < 3; i++)
	{
		if (ModemComm(strAtCmd, strAnswer) == 0)
		{
			if (ParseAnswerCommon(strAnswer) == 0)
				return ExtractAnswerInfo(strAnswer, strModel);
		}
		SleepMS(100);
	}
	return -1;
}

int C_MODEM::SimCheck(string &strSimCard)
{
	string strAtCmd, strAnswer;
	if (GetSimCheckCmd(strAtCmd) != 0)
		return -1;

	for (int i = 0; i < 3; i++)
	{
		if (ModemComm(strAtCmd, strAnswer) == 0)
		{
			if (ParseAnswerCommon(strAnswer) == 0)
				return ExtractAnswerInfo(strAnswer, strSimCard);
		}
		SleepMS(100);
	}
	return -1;
}

int C_MODEM::SignalDetect(int &Signal)
{
	string strAtCmd, strAnswer, strSignal;
	if (GetSignalDetectCmd(strAtCmd) != 0)
		return -1;

	for (int i = 0; i < 3; i++)
	{
		if (ModemComm(strAtCmd, strAnswer) == 0)
		{
			if ((ParseAnswerCommon(strAnswer) == 0) && (ExtractAnswerInfo(strAnswer, strSignal) == 0))
				return ParseSignal(strSignal, Signal);
		}
		SleepMS(100);
	}
	
	return -1;
}

int C_MODEM::GetModemTestCmd(string &strAtCmd)
{
	strAtCmd.append("AT\r\n");
	return 0;
}

int C_MODEM::GetModemInitCmd(int step, string &strAtCmd)
{
	if (step == 1)
		strAtCmd.append("ATE0\r");
	else if (step == 2)
		strAtCmd.append("AT+CMGF=0\r");
	else if (step == 3)
		strAtCmd.append("AT+CPMS=\"SM\",\"SM\",\"SM\"\r");
	else
		return -1;

	return 0;
}

int C_MODEM::GetModemManufacturerCmd(string &strAtCmd)
{
	strAtCmd.append("AT+CGMI\r");
	return 0;
}

int C_MODEM::GetModemModelCheckCmd(string &strAtCmd)
{
	strAtCmd.append("AT+CGMM\r");
	return 0;
}

int C_MODEM::GetSimCheckCmd(string &strAtCmd)
{
	strAtCmd.append("AT+CIMI\r");
	return 0;
}

int C_MODEM::GetSignalDetectCmd(string &strAtCmd)
{
	strAtCmd.append("AT+CSQ\r");
	return 0;
}

int C_MODEM::ParseSignal(const string &strSignal, int &Signal)
{
	string::size_type start = strSignal.find("+CSQ:");
	if (start == string::npos)
		return -1;

	string::size_type end = strSignal.find(",", start+5);
	if (end != string::npos)
	{
		string strVal(strSignal, start+5, end-(start+5));
		int nVal = atoi(strVal.c_str());
		if (nVal >= 99 || nVal <= 0)
			Signal = -113;
		else
			Signal = -113 + nVal * 2;
		return 0;
	}
	return -1;
}

int C_MODEM::InitSmsMode(void)
{
	string strAtCmd, strAnyData, strAnswer;
	strAnyData.append("\"SM\",\"SM\"");
	if (GetSmsStoryCmd(strAtCmd, strAnyData) != 0)
		return -1;
	if (ModemComm(strAtCmd, strAnswer) != 0)
		return -1;
	if (ParseAnswerCommon(strAnswer) != 0)
		return -1;

	return 0;
}

int C_MODEM::PrepareSms_Text(void)
{
	string strAtMode, strAtSet, strAnyData, strAnswer;
	strAnyData.assign("1");
	if (GetSmsModeCmd(strAtMode, strAnyData) != 0)
		return -1;
	if (ModemComm(strAtMode, strAnswer) != 0)
		return -1;
	if (ParseAnswerCommon(strAnswer) != 0)
		return -1;

	strAnswer.clear();
	strAnyData.assign("\"GSM\"");
	if (GetSmsCharSetCmd(strAtSet, strAnyData) != 0)
		return -1;
	if (ModemComm(strAtSet, strAnswer) != 0)
		return -1;
	if (ParseAnswerCommon(strAnswer) != 0)
		return -1;
	return 0;
}

int C_MODEM::PrepareSms_Pdu(void)
{
	string strAtMode, strAtSet, strAnyData, strAnswer;
	strAnyData.assign("0");
	if (GetSmsModeCmd(strAtMode, strAnyData) != 0)
		return -1;
	if (ModemComm(strAtMode, strAnswer) != 0)
		return -1;
	if (ParseAnswerCommon(strAnswer) != 0)
		return -1;

	strAnswer.clear();
	strAnyData.assign("\"UCS2\"");
	if (GetSmsCharSetCmd(strAtSet, strAnyData) != 0)
		return -1;
	if (ModemComm(strAtSet, strAnswer) != 0)
		return -1;
	if (ParseAnswerCommon(strAnswer) != 0)
		return -1;
	return 0;
}

int C_MODEM::SmsDelete(S_BOOL fRead, S_BOOL fAll)
{
	int DelFlag = 0;

	if (fAll == true) DelFlag = 4;
	else if (fRead == true) DelFlag = 1;

	string strAtCmd, strAnswer;
	if (GetSmsDeleteCmd(strAtCmd, 0, DelFlag) != 0)
		return -1;

	if (ModemComm(strAtCmd, strAnswer) != 0)
		return -1;
	if (ParseAnswerCommon(strAnswer) != 0)
		return -1;
	return 0;
}

int C_MODEM::SmsSubmit_Text(const string &strSCA, const string &strDA, const string &strUD)
{
	if (PrepareSms_Text() != 0)
		return -1;

	string strAtSend, strAnyData, strAnswer;
	strAnyData.assign("\"").append(strDA).append("\"");
	if (GetSmsSendCmd(strAtSend, strAnyData) != 0)
		return -1;

	if (ModemComm(strAtSend, strAnswer) != 0)
		return -1;
	if (ParseSpecialString(strAnswer, ">") != 0)
		return -1;

	strAnswer.clear();
	strAnyData.assign(strUD).push_back(0x1A);
	if (ModemComm(strAnyData, strAnswer) != 0)
		return -1;
	if (ParseAnswerCommon(strAnswer) != 0)
		return -1;
	return 0;
}

int C_MODEM::SmsSubmit_PDU(const string &strSCA, const string &strDA, const string &strUD, const C_SMS &smsSet)
{
	char buffer[12];
	string strSubmit;
	C_SmsCoder SmsCoder(smsSet);
	int PduLen = SmsCoder.SmsEncode(strSCA, strDA, strUD, strSubmit);
	if (PduLen <= 0)
	{
		logwt<<"SMS("<<strUD<<") code failed!"<<endl;
		return -1;
	}
	if (PrepareSms_Pdu() != 0)
		return -1;
	sprintf(buffer, "%d", PduLen);
	string strAtSend, strAnswer;
	string strAnyData(buffer);
	if (GetSmsSendCmd(strAtSend, strAnyData) != 0)
		return -1;

	if (ModemComm(strAtSend, strAnswer) != 0)
		return -1;
	if (ParseSpecialString(strAnswer, ">") != 0)
		return -1;

	strAnswer.clear();
	strSubmit.push_back(0x1A);
	if (ModemComm(strSubmit, strAnswer, 3000) != 0)
		return -1;
	if (ParseAnswerCommon(strAnswer) != 0)
		return -1;
	return 0;
}

//≤‚ ‘¬Î
//0891683108200005F0040D91683165303093F600000101036195542311C1A050D9BC52A14765981C269FD56D
//0891683108200005F0040D91683165303093F6000801019212607123044F60597D
int C_MODEM::SmsDeliver_PDU(string &strOA, string &strUD, string &strSCTS, C_SMS &MySms)
{
	if (PrepareSms_Pdu() != 0)
		return -1;

	string strAtCmd, strAnswer;
	if (GetSmsListCmd(strAtCmd, -1) != 0)
		return -1;
	if (ModemComm(strAtCmd, strAnswer, 6000) != 0)
		return -1;
	if (ParseAnswerCommon(strAnswer) != 0)
		return -1;
	string strDeliver;
	if (ExtractSmsInfo(strAnswer, strDeliver) != 0)
		return -1;
	C_SmsCoder SmsCoder;
	if (SmsCoder.SmsDecode(strDeliver, strOA, strUD, strSCTS) != 0)
	{
		logwt<<"SMS("<<strDeliver<<") decode failed!"<<endl;
		return -1;
	}
	SmsCoder.GetSmsCodeParam(MySms);
	return 0;
}

int C_MODEM::ExtractSmsInfo(const string &strAllAnswer, string &strDeliver)
{
	string::size_type start, end;
	string strPrefix;
	if (GetSmsInfoPrefix(strPrefix) != 0)
		return -1;
	if ((start = strAllAnswer.find(strPrefix)) == string::npos)
		return -1;
	if ((start = strAllAnswer.find("\x0D\x0A", start)) == string::npos)
		return -1;
	if ((end = strAllAnswer.find("\x0D\x0A", start+2)) == string::npos)
		return -1;
	strDeliver.append(strAllAnswer, start+2, end - (start + 2));
	return 0;
}

int C_MODEM::GetSmsStoryCmd(string &strAtCmd, string &strParam)
{
	if (strParam.empty())
		strParam.append("\"SM\",\"SM\",\"SM\"");
	strAtCmd.append("AT+CPMS=").append(strParam).append("\r");
	return 0;
}

int C_MODEM::GetSmsModeCmd(string &strAtCmd, string &strParam)
{
	strAtCmd.append("AT+CMGF=").append(strParam).append("\r");
	return 0;
}

int C_MODEM::GetSmsCharSetCmd(string &strAtCmd, string &strParam)
{
	strAtCmd.append("AT+CSCS=").append(strParam).append("\r");
	return 0;
}

int C_MODEM::GetSmsSendCmd(string &strAtCmd, string &strParam)
{
	strAtCmd.append("AT+CMGS=").append(strParam).append("\r");
	return 0;
}

int C_MODEM::GetSmsReadCmd(string &strAtCmd, int SmsIdx)
{
	char buffer[12];
	sprintf(buffer, "%d", SmsIdx);
	strAtCmd.append("AT+CMGR=").append(buffer).append("\r");
	return 0;
}

int C_MODEM::GetSmsListCmd(string &strAtCmd, int stat)
{
	char buffer[12];
	if (stat == -1) stat = 0;
	sprintf(buffer, "%d", stat);
	strAtCmd.append("AT+CMGL=").append(buffer).append("\r");
	return 0;
}

int C_MODEM::GetSmsDeleteCmd(string &strAtCmd, int SmsIdx, int DelFlag)
{
	char buffer[12];
	sprintf(buffer, "%d", SmsIdx);
	strAtCmd.append("AT+CMGD=").append(buffer);
	if (DelFlag != -1)
	{
		sprintf(buffer, "%d", SmsIdx);
		strAtCmd.append(",").append(buffer);
	}
	strAtCmd.append("\r");
	return 0;
}

int C_MODEM::GetSmsInfoPrefix(string& strInfo)
{
	strInfo.append("+CMGL:");
	return 0;
}

int C_MODEM::ModemComm(const string& strAtCmd, string &strAnswer, int WaitMS)
{
	if (ModemConnect() == false)
		return -1;

	for (int i = 0; i < 3; i++)
	{
		ClearModemBuffer();
		if (m_pConnect->SendData(strAtCmd) > 0)
		{
//			logit<<"ATCMD:"<<strAtCmd<<endl;
			if (WaitMS) SleepMS(WaitMS);
			if (m_pConnect->RecvData(strAnswer) > 0)
			{
//				logit<<"ANSWER:"<<strAnswer<<endl;
				return 0;
			}
		}
		SleepMS(100);
	}
	return -1;
}

int C_MODEM::ClearModemBuffer(void)
{
	if (ModemConnect() == false)
		return -1;
	m_pConnect->ClearInBuffer();
	m_pConnect->ClearOutBuffer();
	return 0;
}

int C_MODEM::ParseAnswerCommon(const string &strAnswer)
{
	if (strAnswer.find("\r\nOK\r\n") != string::npos)
		return 0;
	if (strAnswer.find("\r\nERROR\r\n") != string::npos)
		return -1;
	return -2;
}

int C_MODEM::ParseSpecialString(const string &strAnswer, const string &strFind)
{
	if (strFind.empty())
		return -1;

	if (strAnswer.find(strFind) != string::npos)
		return 0;
	return -1;
}

int C_MODEM::ParseSpecialString(const string &strAnswer, const char *pFind)
{
	if (pFind == NULL || *pFind == '\0')
		return -1;

	string strFind(pFind);
	return ParseSpecialString(strAnswer, strFind);
}

int C_MODEM::ExtractAnswerInfo(const string &strAnswer, string &strInfo)
{
	string::size_type start = strAnswer.find("\x0D\x0A");
	if (start == string::npos)
		return -1;

	string::size_type end = strAnswer.find("\x0D\x0A", start+2);
	if (end != string::npos)
	{
		string strMyAnswer(strAnswer, start+2, end-(start+2));
		start = strMyAnswer.find_first_not_of(' ');
		end  = strMyAnswer.find_last_not_of(' ');
		strInfo.append(strMyAnswer, start, end-start+1);
		return 0;
	}
	return -1;
}

S_BOOL C_MODEM::ModemConnect(void)
{
	if (m_pConnect == NULL)
		return false;
	if (m_pConnect->IsConnected())
		return true;

	return m_pConnect->Connect();
}



//
//
//
C_MODEM *C_ModemFact::CreateModem(int ModemType, S_CHANNEL &channel)
{
	switch (ModemType)
	{
	case MODEM_TYPE_GPRS:
		return new C_GprsModem(channel);
	default:
		break;
	}
	return NULL;
}

void C_ModemFact::DestroyModem(C_MODEM *pModem)
{
	delete pModem;
}



