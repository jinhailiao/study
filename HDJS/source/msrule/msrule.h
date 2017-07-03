//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			msrule.h
// Description:		master station rule
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-04-28  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __MSRULE_H__
#define __MSRULE_H__

#include "haidef.h"

enum 
{
	AFN_CONFIRM_DENY = 0x00,
	AFN_RESET = 0x01,
	AFN_LINK_CHECK = 0x02,
	AFN_SET_PARAM = 0x04,
	AFN_CONTROL_CMD = 0x05,
	AFN_AUTH_CERTIFY = 0x06,
	AFN_REQUEST_CONFIG = 0x09,
	AFN_GET_PARAM = 0x0A,
	AFN_GET_CLASS1DATA = 0x0C,
	AFN_GET_CLASS2DATA = 0x0D,
	AFN_GET_CLASS3DATA = 0x0E,
};

enum
{
	FUN_PRM1_RESETCMD = 1,
	FUN_PRM1_USERDATA = 4,
	FUN_PRM1_LINKCHECK = 9,
	FUN_PRM1_LEVEL1DATA = 10,
	FUN_PRM1_LEVEL2DATA = 11,

	FUN_PRM0_CONFIRM = 0,
	FUN_PRM0_USERDATA = 8,
	FUN_PRM0_NOTDATA = 9,
	FUN_PRM0_LINKSTAT = 11
};

enum
{
	MSCMD_CHECK_OK,
	MSCMD_CHECK_WRONG,
	MSCMD_CHECK_PARTIAL
};

enum
{
	MSCMD_ANALYSE_ERROR,
	MSCMD_ANALYSE_OK,
	MSCMD_ANALYSE_RESEND,
	MSCMD_ANALYSE_DENY
};


#define MSRULE_CMD_LEN_MIN	20

class C_MSRULE
{
public:
	static C_MSRULE &instance(void);//{static C_MSRULE sMsRule; return sMsRule;}
	/** @brief MSRULE init information
	 *  @note Call them before use MSRULE
	 */
	S_WORD SetPacketMax(S_WORD len){S_WORD n = m_PacketMax; m_PacketMax = len; return n;}
	S_WORD GetPacketMax(){return m_PacketMax;}
	S_BOOL SetTermAddr(const S_CHAR *pRTUA);
	S_BOOL InitCommParam(S_BOOL Class1ReportConfirm,S_BOOL Class2ReportConfirm,S_BOOL Class3ReportConfirm,S_BYTE TpDelayM);

public:
	void RtuaHBO2NBO(const S_CHAR *pHBO, S_CHAR *pNBO);
	void RtuaNBO2HBO(const S_CHAR *pNBO, S_CHAR *pHBO);

	int FindCmdHead(const string &strCmdBuf);
	int ExtractCmd(const string &strCmdBuf, string &strCmd);
	S_BOOL FrameCheckSum(const string &strCmd);
	int GetAddr(const string &strCmd, string &strAddr);
	S_BOOL RuleFlagConfirm(const string &strCmd){return (strCmd[1]&0x03) == m_RuleFlag;}

	S_BOOL ExtractUserData(const string &strCmd, string &strUserData){strUserData.append(strCmd.begin()+6, strCmd.end()-2); return true;}
	S_BOOL ExtractAppData(const string &strUserData, string &strAppData);
	S_BOOL ExtractPnFn(const string &strAppData, string &strPnfn);
	S_BOOL TpvParse(const string &strTPV);
	int ParseUserData(const string &strUserData);
	int ParseAppData(const string &strAppData);

	int MakeLoginFrame(string &strLogin);
	int MakeHeartBeatFrame(string &strHB);

private:
	int MakeLinkCheckFrame(const string &strPnfn, string &strHB);

