//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			ppp.cpp
// Description:		ppp process
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-16  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "ppp.h"
#include "htrace.h"
#include "context.h"
#include "haicfg.h"
#include "sysipc.h"

S_PPPINFO::S_PPPINFO(S_SERVINFO *pInfo)
{
	memset(this, 0x00, sizeof(*this));
	m_pServInfo = pInfo;
}

S_PPPINFO::~S_PPPINFO()
{
}


C_PPP::C_PPP(void)
{
	m_pServInfo = &(CONTEXT.m_ServInfo[HDJS_SERID_PPP]);
	m_pPppInfo = &(CONTEXT.m_PppInfo);
	m_pModem = NULL;
}

C_PPP::~C_PPP()
{
}

int C_PPP::BeforeService(void)
{
	C_SERVER::SetCurrentServId(HDJS_SERID_PPP);
	C_HTRACE::SetSystemTraceSetting(&CONTEXT.m_TraceSet[HDJS_SERID_PPP]);
	return 0;
}

int C_PPP::Servicing(void)
{
	while (1)
	{
		HeartBeat();
		CheckStatus();
		if (PppOnline())
			OnlineServer();
		else
			ModemServer();
		SleepMS(100);
	}
	return 0;
}

int C_PPP::AfterService(void)
{
	return 0;
}

S_BOOL C_PPP::CheckStatus(void)
{
	return true;
}

S_BOOL C_PPP::PppOnline(void)
{
	return false;
}

int C_PPP::ModemServer(void)
{
	if (SetModemConnect() == false)
		return -1;

	ModemReset();
	HeartBeat();
	ModemCheck();
	HeartBeat();
	ModemRunInfo();

	HeartBeat();
	InitSmsMode();

	while (1)
	{
		int SerID = HDJS_SERID_UIMAN;
		S_BYTE MsgT = C_SYSIPC::SYSIPC_TYPE_MSG_SEND;
		string msg;
		if ((C_SYSIPC::Query4Server(SerID, MsgT, msg) >= 0) && MsgT == C_SYSIPC::SYSIPC_TYPE_MSG_SEND)
		{
			string strSCA, strDA, strUD;
			strDA.append(msg.begin(), msg.begin()+11);
			strUD.assign(msg.begin()+11, msg.end());
			SmsSubmit(strSCA, strDA, strUD, false);
		}
		SleepSecond(2);
	
		string strOA, strMyUD, strSCTS;
		if (SmsDeliver(strOA, strMyUD, strSCTS) == 0)
		{
			SmsDelete(true, true);
			msg.assign(strOA).append(strMyUD);
			C_SYSIPC::Send2Server(HDJS_SERID_UIMAN, C_SYSIPC::SYSIPC_TYPE_MSG_SEND, msg);
		}
		SleepSecond(2);
		HeartBeat();
	}

	ClrModemConnect();
	return 0;
}

int C_PPP::OnlineServer(void)
{
	return 0;
}

int C_PPP::DetectModemType(void)
{
//	const int delayS = 10;
//	for (int cnt = 0; cnt < MODEM_TYPE_CHECK_SECONDS/delayS; cnt++)
//	{
//		if (checksuccessful)
//			break;
//		SleepSecond(delayS);
//		HeartBeat();
//	}
	logit<<"Detect Modem: GPRS."<<endl;
	return MODEM_TYPE_GPRS;
}

S_CHANNEL* C_PPP::GetModemChannel(void)
{
	return C_ContextOP::FindUpChannel(m_pPppInfo->m_PppSet.m_ChannlName);
}

S_BOOL C_PPP::SetModemConnect(void)
{
	if (m_pModem != NULL)
		return true;

	int ModemType = DetectModemType();
	if (ModemType == MODEM_TYPE_UNKNOW)
	{
		loget<<"Modem type detect failed!!!"<<endl;
		return false;
	}
	S_CHANNEL *pModemChannel = GetModemChannel();
	if (pModemChannel == NULL)
	{
		loget<<"Get Modem Channel failed!!!"<<endl;
		return false;
	}
	m_pModem = C_ModemFact::CreateModem(ModemType, *pModemChannel);

	return (m_pModem != NULL);
}

