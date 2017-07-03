//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			msrule.cpp
// Description:		master station rule
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-04-28  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "msrule.h"
#include "timeop.h"
#include "mscmd.h"
#include "msfact.h"

C_MSRULE::C_MSRULE(void)
{
	memset(this, 0x00, sizeof(*this));
}

C_MSRULE::~C_MSRULE()
{
}

C_MSRULE &C_MSRULE::instance(void)
{
	static C_MSRULE sMsRule;
	return sMsRule;
}

S_BOOL C_MSRULE::SetTermAddr(const S_CHAR *pRTUA)
{
	/** HBO-->NBO
	 */
	RtuaHBO2NBO(pRTUA, m_TermAddr);
	return true;
}

S_BOOL C_MSRULE::InitCommParam(S_BOOL Class1ReportConfirm,S_BOOL Class2ReportConfirm,S_BOOL Class3ReportConfirm,S_BYTE TpDelayM)
{
	m_Class1ReportConfirm = Class1ReportConfirm;
	m_Class2ReportConfirm = Class2ReportConfirm;
	m_Class3ReportConfirm = Class3ReportConfirm;
	m_TpDelayM = TpDelayM;//=6 #终端作为启动站允许发送传输延时时间
	return true;
}

void C_MSRULE::RtuaHBO2NBO(const S_CHAR *pHBO, S_CHAR *pNBO)
{
	pNBO[0] = pHBO[1], pNBO[1] = pHBO[0];
	pNBO[2] = pHBO[3], pNBO[3] = pHBO[2];
}

void C_MSRULE::RtuaNBO2HBO(const S_CHAR *pNBO, S_CHAR *pHBO)
{
	pHBO[0] = pNBO[1], pHBO[1] = pNBO[0];
	pHBO[2] = pNBO[3], pHBO[3] = pNBO[2];
}

int C_MSRULE::FindCmdHead(const string &strCmdBuf)
{
	return (int)strCmdBuf.find(0x68);
}

int C_MSRULE::ExtractCmd(const string &strCmdBuf, string &strCmd)
{
	int end = -1;
	if (strCmdBuf.size() < MSRULE_CMD_LEN_MIN)
		return 0;//partial frame

	int ok = CheckCmd(strCmdBuf, 0, end);
	if (ok == MSCMD_CHECK_OK)
		strCmd.append(strCmdBuf, 0, end+1);
	else if (ok == MSCMD_CHECK_WRONG)
		end = -1;//wrong frame
	else if (ok == MSCMD_CHECK_PARTIAL)
		end = 0;

	return end;
}

S_BOOL C_MSRULE::FrameCheckSum(const string &strCmd)
{
	S_BYTE sum = 0;
	for (S_DWORD i = 6; i < strCmd.size()-2; ++i)
		sum += (S_BYTE)strCmd[i];

	return (sum == (S_BYTE)strCmd[strCmd.size()-2]);
}

int C_MSRULE::GetAddr(const string &strCmd, string &strAddr)
{
	S_WORD FrameLen = (((S_WORD)strCmd[1] & 0xFF) + ((S_WORD)strCmd[2] << 8)) >> 2;
	if (FrameLen < 10)
		return 0;
	strAddr.append(strCmd, 7, 5);
	return (int)strAddr.size();
}


int C_MSRULE::CheckCmd(const string &strCmdBuf, int start, int &end)
{
	if (start + MSRULE_CMD_LEN_MIN > (int)strCmdBuf.size())
		return MSCMD_CHECK_PARTIAL;

	end = start;
	if ((strCmdBuf[start] != 0x68) || (strCmdBuf[end=start+5] != 0x68))
		return MSCMD_CHECK_WRONG;
	if (strCmdBuf[start+1] != strCmdBuf[start+3] || strCmdBuf[start+2] != strCmdBuf[start+4])
		return MSCMD_CHECK_WRONG;

	S_WORD FrameLen = ((S_WORD)strCmdBuf[start+1] & 0xFF) + ((S_WORD)strCmdBuf[start+2] << 8);
	FrameLen >>= 2;
	if (start+FrameLen+8 > (int)strCmdBuf.size())
		return MSCMD_CHECK_PARTIAL;
	if (strCmdBuf[start+FrameLen+8-1] != 0x16)
		return MSCMD_CHECK_WRONG;

	end = start+FrameLen+8-1;
	return MSCMD_CHECK_OK;
}

