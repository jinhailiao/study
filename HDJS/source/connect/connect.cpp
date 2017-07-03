//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			connect.cpp
// Description:		connectino father
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-23  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "connect.h"
#include "timeop.h"
#include "htrace.h"
#include "server.h"
#include "servfact.h"
#include "appbase.h"

C_CONNECT::C_CONNECT(S_CHANNEL &channel)
{
	m_LastConnectTime = 0;
	m_Channel = channel;
}

C_CONNECT::~C_CONNECT()
{
}

S_BOOL C_CONNECT::Connect(void)
{
#define CONNECT_CNT_MAX		3
	S_BOOL ConnectOK;

	time_t CurTime = time(NULL);
	if (CurTime < m_LastConnectTime) m_LastConnectTime = CurTime;
	if (m_LastConnectTime > 0 && (CurTime-m_LastConnectTime < m_Channel.m_ConnectInterS))
		return false;
	m_LastConnectTime = CurTime;

	for (int ConnectCnt = 0; ConnectCnt < CONNECT_CNT_MAX; ConnectCnt++)
	{
		logit<<m_Channel.m_Name<<":Try to connect..."<<endl;
		if ((ConnectOK = OnConnect()) == true)
			break;
		else
			logit<<m_Channel.m_Name<<":Connect failed.timeout="<<m_Channel.m_ConnectTimeoutS<<" second(s)."<<endl;
 
		logit<<m_Channel.m_Name<<":Connect failed.Sleep for "<<m_Channel.m_ConnectInterS<<" second(s)."<<endl;
		C_SERVER* pServer = C_SERVFACT::GetCurServer();
		for (int n = 0; n <= m_Channel.m_ConnectInterS; n++)
		{
			pServer->HeartBeat();
			SleepSecond(1);
		}
	}
	if (ConnectOK = true)
		logit<<m_Channel.m_Name<<":Connect succeed."<<endl;

	return ConnectOK;
}

S_BOOL C_CONNECT::Disconnect(void)
{
	return OnDisconnect();
}

S_BOOL C_CONNECT::IsConnected(void)
{
	return ConnectSuccessful();
}

int C_CONNECT::SendData(const string &strData)
{
	if (strData.size() == 0)
		return 0;

	int size = (int)strData.size();
	int sendcnt = 0;

	C_MSTIMOUT ts(m_Channel.m_WriteTimeoutMS);
	while (size > sendcnt)
	{
		int cnt = WriteData((S_BYTE*)strData.data() + sendcnt, size-sendcnt);

		if (cnt > 0)
			WriteDataDelay(cnt);
		else if (cnt < 0)
			return -1;
		sendcnt += cnt;

		if (sendcnt >= size)
			break;//send ok
		if (ts.timout())
			break;
	}

	logit<<m_Channel.m_Name<<" send:"<<HEX<<strData<<endl;
	return sendcnt;
}

int C_CONNECT::RecvData(string &strData)
{
	int size = 0;
	S_BYTE RecvBuf[1024];
	C_MSTIMOUT ts(m_Channel.m_ReadTimeoutMS);

	while (1)
	{
		int count = ReadData(RecvBuf, sizeof(RecvBuf));
		if (count > 0)
		{
			strData.append((char *)RecvBuf, count);
			size += count;
		}
		else if (count < 0)
		{
			return -1;
		}
		else// if (count == 0)
		{
			if (size != 0) break;
			if (ts.timout()) break;
		}
	}
	if (size != 0)
		logit<<m_Channel.m_Name<<" recv:"<<HEX<<strData<<endl;
	return size;
}