S_BOOL C_PPP::ClrModemConnect(void)
{
	if (m_pModem != NULL)
		C_ModemFact::DestroyModem(m_pModem);
	m_pModem = NULL;
	return true;
}

int C_PPP::ModemReset(void)
{
	if (m_pModem == NULL)
		return -1;
#ifndef WIN32
	m_pModem->PowerOff();
	m_pModem->PowerOn();
#endif
	return 0;
}

int C_PPP::ModemCheck(void)
{
	if (m_pModem == NULL)
		return -1;
	
	if (m_pModem->ModemTest() == 0)
		logit<<"MODEM 测试 OK！"<<endl;
	else
		logwt<<"MODEM 测试失败！"<<endl;

	m_pModem->ModemInit();

	return 0;
}

int C_PPP::ModemRunInfo(void)
{
	if (m_pModem == NULL)
		return -1;
	int Signal = -113;
	string strManufacturer, strModel, strSimCard;

	if (m_pModem->ModemManufacturer(strManufacturer) == 0)
		logit<<"MODEM厂家信息:"<<strManufacturer<<endl;
	else
		logwt<<"查询MODEM厂家信息失败！"<<endl;

	if (m_pModem->ModemModelCheck(strModel) == 0)
		logit<<"MODEM型号信息:"<<strModel<<endl;
	else
		logwt<<"查询MODEM型号信息失败！"<<endl;

	if (m_pModem->SimCheck(strSimCard) == 0)
		logit<<"SIM卡信息:"<<strSimCard<<endl;
	else
		logwt<<"查询SIM卡信息失败！"<<endl;

	for (int i = 0; i < 3; i++)
	{
		if (m_pModem->SignalDetect(Signal) == 0)
		{
			logit<<"当前网络信号:"<<Signal<<endl;
			if (Signal != -113)
				break;
		}
		logwt<<"查询网络信号失败！"<<endl;
		HeartBeat(), SleepSecond(10);
	}
	return 0;
}

int C_PPP::InitSmsMode(void)
{
	if (m_pModem == NULL)
		return -1;
	return m_pModem->InitSmsMode();
}

int C_PPP::SmsSubmit(const string &strSCA, const string &strDA, const string &strUD, S_BOOL fTextMode)
{
	int ok;
	if (m_pModem == NULL)
		return -1;
	if (strDA.empty() || strUD.empty())
		return -1;

	if (fTextMode == true)
	{
		ok = m_pModem->SmsSubmit_Text(strSCA, strDA, strUD);
		if (ok == 0)
			logit<<"SMS("<<strUD<<") submit successfully."<<endl;
		else
			logwt<<"SMS("<<strUD<<") submit failed."<<endl;
		return ok;
	}
	
	C_SMS MySms;
	ok = m_pModem->SmsSubmit_PDU(strSCA, strDA, strUD, MySms);
	if (ok == 0)
		logit<<"SMS("<<strUD<<") submit successfully."<<endl;
	else
		logwt<<"SMS("<<strUD<<") submit failed."<<endl;
	return ok;
}

int C_PPP::SmsDeliver(string &strOA, string &strUD, string &strSCTS)
{
	int ok;
	if (m_pModem == NULL)
		return -1;

	C_SMS MySms;
	if ((ok = m_pModem->SmsDeliver_PDU(strOA, strUD, strSCTS, MySms)) == 0)
	{
		logit<<"OA:"<<strOA<<endl;
		logit<<"SCTS:"<<strSCTS<<endl;
		logit<<"SMS("<<strUD<<") SmsDeliver successfully."<<endl;
	}
	return ok;
}

int C_PPP::SmsDelete(S_BOOL fRead, S_BOOL fAll)
{
	if (m_pModem == NULL)
		return -1;
	return m_pModem->SmsDelete(fRead, fAll);	
}



