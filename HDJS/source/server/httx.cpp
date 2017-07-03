//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			httx.cpp
// Description:		master station communication
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-16  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "httx.h"
#include "htrace.h"
#include "context.h"
#include "haicfg.h"
#include "sysipc.h"
#include "connfact.h"
#include "msrule.h"
#include "mscmd.h"
#include "msfact.h"

#define REPORT_NAME_LOGIN	"Login"
#define REPORT_NAME_HBEAT	"HeartBeat"


int S_FORWARD::SetForwardedAddr(int idx, const char *pStrAddr)
{//TODO:ADDR:BCD+HEX
	if (idx < 0 || idx >= FORWARD_ADDR_MAX || strlen(pStrAddr) < 8)
		return -1;
	m_ForwardedAddr[idx][0] = ((pStrAddr[0]-'0') << 4) + ((pStrAddr[1]-'0') & 0x0F);
	m_ForwardedAddr[idx][1] = ((pStrAddr[2]-'0') << 4) + ((pStrAddr[3]-'0') & 0x0F);
	m_ForwardedAddr[idx][2] = ((pStrAddr[4]-'0') << 4) + ((pStrAddr[5]-'0') & 0x0F);
	m_ForwardedAddr[idx][3] = ((pStrAddr[6]-'0') << 4) + ((pStrAddr[7]-'0') & 0x0F);
	return 0;
}

char *S_FORWARD::GetForwardedAddrStr(int idx, char *pStrAddr)
{//TODO:ADDR:BCD+HEX
	if (idx < 0 || idx >= FORWARD_ADDR_MAX || pStrAddr == NULL)
		return NULL;

	for (int i = 0; i < 4; i++)
	{
		pStrAddr[i*2+0] = ((m_ForwardedAddr[idx][i]>>4)&0x0F) + '0';
		pStrAddr[i*2+1] = ((m_ForwardedAddr[idx][i]>>0)&0x0F) + '0';
	}
	return pStrAddr;
}

//
//
//
S_HTTXINFO::S_HTTXINFO(S_SERVINFO *pInfo)
{
	memset(this, 0x00, sizeof(*this));
	m_pServInfo = pInfo;
}

S_HTTXINFO::~S_HTTXINFO()
{
}

//
//
//
int C_HTTX::BeforeService(void)
{
	return 0;
}

int C_HTTX::Servicing(void)
{
	while (1)
	{
		HeartBeat();
		ServiceProc();
	}
	return 0;
}

int C_HTTX::ServiceProc(void)
{
	m_pFConnect = NULL;

	for (size_t i = 0; i < HTTX_CHANNEL_MAX-1; i++)/** @note 最后一个通道是级联 */
	{
		char *pChannlName;
		if (CONTEXT.m_forward.m_MasterForwarder == false && m_pHttxInfo->m_ChannelName[HTTX_CHANNEL_MAX-1][0])/** 从集中器 */
			pChannlName = m_pHttxInfo->m_ChannelName[HTTX_CHANNEL_MAX-1];/** 级联通道 */
		else
			pChannlName = m_pHttxInfo->m_ChannelName[i]; /** 主站通道 */

		HeartBeat();
		/** @brief 查找上行通道 */
		if ((m_pChannel = C_ContextOP::FindUpChannel(pChannlName)) == NULL)
		{
			if (*pChannlName != '\0')
				logwt<<"Find channel("<<pChannlName<<") failed!!!"<<endl;
			SleepSecond(1);
			continue;
		}

		if ((m_pMConnect = C_CONNFACT::CreateConnection(*m_pChannel)) == NULL)
		{
			logwt<<"Create connection failed for channel "<<m_pChannel->m_Name<<endl;;
			SleepSecond(1);
			continue;
		}

		ChannelService();

		C_CONNFACT::DestroyConnection(m_pMConnect);

		SleepSecond(1);
	}

	return 0;
}