int C_MSRULE::MakeLoginFrame(string &strLogin)
{
	string strLoginPnfn("\x00\x00\x01\x00", 4);

	return MakeLinkCheckFrame(strLoginPnfn, strLogin);
}

int C_MSRULE::MakeHeartBeatFrame(string &strHB)
{
	string strHeartPnfn("\x00\x00\x04\x00", 4);

	return MakeLinkCheckFrame(strHeartPnfn, strHB);
}

int C_MSRULE::MakeLinkCheckFrame(const string &strPnfn, string &strFrame)
{
	string strRTUA;
	strings pnfnDatas, appDatas, userDatas, msFrames;

	MakeTermAddr(strRTUA, 0x00);
	pnfnDatas.push_back(strPnfn);

	PacketAppData(true, AFN_LINK_CHECK, false, true, pnfnDatas, appDatas);
	PacketUserData(MakePrimaryCtrlCode(FUN_PRM1_LINKCHECK), strRTUA, appDatas, userDatas);
	PacketMsruleFrame(userDatas, msFrames);

	strFrame.append(msFrames[0]);

	return (int)strFrame.size();
}

int C_MSRULE::PacketMsruleFrame(const strings &UserDatas, strings &msFrames)
{
	for (S_DWORD i = 0; i < UserDatas.size(); ++i)
	{
		string strData;
		S_WORD len = (S_WORD)UserDatas[i].size();
		len = (len << 2) | m_RuleFlag;
		strData.push_back(0x68);
		strData.append((char *)&len, 2);
		strData.append((char *)&len, 2);
		strData.push_back(0x68);
		strData.append(UserDatas[i]);
		strData.push_back(CheckSum(UserDatas[i]));
		strData.push_back(0x16);

		msFrames.push_back(strData);
	}
	return (int)msFrames.size();
}

int C_MSRULE::PacketUserData(S_BYTE Ctrl, const string &strRTUA, const strings &appDatas, strings &UserDatas)
{
	m_EC = 0x00;//clear EC, beacuse Ctrl has made.
	for (S_DWORD i = 0; i < appDatas.size(); ++i)
	{
		string strData;
		strData.push_back(Ctrl);
		strData.append(strRTUA);
		strData.append(appDatas[i]);

		UserDatas.push_back(strData);
	}

	return (int)UserDatas.size();
}

int C_MSRULE::PacketAppData(S_BOOL primary, S_BYTE AFN, S_BOOL TpV, S_BOOL con, const strings &pnfnDatas, strings &appDatas)
{
	S_BYTE seq;
	S_BOOL FIR, FIN;
	m_EC = GetEC();

	for (S_DWORD i = 0; i < pnfnDatas.size(); ++i)
	{
		if (i == 0) FIR = true;
		else FIR = false;

		if (i == pnfnDatas.size()-1) FIN = true;
		else FIN = false;

		if (primary == true) seq = m_PFC & 0x0F;
		else seq = m_RSEQ & 0x0F;

		S_BYTE SEQ = MakePSEQ(TpV, FIR, FIN, con, seq);

		string strData;
		_PacketAppData(AFN, SEQ, m_EC, pnfnDatas[i], strData);
		appDatas.push_back(strData);

		if (primary == true) m_PFC += 1;
		else m_RSEQ += 1;
	}

	return (int)appDatas.size();
}

int C_MSRULE::_PacketAppData(S_BYTE AFN, S_BYTE SEQ, S_WORD EC, const string &pnfnData, string &appData)
{
	appData.push_back(AFN);
	appData.push_back(SEQ);
	appData.append(pnfnData);
	if (EC != 0)
		appData.append((char *)&EC, 2);
	if (GetTPV(SEQ))
	{
		string tpv;
		MakeTPV(tpv);
		appData.append(tpv);
	}

	return 0;
}

S_BOOL C_MSRULE::ExtractAppData(const string &strUserData, string &strAppData)
{
	if (strUserData.size() <= 6)
		return false;
	strAppData.append(strUserData.begin()+6, strUserData.end());
	return true;
}

S_BOOL C_MSRULE::ExtractPnFn(const string &strAppData, string &strPnfn)
{
	size_t EndLen = 0;
	C_AFNOBJ *pAfnObj = C_AFNFACT::GetAfnObj(m_AFN);

	if (SeqTPV()) EndLen += 6;
	if (pAfnObj && pAfnObj->NeedPassword()) EndLen += 16;

	if (strAppData.size() <= EndLen + 2)
		return false;

	strPnfn.append(strAppData.begin()+2, strAppData.end()-EndLen);
	return true;
}

