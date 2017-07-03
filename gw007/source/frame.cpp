//
//
//
#include "frame.h"
#include "gwmsrule.h"
#include "timeop.h"

int C_String2Char::operator()(char ch)
{
	if (m_strAscii.empty() == false)
		m_strAscii.push_back(' ');
	m_strAscii.push_back(kge_Hex2Char((ch>>4)&0x0F));
	m_strAscii.push_back(kge_Hex2Char((ch>>0)&0x0F));
	return (int)m_strAscii.size();
}

int C_FRAME::MakeFrame(const S_RULE &rule, const S_FrameData &FrameData, string &strFrame)
{
	C_MSRULE MsRule(rule.m_Protocol);
	S_FrameData myFrameData;
	if (MsRule.CheckData(FrameData, myFrameData) == false)
		return -1;
	
	int DA = 0, DT = 0;
	sscanf(myFrameData.m_strDA.c_str(), "%X", &DA);
	sscanf(myFrameData.m_strDT.c_str(), "%X", &DT);
	DA = KGE_SWAPWORD(DA);
	DT = KGE_SWAPWORD(DT);

	string strCmdInfo;
	strCmdInfo.append((char *)&DA, 2);
	strCmdInfo.append((char *)&DT, 2);
	Ascii2Data(myFrameData.m_strData, strCmdInfo);

	string strPassword;
	Ascii2Data(myFrameData.m_strPW, strPassword);
	if ((int)strPassword.size() > rule.m_PwdLen)
		strPassword.erase(strPassword.begin()+rule.m_PwdLen, strPassword.end());
	else if ((int)strPassword.size() < rule.m_PwdLen)
		strPassword.append((size_t)(rule.m_PwdLen-strPassword.size()), (char)0);

	int Master = atoi(myFrameData.m_strMaster.c_str());
	int Delay = atoi(myFrameData.m_strDelay.c_str());
	int SEQ = atoi(myFrameData.m_strSEQ.c_str());
	C_FrameSeq FrameSeq(SEQ&0x0F);
	FrameSeq.SetFIN(1);
	FrameSeq.SetFIR(1);
	FrameSeq.SetCON(1);
	if (Delay > 0)
		FrameSeq.SetTpV(1);

	C_FrameTP FrameTP(SEQ&0xFF, C_TIME::TimeNow(), Delay&0xFF);
	const S_AFN &objAFN = rule.m_AfnItem[myFrameData.m_AfnSN];

	MsRule.SetFrameData(strPassword, myFrameData.m_strTerm, Master);

	return MsRule.MakePrimary(objAFN, FrameSeq, FrameTP, strCmdInfo, strFrame);
}

int C_FRAME::Ascii2Data(const string &strAscii, string &strData)
{
	for (string::const_iterator iter = strAscii.begin(); iter < strAscii.end(); iter += 2)
	{
		S_BYTE H = kge_Char2Hex(*(iter+0));
		S_BYTE L = kge_Char2Hex(*(iter+1));
		strData.push_back((H<<4)|(L&0x0F));
	}

	return (int)strData.size();
}