int C_HTTX::ChannelService(void)
{
	/** @brief MSRULE initialization
	 */
	InitMsruleInformation();

	/** @brief Request dial
	 */
	CONTEXT.m_ConnectStat.m_HttxNeedPPP[m_HttxID] = false;
	if (m_pChannel->NeedPPP() == true)
	{
		//FlowCtrl
		logvt<<"Waiting PPP dial......"<<endl;
		CONTEXT.m_ConnectStat.m_HttxNeedPPP[m_HttxID] = true;
		int timeoutS = 0;
		while (CONTEXT.m_ConnectStat.m_PppOnline == false)
		{
			HeartBeat(), SleepSecond(1);
			if ((++timeoutS % 120) == 0) logit<<"Keep wait......"<<endl;
		}
		logit<<"PPP dial successfully!"<<endl;
	}

	//flowCtrl
	if (m_pMConnect->Connect() == false)
	{
		SleepSecond(2);
		return -1;
	}

	//flowCtrl
	if (NeedLogin()) MakeLogin();

	while (m_pMConnect->IsConnected())
	{
		string strCmd;

		HeartBeat();
		if (CmdGet(strCmd) > 0)
		{
			int CmdType = CmdCheck(strCmd);
			if (CmdType == CMDTYPE_ADDR_LOCAL || CmdType == CMDTYPE_ADDR_BROADCAST)
				CmdLocalProc(strCmd);
			if (CmdType == CMDTYPE_ADDR_SLAVE || CmdType == CMDTYPE_ADDR_BROADCAST)
				CmdSlaveProc(strCmd);
			if (CmdType == CMDTYPE_ADDR_RELAY || CmdType == CMDTYPE_ADDR_BROADCAST)
				CmdRelayProc(strCmd);
		}
		else if (HeartBeatTime())
		{
			MakeHeartBeat();
		}
/*		else if (Class3report)
		{
		}
		else if (class2report)
		{
		}
		else if (class1report)
		{
		}
		else if (checkslave)
		{
		}
*/
		ResponseSend();
		ReportSend();
	}

	return 0;
}

int C_HTTX::AfterService(void)
{
	return 0;
}

int C_HTTX::InitMsruleInformation(void)
{
	MSRULE.SetPacketMax(m_pChannel->m_PackageMaxLen);
	MSRULE.SetTermAddr(CONTEXT.m_SysConfig.m_RTUA);
	MSRULE.InitCommParam(m_pHttxInfo->m_Class1ReportConfirm,m_pHttxInfo->m_Class2ReportConfirm,
		m_pHttxInfo->m_Class3ReportConfirm,m_pHttxInfo->m_TpDelayM);
	return 0;
}

int C_HTTX::MakeLogin(void)
{
	S_REPORTCMD ReportCmd;

	MSRULE.MakeLoginFrame(ReportCmd.m_frame);

	ReportCmd.m_confirm = true;
	ReportCmd.m_resend = 0;
	ReportCmd.m_SendTime = 0;
	ReportCmd.m_CmdInfo = REPORT_NAME_LOGIN;

	m_ReportCmds.push_back(ReportCmd);
    
	return 0;
}

int C_HTTX::MakeHeartBeat(void)
{
	S_REPORTCMD ReportCmd;

	MSRULE.MakeHeartBeatFrame(ReportCmd.m_frame);

	ReportCmd.m_confirm = true;
	ReportCmd.m_resend = 0;
	ReportCmd.m_SendTime = 0;
	ReportCmd.m_CmdInfo = REPORT_NAME_HBEAT;

	m_ReportCmds.push_back(ReportCmd);
    
	return 0;
}

int C_HTTX::ReportSend(void)
{
	if (m_ReportCmds.empty())
		return 0;

	S_REPORTCMD &Report = m_ReportCmds.front();

	if (Report.m_resend == 0)
	{
		if (CmdPut(Report.m_frame) != 0)
			return -1;
		if (Report.m_confirm == true)
		{
			Report.m_resend = 1;
			Report.m_SendTime = time(NULL);
		}
		else
		{
			m_ReportCmds.pop_front();
		}
	}
	else if (Report.m_resend < m_pHttxInfo->m_ResendTimes)
	{
		time_t curtime = time(NULL);
		if (Report.m_SendTime + m_pHttxInfo->m_WaitResponseTimeoutS >= curtime)
			return 0;
		if (CmdPut(Report.m_frame) != 0)
			return -1;
		Report.m_resend += 1;
		Report.m_SendTime = curtime;
	}
	else
	{
		time_t curtime = time(NULL);
		if (Report.m_SendTime + m_pHttxInfo->m_WaitResponseTimeoutS >= curtime)
			return 0;
		logwt<<Report.m_CmdInfo<<" hasn't been confirmed!!!"<<endl;
		ReportFailedProc(Report.m_CmdInfo);
		m_ReportCmds.pop_front();
	}
	return 0;
}

int C_HTTX::ResponseSend(void)
{
	return 0;
}

int C_HTTX::CmdPut(const string &strCmd)
{
	if (m_pMConnect->SendData(strCmd) < 0)
		m_pMConnect->Disconnect();
	else
		;//count traffic

	return 0;
}