S_BOOL C_MSRULE::TpvParse(const string &strTPV)
{
	char BcdTime[7];
	S_BYTE PFC = strTPV[0];///???
	PFC = PFC;//avoid warn
	S_WORD DelayS = (S_WORD)strTPV[5] * 60;
	time_t CurTime = time(NULL);

	C_TIME::Time2BCDStr(CurTime, BcdTime);
	copy(strTPV.begin()+1, strTPV.begin()+5, BcdTime);
	time_t StartTime = C_TIME::BcdStr2Time(BcdTime);
	if (StartTime + DelayS > CurTime)
		return true;
	return false;
}

int C_MSRULE::ParseUserData(const string &strUserData)
{
	int ok = MSCMD_ANALYSE_OK;
	S_BYTE ctrl = strUserData[0];

	if (CtrlDIR(ctrl) != 0x00)//from master station?
		return MSCMD_ANALYSE_ERROR;
	//PRM
	if (CtrlFCB() && CtrlFCB(ctrl) && CtrlFCV()==CtrlFCV(ctrl))
		ok = MSCMD_ANALYSE_RESEND;
	//FUN
	m_CtrlCode = ctrl;
	m_FrameAddr[0] = strUserData[1];
	m_FrameAddr[1] = strUserData[2];
	m_FrameAddr[2] = strUserData[3];
	m_FrameAddr[3] = strUserData[4];
	m_MSAG = strUserData[5];

	return ok;
}

int C_MSRULE::ParseAppData(const string &strAppData)
{
	S_WORD NeedLenMin = 2;
	C_AFNOBJ *pAfnObj = NULL;
	string strTPV, strPassword;

	if (strAppData.size() < NeedLenMin)
		goto END;

	m_AFN = strAppData[0];
	m_SEQ = strAppData[1];
	m_RSEQ = SeqRSEQ(m_SEQ);

	if (SeqTPV())//has TpV
	{
		NeedLenMin += 6;
		if (strAppData.size() < NeedLenMin)
			goto END;
		strTPV.append(strAppData.end()-6, strAppData.end());
		if (TpvParse(strTPV) == false)
			goto END;
	}

	pAfnObj = C_AFNFACT::GetAfnObj(m_AFN);
	if (pAfnObj && pAfnObj->NeedPassword())
	{
		NeedLenMin += 16;
		if (strAppData.size() < NeedLenMin)
			goto END;
		int TpvLen = SeqTPV()? 6:0;
		strPassword.append(strAppData.end()-TpvLen-16, strAppData.end()-TpvLen);
		if (PasswordCheck(strPassword) == false)
			return MSCMD_ANALYSE_DENY;
	}

	return MSCMD_ANALYSE_OK;
END:
	ClearError();
	return MSCMD_ANALYSE_ERROR;
}

int C_MSRULE::MakeTPV(string &strTPV)
{
	char bcdstr[7];

	strTPV.push_back(m_PFC);
	C_TIME::CurTime2BCDStr(bcdstr);
	strTPV.append(bcdstr, 4);
	strTPV.push_back(m_TpDelayM);

	return (int)strTPV.size();
}

S_BYTE C_MSRULE::MakePrimaryCtrlCode(S_BYTE funCode)
{
	S_BYTE DIR = 1, PRM = 1, ACD;
	if (m_EC) ACD = 1;
	else ACD = 0;

	return (funCode&0x0F)|(ACD<<5)|(PRM<<6)|(DIR<<7);
}

int C_MSRULE::MakeTermAddr(string &strRTUA, S_BYTE MSA)
{
	strRTUA.append((char*)m_TermAddr, 4);
	strRTUA.push_back(MSA);

	return (int)strRTUA.size();
}

S_BYTE C_MSRULE::CheckSum(const string &strData)
{
	S_BYTE sum = 0;
	for (S_DWORD i = 0; i < strData.size(); ++i)
		sum += (S_BYTE)strData[i];
	return sum;
}

S_BOOL C_MSRULE::ClearError(void)
{
	m_CtrlCode = 0x00;
	m_MSAG = 0x00;
	m_AFN  = 0x00;
	m_SEQ  = 0x00;
	m_RSEQ = 0x00;
	memset(m_FrameAddr, 0x00, sizeof(m_FrameAddr));

	return true;
}