	int CheckCmd(const string &strCmdBuf, int start, int &end);
	int PacketMsruleFrame(const strings &UserDatas, strings &msFrames);
	int PacketUserData(S_BYTE Ctrl, const string &strRTUA, const strings &appDatas, strings &UserDatas);
	int PacketAppData(S_BOOL primary, S_BYTE AFN, S_BOOL TpV, S_BOOL con, const strings &pnfnDatas, strings &appDatas);

private:
	int _PacketAppData(S_BYTE AFN, S_BYTE SEQ, S_WORD EC, const string &pnfnData, string &appData);
	S_BYTE MakePSEQ(S_BOOL TpV, S_BOOL FIR, S_BOOL FIN, S_BOOL con, S_BYTE SEQ)
	{
		return (SEQ & 0x0F)|(con<<4)|(FIN<<5)|(FIR<<6)|(TpV<<7);
	}
	S_BYTE GetTPV(S_BYTE SEQ){return SEQ & 0x80;}
	S_WORD GetEC(void){return 0;};
	int MakeTPV(string &strTPV);
	S_BYTE MakePrimaryCtrlCode(S_BYTE funCode);
	int MakeTermAddr(string &strRTUA, S_BYTE MSA);
	S_BYTE CheckSum(const string &strData);
	S_BOOL ClearError(void);

	S_BYTE CtrlDIR(S_BYTE ctrl){return ctrl&0x80;}
	S_BYTE CtrlDIR(void){return CtrlDIR(m_CtrlCode);}
	S_BYTE CtrlPRM(S_BYTE ctrl){return ctrl&0x40;}
	S_BYTE CtrlPRM(void){return CtrlPRM(m_CtrlCode);}
	S_BYTE CtrlFCB(S_BYTE ctrl){return ctrl&0x20;}
	S_BYTE CtrlFCB(void){return CtrlFCB(m_CtrlCode);}
	S_BYTE CtrlFCV(S_BYTE ctrl){return ctrl&0x10;}
	S_BYTE CtrlFCV(void){return CtrlFCV(m_CtrlCode);}
	S_BYTE CtrlFUN(S_BYTE ctrl){return ctrl&0x0F;}
	S_BYTE CtrlFUN(void){return CtrlFUN(m_CtrlCode);}

	S_BYTE SeqTPV(S_BYTE seq){return seq&0x80;}
	S_BYTE SeqTPV(void){return SeqTPV(m_SEQ);}
	S_BYTE SeqFIR(S_BYTE seq){return seq&0x40;}
	S_BYTE SeqFIR(void){return SeqFIR(m_SEQ);}
	S_BYTE SeqFIN(S_BYTE seq){return seq&0x20;}
	S_BYTE SeqFIN(void){return SeqFIN(m_SEQ);}
	S_BYTE SeqCON(S_BYTE seq){return seq&0x10;}
	S_BYTE SeqCON(void){return SeqCON(m_SEQ);}
	S_BYTE SeqRSEQ(S_BYTE seq){return seq&0x0F;}
	S_BYTE SeqRSEQ(void){return SeqRSEQ(m_SEQ);}

	S_BYTE GetMSA(S_BYTE val){return val>>1;}
	S_BYTE GetMSA(void){return GetMSA(m_MSAG);}

	S_BOOL PasswordCheck(const string &strPassword){return true;}

private:
	C_MSRULE(void);
	~C_MSRULE();

private:
	S_BYTE m_CtrlCode;
	S_CHAR m_FrameAddr[4];
	S_BYTE m_MSAG;//MSA+GROUP
	S_BYTE m_AFN;
	S_BYTE m_SEQ;
	S_BYTE m_RSEQ;

	S_BOOL m_Class1ReportConfirm;
	S_BOOL m_Class2ReportConfirm;
	S_BOOL m_Class3ReportConfirm;
	S_BYTE m_TpDelayM;//=6 #终端作为启动站允许发送传输延时时间
	S_BYTE m_PFC;
	S_WORD m_EC;
	
	S_WORD m_PacketMax;
	S_CHAR m_TermAddr[4];

	static const S_WORD m_RuleFlag = 0x02;
};


#define MSRULE C_MSRULE::instance()

#endif //__MSRULE_H__