int C_HTTX::CmdGet(string &strCmd)
{
	static string strCmdBuf;

	if (m_pMConnect->RecvData(strCmdBuf) < 0)
	{
		m_pMConnect->Disconnect();
		return -1;
	}
	if (strCmdBuf.empty())
		return 0;

	int pos = MSRULE.FindCmdHead(strCmdBuf);
	if (pos == -1)
	{
		strCmdBuf.erase();
	}
	else
	{
		strCmdBuf.erase(0, pos);//erase char before CmdHead
		int size = MSRULE.ExtractCmd(strCmdBuf, strCmd);
		if (size == 0)//partial frame
			;
		else if (size > 0)//successed
			strCmdBuf.erase(0, size+1);
		else//wrong frame
			strCmdBuf.erase(0, 1);
	}

	return (int)strCmd.size();
}

int C_HTTX::CmdCheck(const string &strCmd)
{
	S_CHAR CmdRTUA[4], *CurRTUA = CONTEXT.m_SysConfig.m_RTUA;
	S_BYTE MSA;
	string strAddr;

	if (MSRULE.FrameCheckSum(strCmd) == false)
		return CMDTYPE_ADDR_UNKNOW;
	if (MSRULE.RuleFlagConfirm(strCmd) == false)
		return CMDTYPE_ADDR_UNKNOW;
	if (MSRULE.GetAddr(strCmd, strAddr) != 5)
		return CMDTYPE_ADDR_UNKNOW;
	MSRULE.RtuaNBO2HBO(strAddr.data(), CmdRTUA);
	MSA = strAddr[4];
	if (CmdRTUA[0] != CurRTUA[0] || CmdRTUA[1] != CurRTUA[1])//area code
		return CMDTYPE_ADDR_UNKNOW;
	if (CmdRTUA[2] == CurRTUA[2] && CmdRTUA[3] == CurRTUA[3])
		return CMDTYPE_ADDR_LOCAL;
	if (CmdRTUA[2] == 0x00 && CmdRTUA[3] == 0x00)//invalid addr
		return CMDTYPE_ADDR_UNKNOW;
	if (CmdRTUA[2] == 0xFF && CmdRTUA[3] == 0xFF && (MSA & 0x01))
		return CMDTYPE_ADDR_BROADCAST;
	//slave check
	//relay check
	return CMDTYPE_ADDR_UNKNOW;
}

int C_HTTX::CmdLocalProc(const string &strCmd)
{
	strings strResponse;
	int ok = CmdParser(strCmd, strResponse);
	if (ok == CMDPARSE_CONFIRM || ok == CMDPARSE_DENY || ok == CMDPARSE_CONFIRM_F3)
		CmdConfirmProc(ok, strResponse);
	else if (ok == CMDPARSE_MAKE_CONFIRM)
		;
	else if (ok == CMDPARSE_MAKE_DENY)
		;
	else if (ok == CMDPARSE_MAKE_RESPONSE)
		;
	else if (ok == CMDPARSE_RESEND)//same FCV, resend previous response cmd.
		;
	else if (ok == CMDPARSE_ERROR)
		logwt<<"Cmd may be error,Can not be parse!!!"<<endl;
	else
		logwt<<"Unknow return value of CmdParser!!!"<<endl;

	return 0;
}

int C_HTTX::CmdSlaveProc(const string &strCmd)
{
	return 0;
}

int C_HTTX::CmdRelayProc(const string &strCmd)
{
	return 0;
}

int C_HTTX::CmdParser(const string &strCmd, strings &strResponse)
{
	int ok;
	string strUserData, strAppData, strPnfn;
	MSRULE.ExtractUserData(strCmd, strUserData);
	if ((ok = MSRULE.ParseUserData(strUserData)) == MSCMD_ANALYSE_ERROR)
		return CMDPARSE_ERROR;
	else if (ok == MSCMD_ANALYSE_RESEND)
		return CMDPARSE_RESEND;

	if (MSRULE.ExtractAppData(strUserData, strAppData) == false)
		return CMDPARSE_ERROR;
	if ((ok = MSRULE.ParseAppData(strAppData)) == MSCMD_ANALYSE_ERROR)
		return CMDPARSE_ERROR;
	else if (ok == MSCMD_ANALYSE_DENY)
		return CMDPARSE_DENY;

	if (MSRULE.ExtractPnFn(strAppData, strPnfn) == false)
		return CMDPARSE_DENY;

	C_AFNOBJ *pAfnObj = C_AFNFACT::GetAfnObj(strAppData[0]);
	if (pAfnObj == NULL)
		return CMDPARSE_ERROR;

	return pAfnObj->AfnDataProc(strPnfn, strResponse);
}



