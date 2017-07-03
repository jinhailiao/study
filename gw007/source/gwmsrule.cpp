/** @file
 *  @brief 国网规约解析
 *  @author jinhailiao
 *  @date 2012/05/04
 *  @version 0.1
 */
#include "gwmsrule.h"
#include "timeop.h"

/** @brief MP统计
 *  @return 0 终端测量点
 *  @return 1-8 表计等测量点
 *  @return 0xFF 所有测量点
 */
INT8U C_DA::GetMpCount(void)
{
	if (m_DA2 == 0x00)
		return 0;
	else if (m_DA2 == 0xFF)
		return 0xFF;
	return m_DA1.CountBit();
}

/** @brief MP点测试
 *  @param[in] pos 0-7
 *  @return false 不存在的点
 *  @return true 存在的点
 */
bool C_DA::TestMP(int pos)
{
	if (GetMpCount() == 0xFF)
		return false;
	else if (GetMpCount() == 0x00)
		return pos == 0?true:false;
	return m_DA1.TestBit(pos);
}
/** @brief MP计算 
 *  @param[in] pos 0-7
 *  @return 测量点号
 */
INT16U C_DA::GetMP(int pos)
{
	if (GetMpCount() == 0xFF || GetMpCount() == 0x00)
		return 0x00;
	return (m_DA2-1)*8 + (pos+1);
}

/** @brief 将PN转换成DA
 * @param[in] PN 测量点号
 * @return 有效DA
 */
S_WORD C_DAPN::PN2DA(S_WORD PN)
{
	INT16U DA = 0;

	if (PN != 0)
	{
		PN -= 1;
		DA = (((PN/8+1)<<8)&0xFF00) | ((1<<(PN%8))&0x00FF);
	}

	return DA;
}

/** @brief 将DA转换成PN
 * @param[in] DA 信息点
 * @return 有效PN
 */
S_WORD C_DAPN::DA2PN(S_WORD DA)
{
	C_DA da(DA);
	if (da.GetMpCount() == 0xFF)
		return 0;
	for (int i = 0; i < 8; i++)
	{
		if (da.TestMP(i) == true)
			return da.GetMP(i);
	}
	return 0;
}

/** @brief 将FN转换成DT
 * @param[in] FN 信息功能点
 * @return 有效DT
 */
S_WORD C_DTFN::FN2DT(S_WORD FN)
{
	INT16U DT = 0;
	if (FN != 0)
	{
		FN -= 1;
		DT = (((FN/8)<<8)&0xFF00) | ((1<<(FN%8))&0x00FF);
	}
	return DT;
}

/** @brief 将DT转换成FN
 * @param[in] DT 信息类
 * @return 有效FN
 */
S_WORD C_DTFN::DT2FN(S_WORD DT)
{
	S_BYTE DataPoint = KGE_GETLOBYTE(DT);
	S_BYTE DataGroup = KGE_GETHIBYTE(DT);

	if (DataPoint == 0)
		return 0;
	
	int i;
	for (i = 0; i < 8; i++)
	{//查找信息点
		if ((DataPoint>>i)&0x01)
			break;
	}

	return (S_WORD)DataGroup*8+(i+1);
}

INT8U C_CtrlCode::GetPrimaryFun(INT8U AFN)
{
	return 11;
}

INT8U C_CtrlCode::GetSlaveFun(INT8U PreAFN, INT8U NowAFN)
{
	return 0;
}

void C_RTUA::ParseString(const string &strAddr)
{
	m_AreaCode = strAddr[0] + (strAddr[1] << 8);
	m_DevAddr  = strAddr[2] + (strAddr[3] << 8);
	m_MSA = (strAddr[4] >> 1) & 0x7F;
	m_Group = strAddr[4] & 0x01;
}

void C_RTUA::PrintString(string &strAddr)const
{
	S_BYTE A3 = (m_MSA<<1) | m_Group;
	strAddr.append((char *)&m_AreaCode, 2);
	strAddr.append((char *)&m_DevAddr, 2);
	strAddr.push_back(A3);
}

void C_RTUA::ParseAscii(const char *pRTUA)
{
	S_BYTE RTUA[4];
	RTUA[0] = (kge_Char2Hex(pRTUA[0]) << 4) + (kge_Char2Hex(pRTUA[1]) & 0x0F);// convert to BCD
	RTUA[1] = (kge_Char2Hex(pRTUA[2]) << 4) + (kge_Char2Hex(pRTUA[3]) & 0x0F);
	RTUA[2] = (kge_Char2Hex(pRTUA[4]) << 4) + (kge_Char2Hex(pRTUA[5]) & 0x0F);// convert to HEX
	RTUA[3] = (kge_Char2Hex(pRTUA[6]) << 4) + (kge_Char2Hex(pRTUA[7]) & 0x0F);

	m_AreaCode = KGE_COMBWORD(RTUA[0], RTUA[1]);
	m_DevAddr  = KGE_COMBWORD(RTUA[2], RTUA[3]);
}

char *C_RTUA::PrintAscii(char *pRTUA)const
{
	S_BYTE RTUA[4];
	RTUA[0] = KGE_GETHIBYTE(m_AreaCode);
	RTUA[1] = KGE_GETLOBYTE(m_AreaCode);
	RTUA[2] = KGE_GETHIBYTE(m_DevAddr);
	RTUA[3] = KGE_GETLOBYTE(m_DevAddr);
	for (int i = 0; i < 2; i++)
	{
		pRTUA[i*2+0] = ((RTUA[i]>>4)&0x0F) + '0';//BCD convert to ASCII
		pRTUA[i*2+1] = ((RTUA[i]>>0)&0x0F) + '0';
	}
	for (int i = 2; i < 4; i++)
	{
		pRTUA[i*2+0] = kge_Hex2Char((RTUA[i]>>4)&0x0F);//HEX convert to ASCII
		pRTUA[i*2+1] = kge_Hex2Char((RTUA[i]>>0)&0x0F);
	}
	return pRTUA;
}