int C_HTTX::ReportFailedProc(const string &strReportCmd)
{
	if (strReportCmd.compare(REPORT_NAME_LOGIN) == 0)
	{
		m_pMConnect->Disconnect();
	}
	else if (strReportCmd.compare(REPORT_NAME_HBEAT) == 0)
	{
		m_pMConnect->Disconnect();
	}

	return 0;
}

int C_HTTX::CmdConfirmProc(int CmdResult, const strings &strF3)
{
	if (m_ReportCmds.empty())
		return -1;
	S_REPORTCMD &ReportCmd = m_ReportCmds.front();
	if (ReportCmd.m_SendTime == 0)
		return -1;

	if (CmdResult == CMDPARSE_CONFIRM_F3)
	{
		if (strF3.empty() || strF3[0].empty())
			return -1;
		const string &first = strF3[0];
		CmdResult = first[5];
	}

	if (CmdResult == CMDPARSE_CONFIRM)
		logit<<ReportCmd.m_CmdInfo<<" has been confirmed!!!"<<endl;
	else if (CmdResult == CMDPARSE_DENY)
		logit<<ReportCmd.m_CmdInfo<<" has been denied!!!"<<endl;
	else
		return -1;

	m_ReportCmds.pop_front();
	return 0;
}


S_BOOL C_HTTX::HeartBeatTime(void)
{
	static time_t HBT = 0;
	if (!m_ReportCmds.empty() || !NeedHBeat())
		return false;

	time_t cur = time(NULL), diff = 0x00;
	if (HBT == 0)
		diff = m_pHttxInfo->m_HeartBeatPeriodM * 60;
	else if (HBT > 0)
	{
		if (HBT > cur) diff = HBT - cur;
		else diff = cur - HBT;
	}

	if (diff >= m_pHttxInfo->m_HeartBeatPeriodM * 60)
	{
		HBT = cur;
		return true;
	}
	return false;
}



//
//master station communication
//
C_HTTXI::C_HTTXI(void)
{
	m_HttxID = HTTX_ID_1;
	m_pServInfo = &(CONTEXT.m_ServInfo[HDJS_SERID_HTTX1]);
	m_pHttxInfo = &(CONTEXT.m_HttxInfo[HTTX_ID_1]);
}

C_HTTXI::~C_HTTXI()
{
}

int C_HTTXI::BeforeService(void)
{
	C_SERVER::SetCurrentServId(HDJS_SERID_HTTX1);
	C_HTRACE::SetSystemTraceSetting(&CONTEXT.m_TraceSet[HDJS_SERID_HTTX1]);
	return 0;
}

int C_HTTXI::Servicing(void)
{
	return C_HTTX::Servicing();
}

int C_HTTXI::AfterService(void)
{
	return 0;
}

//
//Local ethernet communication
//
C_HTTXII::C_HTTXII(void)
{
	m_HttxID = HTTX_ID_2;
	m_pServInfo = &(CONTEXT.m_ServInfo[HDJS_SERID_HTTX2]);
	m_pHttxInfo = &(CONTEXT.m_HttxInfo[HTTX_ID_2]);
}

C_HTTXII::~C_HTTXII()
{
}

int C_HTTXII::BeforeService(void)
{
	C_SERVER::SetCurrentServId(HDJS_SERID_HTTX2);
	C_HTRACE::SetSystemTraceSetting(&CONTEXT.m_TraceSet[HDJS_SERID_HTTX2]);
	return 0;
}

int C_HTTXII::Servicing(void)
{
	return C_HTTX::Servicing();
}

int C_HTTXII::AfterService(void)
{
	return 0;
}


//
//Local serial communication
//
C_HTTXIII::C_HTTXIII(void)
{
	m_HttxID = HTTX_ID_3;
	m_pServInfo = &(CONTEXT.m_ServInfo[HDJS_SERID_HTTX3]);
	m_pHttxInfo = &(CONTEXT.m_HttxInfo[HTTX_ID_3]);
}

C_HTTXIII::~C_HTTXIII()
{
}

int C_HTTXIII::BeforeService(void)
{
	C_SERVER::SetCurrentServId(HDJS_SERID_HTTX3);
	C_HTRACE::SetSystemTraceSetting(&CONTEXT.m_TraceSet[HDJS_SERID_HTTX3]);
	return 0;
}

int C_HTTXIII::Servicing(void)
{
	return C_HTTX::Servicing();
}

int C_HTTXIII::AfterService(void)
{
	return 0;
}