void C_FrameTP::ParseString(const string &strTP)
{
	char BcdTime[7];
	m_PFC = strTP[0];
	C_TIME::CurTime2BCDStr(BcdTime);
	copy(strTP.begin()+1, strTP.begin()+1+4, BcdTime+3);
	m_timeFrameSend = C_TIME::BcdStr2Time_Big(BcdTime);
	m_IntervalM = strTP[5];
}

void C_FrameTP::PrintString(string &strTP)
{
	INT8U BcdTime[7];
	C_TIME::Time2BCDStr(m_timeFrameSend, (char*)BcdTime);
	strTP.push_back(m_PFC);
	strTP.append((char *)BcdTime+3, 4);
	strTP.push_back(m_IntervalM);
}

bool C_FrameTP::Valid(void)
{
	if (m_IntervalM == 0)
		return true;
	if (C_TIME::TimeNow() - m_timeFrameSend <= (S_DWORD)m_IntervalM * 60)
		return true;
	return false;
}

bool C_MSRULE::CheckData(const S_FrameData &srcData, S_FrameData &dstData)
{
	dstData = srcData;
//	dstData.m_strSEQ;
//	dstData.m_strDelay;
//	dstData.m_strMaster;
	if (dstData.m_strTerm.size() > 8)
		dstData.m_strTerm.erase(dstData.m_strTerm.begin()+8, dstData.m_strTerm.end());
	else if (dstData.m_strTerm.size() < 8)
		dstData.m_strTerm.append((size_t)(8-dstData.m_strTerm.size()), (char)'0');

	if (dstData.m_strDA.size() > 4)
		dstData.m_strDA.erase(dstData.m_strDA.begin()+4, dstData.m_strDA.end());
	else if (dstData.m_strDA.size() < 4)
		dstData.m_strDA.append((size_t)(4-dstData.m_strDA.size()), (char)'0');

	if (dstData.m_strDT.size() > 4)
		dstData.m_strDT.erase(dstData.m_strDT.begin()+4, dstData.m_strDT.end());
	else if (dstData.m_strDT.size() < 4)
		dstData.m_strDT.append((size_t)(4-dstData.m_strDT.size()), (char)'0');

	if (dstData.m_strPW.size() > 32)
		dstData.m_strPW.erase(dstData.m_strPW.begin()+32, dstData.m_strPW.end());
	else if (dstData.m_strPW.size() < 32)
		dstData.m_strPW.append((size_t)(32-dstData.m_strPW.size()), (char)'0');

//	dstData.m_strData;
	return true;
}

bool C_MSRULE::SetFrameData(const string &strPassword, const string &strTermAddr, S_BYTE MasterAddr)
{
	m_strPassword = strPassword;
	m_strTermAddr = strTermAddr;
	m_MasterAddr = MasterAddr;
	return true;
}

int C_MSRULE::Make(const S_AFN &AFN, C_CtrlCode &CtrlCode, C_RTUA &FrameAddr, C_FrameSeq &FrameSeq, C_FrameTP &FrameTP, string &strCmdInfo, string &strCmd)
{
	string strAppData, strUserData;
	//make app data
	strAppData.push_back((S_BYTE)AFN.m_ID);
	strAppData.push_back(FrameSeq.GetAll());
	strAppData.append(strCmdInfo);

	if (AFN.m_PW == true)
	{
		strAppData.append(m_strPassword);
	}
	if (FrameSeq.GetTpV() == 0x01)
	{
		string strTP;
		FrameTP.PrintString(strTP);
		strAppData.append(strTP);
	}

	//make user data
	string strAddr;
	FrameAddr.PrintString(strAddr);
	strUserData.push_back(CtrlCode.GetAll());
	strUserData.append(strAddr);
	strUserData.append(strAppData);

	//make cmd
	strCmd.push_back(0x68);
	S_WORD FrameLen = (S_WORD)strUserData.size();
	FrameLen = (FrameLen << 2) | (m_ProtocolCode&0x03);//协议标识
	strCmd.append((char *)&FrameLen, 2);
	strCmd.append((char *)&FrameLen, 2);
	strCmd.push_back(0x68);
	strCmd.append(strUserData);
	int CheckSum = accumulate(strUserData.begin(), strUserData.end(), 0);
	strCmd.push_back((INT8U)CheckSum);
	strCmd.push_back(0x16);

	return (int)strCmd.size();
}

int C_MSRULE::MakePrimary(const S_AFN &AFN, C_FrameSeq &FrameSeq, C_FrameTP &FrameTP, string &strCmdInfo, string &strCmd)
{
	C_CtrlCode CtrlCode;
	C_RTUA FrameAddr;

	CtrlCode.SetDIR(0);
	CtrlCode.SetPRM(1);
	CtrlCode.SetFUN(AFN.m_FunCode&0x0F);

	FrameAddr.ParseAscii(m_strTermAddr.c_str());
	FrameAddr.SetMSA(m_MasterAddr&0x7F);

	return Make(AFN, CtrlCode, FrameAddr, FrameSeq, FrameTP, strCmdInfo, strCmd);
}




