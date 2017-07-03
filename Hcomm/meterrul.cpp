
//
//
//
#include "stdafx.h"
#include "meterrul.h"


#define MRULE_REG_FILE "meterreg.ini"

S_MRULE C_MRULELOAD::m_MeterRule[MRULE_MAX];

C_07MREULE MRule07;
C_97MREULE MRule97;
C_LGRMREULE MRuleLgr;

C_MRULE *pCurMrule = &MRule97;

const char *pMeterRule[MRULE_MAX] =
{
	"DLT645-97",
	"DLT645-07",
	"LGR"
};

const S_MDATAID defDataID[MRULE_MAX] =
{
	{"9010", "��ǰ�����й��ܵ���"},
	{"00010000", "��ǰ�����й��ܵ���"},
	{"21", "��������½��"}
};

const S_DLTCTRLCODE Meter97CtrlCode[] =
{
	{"�����ݣ�01H", MR97_CTRL_READ_DATA},
	{"д���ݣ�04H", MR97_CTRL_WRITE_DATA},
	{"�㲥Уʱ��08H", MR97_CTRL_ADJUST_TIME},
	{"дͨ�ŵ�ַ��0AH", MR97_CTRL_WRITE_ADDR},
	{"�޸����룺0FH", MR97_CTRL_CHANGE_PW},
	{"����������㣺10H", MR97_CTRL_CLEAR_NEED},

	{"", 0x00}
};

const S_DLTCTRLCODE Meter07CtrlCode[] =
{
	{"�����ݣ�11H", MR07_CTRL_READ_DATA},
	{"д���ݣ�14H", MR07_CTRL_WRITE_DATA},
	{"��ͨ�ŵ�ַ��13H", MR07_CTRL_READ_ADDR},
	{"дͨ�ŵ�ַ��15H", MR07_CTRL_WRITE_ADDR},
	{"�㲥Уʱ��08H", MR07_CTRL_ADJUST_TIME},
	{"�������16H", MR07_CTRL_FREEZE_CMD},
	{"�޸����룺18H", MR07_CTRL_CHANGE_PW},
	{"����������㣺19H", MR07_CTRL_CLEAR_NEED},
	{"������㣺1AH", MR07_CTRL_CLEAR_METER},
	{"�¼����㣺1BH", MR07_CTRL_CLEAR_EVENT},

	{"", 0x00}
};

enum {TOKE_ERROR, TOKE_RULENAME, TOKE_REGNAME, TOKE_COMMENT};

bool C_MRULELOAD::LoadAll(void)
{
	for (int i = 0; i < MRULE_MAX; i++)//�ȳ�ʼ��ΪĬ�ϵ�
	{
		m_MeterRule[i].m_ruleName = pMeterRule[i];
		m_MeterRule[i].m_dataIDS[0].m_dataID = defDataID[i].m_dataID;
		m_MeterRule[i].m_dataIDS[0].m_regName= defDataID[i].m_regName;
		m_MeterRule[i].IDCount = 1;
	}

	ifstream iMeterReg(MRULE_REG_FILE);
	if (!iMeterReg)//�����ʼ���ļ������ڣ���дһ��
	{
		ofstream oMeterReg(MRULE_REG_FILE);
		if (!oMeterReg)
		{
		}
		else
		{
			oMeterReg<<"#����Լ����ID�����ļ�"<<endl;
			oMeterReg<<"#������Ӧ��Լ�°���DATAID=����ID���ơ��������ID"<<endl;
			for (int i = 0; i < MRULE_MAX; i++)
			{
				oMeterReg<<"["<<pMeterRule[i]<<"]"<<endl;
				oMeterReg<<defDataID[i].m_dataID<<"="<<defDataID[i].m_regName<<endl;
			}
			oMeterReg.close();
		}
	}
	else//��ʼ���ļ����ڣ����س�ʼ���ļ�
	{
		int toke, currule = -1;
		char linebuf[1024],reg[256], name[256];
		while (iMeterReg.getline(linebuf, 1024, '\n'))
		{
			toke = ParseMeterReg(linebuf, reg, name);
			switch (toke)
			{
			case TOKE_RULENAME:
				currule = FindMeterRule(reg);
				if (currule != -1)
				{
					m_MeterRule[currule].m_ruleName = reg;
					m_MeterRule[currule].IDCount = 0;
				}
				break;
			case TOKE_REGNAME:
				if (currule != -1 && m_MeterRule[currule].IDCount < MRULE_DATAID_MAX-1)
				{
					m_MeterRule[currule].m_dataIDS[m_MeterRule[currule].IDCount].m_dataID = reg;
					m_MeterRule[currule].m_dataIDS[m_MeterRule[currule].IDCount].m_regName= name;
					m_MeterRule[currule].IDCount += 1;
				}
				break;
			case TOKE_COMMENT:
				break;
			default:
				toke = TOKE_ERROR;
				break;
			}
			if (toke == TOKE_ERROR)
				break;
		}
		iMeterReg.close();
		for (int i = 0; i < MRULE_MAX; i++)//check if dataID is nothing 
		{
			if (m_MeterRule[i].IDCount <= 0)
				toke = TOKE_ERROR;
		}
		if (toke == TOKE_ERROR)
		{
			for (int i = 0; i < MRULE_MAX; i++)
			{
				m_MeterRule[i].m_ruleName = pMeterRule[i];
				m_MeterRule[i].m_dataIDS[0].m_dataID = defDataID[i].m_dataID;
				m_MeterRule[i].m_dataIDS[0].m_regName= defDataID[i].m_regName;
				m_MeterRule[i].IDCount = 1;
			}
		}
	}
	return true;
}

int C_MRULELOAD::ParseMeterReg(const char *plinebuf, char *preg, char *pname)
{
	char ch;
	int toke = TOKE_COMMENT;
	int state = PMRS_START;
	while (1)
	{
		ch = *plinebuf;
		switch (state)
		{
		case PMRS_START:
			if (ch == '[')
			{
				state = PMRS_RULENAME;
				toke = TOKE_RULENAME;
			}
			else if ((ch>='0' && ch<='9')
				||(ch>='a' && ch<='f')
				||(ch>='A' && ch<='F'))
			{
				state = PMRS_RULEREG;
				toke = TOKE_REGNAME;
				*preg++ = ch;
			}
			else if (ch == '#' || ch == '\r' || ch == '\n' || ch == '\0')//comment
			{
				state = PMRS_END;
				toke = TOKE_COMMENT;
			}
			else if (ch == ' ' || ch == '\t')// skip space
			{
			}
			else //error
			{
				toke  = TOKE_ERROR;
				state = PMRS_END;
			}
			plinebuf++;
			break;
		case PMRS_RULENAME:
			if (ch & 0x80) //����
			{
				*preg++ = ch;
				*preg++ = *(++plinebuf);
			}
			else if (ch == ']')
			{
				*preg = '\0';
				state = PMRS_END;
			}
			else if (ch == '\r' || ch == '\n' || ch == '\0')
			{
				toke  = TOKE_ERROR;
				state = PMRS_END;
			}
			else
			{
				*preg++ = ch;
			}
			plinebuf++;
			break;
		case PMRS_RULEREG:
			if (ch & 0x80)
			{
				toke  = TOKE_ERROR;
				state = PMRS_END;
			}
			else if (ch == '=')
			{
				*preg = '\0';
				state = PMRS_RULERNAME;
			}
			else if ((ch >= '0' && ch <= '9')
				||(ch>='a' && ch<='f')
				||(ch>='A' && ch<='F'))
			{
				*preg++ = ch;
			}
			else if (ch == ' ' || ch == '\t')
			{
			}
			else
			{
				toke = TOKE_ERROR;
				state = PMRS_END;
			}
			plinebuf++;
			break;
		case PMRS_RULERNAME:
			if (ch & 0x80)
			{
				*pname++ = ch;
				*pname++ = *(++plinebuf);
			}
			else if (ch == '\r' || ch == '\n' || ch == '\0')
			{
				*pname = '\0';
				state = PMRS_END;
			}
			else
			{
				*pname++ = ch;
			}
			plinebuf++;
			break;
		default:
			toke = TOKE_ERROR;
			state = PMRS_END;
			break;
		}
		if (state == PMRS_END)
			break;
	}
	return toke;
}

int C_MRULELOAD::FindMeterRule(const char *prule)
{
	for (int i = 0; i < MRULE_MAX; i++)
	{
		if (strcmp(prule, pMeterRule[i]) == 0)
			return i;
	}
	return -1;
}

C_MRULE::C_MRULE(S_MRULE &MeterRule):m_MeterRule(MeterRule)
{
	m_pCtrlCode = NULL;
	m_pMSetName = NULL;
}

C_MRULE::~C_MRULE()
{
}

const char *C_MRULE::GetMRuleName(void)
{
	return m_MeterRule.m_ruleName.c_str();
}

const char *C_MRULE::GetMRuleDataID(int ID)
{
	return m_MeterRule.m_dataIDS[ID].m_dataID.c_str();
}

const char *C_MRULE::GetMRuleRegName(int ID)
{
	return m_MeterRule.m_dataIDS[ID].m_regName.c_str();
}

int C_MRULE::GetMRuleDataIDCount(void)
{
	return m_MeterRule.IDCount;
}

int C_MRULE::FillMeterAddress(string &OutAddr, const string &InAddr)
{
	if (InAddr.size() > 12)
		return 0;
	OutAddr.append((size_t)(12 - InAddr.size()), (char)'0');
	OutAddr.append(InAddr);

	return (int)OutAddr.size();
}

char C_MRULE::Ascii2Digit(char ch)
{
	if (ch >= '0' && ch <= '9')
		return (ch-'0');
	if (ch >= 'a' && ch <= 'f')
		return (ch-'a'+10);
	if (ch >= 'A' && ch <= 'F')
		return (ch-'A'+10);
	return 0;
}

char C_MRULE::Digit2Ascii(char val)
{
	if (val >= 0 && val <= 9)
		return (val+'0');
	if (val >= 10 && val <= 15)
		return (val-10+'A');
	return '0';
}

char *C_MRULE::BCD2String(char bcd, char *pString)
{
	pString[0] = C_MRULE::Digit2Ascii((bcd>>4)&0x0F);
	pString[1] = C_MRULE::Digit2Ascii((bcd>>0)&0x0F);
	pString[2] = '\0';

	return pString;
}

char C_MRULE::String2BCD(const char *pString)
{
	char H = C_MRULE::Ascii2Digit(pString[0]);
	char L = C_MRULE::Ascii2Digit(pString[1]);

	return ((H<<4) | L);
}

int C_MRULE::String2HEX(string &strHEX, const string &strNum)
{
	BYTE H, L;
	for (unsigned int i = 0; i < strNum.size(); i++)
	{
		if (strNum[i] == ' ')
			continue;
		H = (BYTE)C_MRULE::Ascii2Digit(strNum[i]);
		if ((i+1 < strNum.size()) && strNum[i+1] != ' ')
		{
			L = (BYTE)C_MRULE::Ascii2Digit(strNum[++i]);
			H = (H<<4)|L;
		}
		strHEX.push_back(H);
	}
	return (int)strHEX.size();
}

int C_MRULE::ForEachAdd(string &strData, char num)
{
	for (unsigned int i = 0; i < strData.size(); i++)
		strData[i] += num;
	return (int)strData.size();
}

int C_MRULE::ConvertNetByteOrder(string &strData)
{
	for (unsigned int i = 0; i < strData.size() / 2; i++)
	{
		char ch = strData[i];
		strData[i] = strData[strData.size() - i - 1];
		strData[strData.size() - i - 1] = ch;
	}
	return (int)strData.size();
}

char* C_MRULE::Time2BCDStr(time_t time, char *bcdStr7B)
{
	struct tm *curtime = localtime(&time);

	bcdStr7B[0] = cbcd(curtime->tm_sec);
	bcdStr7B[1] = cbcd(curtime->tm_min);
	bcdStr7B[2] = cbcd(curtime->tm_hour);
	bcdStr7B[3] = cbcd(curtime->tm_mday);
	bcdStr7B[4] = cbcd(curtime->tm_mon+1);
	bcdStr7B[5] = cbcd((curtime->tm_year+1900)%100);
	bcdStr7B[6] = cbcd((curtime->tm_year+1900)/100);

	return bcdStr7B;
}

unsigned int C_MRULE::CheckSum(string &frame)
{
	unsigned int cs = 0;
	for (unsigned int i = 0; i < frame.size(); i++)
	{
		cs += (unsigned char)frame[i];
	}
	return cs;
}

//DLT645-2007
C_07MREULE::C_07MREULE(void):C_MRULE(C_MRULELOAD::m_MeterRule[MRULE_645_2007])
{
	m_pCtrlCode = Meter07CtrlCode;
	m_pMSetName = "����07��߼�����";
}

BOOL C_07MREULE::EnableFreezeWnd(int CtrlSN)
{
	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return (CtrlCode == MR07_CTRL_FREEZE_CMD);
}

BOOL C_07MREULE::EnableOpCodeWnd(int CtrlSN)
{
	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return (CtrlCode == MR07_CTRL_WRITE_DATA
		|| CtrlCode == MR07_CTRL_CLEAR_NEED
		|| CtrlCode == MR07_CTRL_CLEAR_METER
		|| CtrlCode == MR07_CTRL_CLEAR_EVENT);
}

BOOL C_07MREULE::EnablePasswordWnd(int CtrlSN)
{
	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return (CtrlCode == MR07_CTRL_WRITE_DATA
		|| CtrlCode == MR07_CTRL_CHANGE_PW
		|| CtrlCode == MR07_CTRL_CLEAR_NEED
		|| CtrlCode == MR07_CTRL_CLEAR_METER
		|| CtrlCode == MR07_CTRL_CLEAR_EVENT);
}

BOOL C_07MREULE::EnableWriteDataWnd(int CtrlSN)
{
	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return (CtrlCode == MR07_CTRL_WRITE_DATA
		|| CtrlCode == MR07_CTRL_CHANGE_PW
		|| CtrlCode == MR07_CTRL_CLEAR_EVENT);
}

BOOL C_07MREULE::EnableAddressWnd(int CtrlSN)
{
	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return (CtrlCode != MR07_CTRL_READ_ADDR);
}

BOOL C_07MREULE::EnableDataIDWnd(int CtrlSN)
{
	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return (CtrlCode == MR07_CTRL_READ_DATA
		|| CtrlCode == MR07_CTRL_WRITE_DATA
		|| CtrlCode == MR07_CTRL_CHANGE_PW);
}

int C_07MREULE::MakeMeterRuleFrame(string &Frame, unsigned char CtrlCode, S_MRULEINFO &RuleInfo)
{
	switch (CtrlCode)
	{
	case MR07_CTRL_READ_DATA:
		return MakeFrameReadData(Frame, RuleInfo);
	case MR07_CTRL_WRITE_DATA:
		return MakeFrameWritData(Frame, RuleInfo);
	case MR07_CTRL_READ_ADDR:
		return MakeFrameReadAddr(Frame, RuleInfo);
	case MR07_CTRL_WRITE_ADDR:
		return MakeFrameWritAddr(Frame, RuleInfo);
	case MR07_CTRL_ADJUST_TIME:
		return MakeFrameAdjsTime(Frame, RuleInfo);
	case MR07_CTRL_FREEZE_CMD:
		return MakeFrameFreezCmd(Frame, RuleInfo);
	case MR07_CTRL_CHANGE_PW:
		return MakeFrameChangePW(Frame, RuleInfo);
	case MR07_CTRL_CLEAR_NEED:
		return MakeFrameClearNeed(Frame, RuleInfo);
	case MR07_CTRL_CLEAR_METER:
		return MakeFrameClearMeter(Frame, RuleInfo);
	case MR07_CTRL_CLEAR_EVENT:
		return MakeFrameClearEvent(Frame, RuleInfo);
	}
	return 0;
}

int C_07MREULE::MakeFrameReadData(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr, strID;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);
	String2HEX(strID, RuleInfo.m_DataID);
	ForEachAdd(strID, 0x33);//��3��
	ConvertNetByteOrder(strID);

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR07_CTRL_READ_DATA);//�����֣���
	Frame.push_back(0x04);//�������
	Frame.append(strID);//������ID
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_07MREULE::MakeFrameWritData(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr, strID, strOP, strPW;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	String2HEX(strID, RuleInfo.m_DataID);
	ForEachAdd(strID, 0x33);//��3��
	ConvertNetByteOrder(strID);

	String2HEX(strPW, RuleInfo.m_Password);
	ForEachAdd(strPW, 0x33);//��3��
	ConvertNetByteOrder(strPW);

	String2HEX(strOP, RuleInfo.m_OpCode);
	ForEachAdd(strOP, 0x33);//��3��
	ConvertNetByteOrder(strOP);

	strData.clear();
	String2HEX(strData, RuleInfo.m_data);
	ForEachAdd(strData, 0x33);//��3��
	ConvertNetByteOrder(strData);

	if (strOP.size() != 4 || strPW.size() != 4 || strData.size() == 0)
		return 0;

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR07_CTRL_WRITE_DATA);//�����֣�д
	Frame.push_back((char)(0x04*3+strData.size()));//�������
	Frame.append(strID);//������ID
	Frame.append(strPW.end()-1, strPW.end());//PA
	Frame.append(strPW.begin(), strPW.end()-1);//P0P1P2
	Frame.append(strOP);//C0C1C2C3
	Frame.append(strData);//����
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_07MREULE::MakeFrameReadAddr(string &Frame, S_MRULEINFO &RuleInfo)
{
	Frame.push_back(0x68);//֡ͷ
	Frame.append((size_t)6, (char)0xAA);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR07_CTRL_READ_ADDR);
	Frame.push_back(0x00);//�������
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_07MREULE::MakeFrameWritAddr(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ForEachAdd(strAddr, 0x33);//��3��
	ConvertNetByteOrder(strAddr);

	Frame.push_back(0x68);//֡ͷ
	Frame.append((size_t)6, (char)0xAA);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR07_CTRL_WRITE_ADDR);
	Frame.push_back(0x06);//�������
	Frame.append(strAddr);//Ԥ�ڵ�ַ
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_07MREULE::MakeFrameAdjsTime(string &Frame, S_MRULEINFO &RuleInfo)
{
	char BcdBuf[8];
	string strData, strAddr;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	string strTime(Time2BCDStr(time(NULL), BcdBuf), 0x06);
	ForEachAdd(strTime, 0x33);//��3��

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR07_CTRL_ADJUST_TIME);
	Frame.push_back(0x06);//�������
	Frame.append(strTime);//��ǰʱ��
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_07MREULE::MakeFrameFreezCmd(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr, strFreezeTime = RuleInfo.m_FreezeTime;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	ForEachAdd(strFreezeTime, 0x33);//��3��
	ConvertNetByteOrder(strFreezeTime);

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR07_CTRL_FREEZE_CMD);
	Frame.push_back(0x04);//�������
	Frame.append(strFreezeTime);//Ԥ�ڵ�ַ
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_07MREULE::MakeFrameChangePW(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr, strID, strPW;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	String2HEX(strID, RuleInfo.m_DataID);
	ForEachAdd(strID, 0x33);//��3��
	ConvertNetByteOrder(strID);

	String2HEX(strPW, RuleInfo.m_Password);
	ForEachAdd(strPW, 0x33);//��3��
	ConvertNetByteOrder(strPW);

	strData.clear();
	String2HEX(strData, RuleInfo.m_data);
	ForEachAdd(strData, 0x33);//��3��
	ConvertNetByteOrder(strData);

	if (strData.size() != 4 || strPW.size() != 4)
		return 0;

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR07_CTRL_CHANGE_PW);
	Frame.push_back(0x0C);//�������
	Frame.append(strID);//������ID
	Frame.append(strPW.end()-1, strPW.end());//PA
	Frame.append(strPW.begin(), strPW.end()-1);//P0P1P2
	Frame.append(strData.end()-1, strData.end());//New PA
	Frame.append(strData.begin(), strData.end()-1);//New P0P1P2
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_07MREULE::MakeFrameClearNeed(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr, strOP, strPW;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	String2HEX(strPW, RuleInfo.m_Password);
	ForEachAdd(strPW, 0x33);//��3��
	ConvertNetByteOrder(strPW);

	String2HEX(strOP, RuleInfo.m_OpCode);
	ForEachAdd(strOP, 0x33);//��3��
	ConvertNetByteOrder(strOP);

	if (strOP.size() != 4 || strPW.size() != 4)
		return 0;

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR07_CTRL_CLEAR_NEED);//�����֣�д
	Frame.push_back(0x08);//�������
	Frame.append(strPW.end()-1, strPW.end());//PA
	Frame.append(strPW.begin(), strPW.end()-1);//P0P1P2
	Frame.append(strOP);//C0C1C2C3
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_07MREULE::MakeFrameClearMeter(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr, strOP, strPW;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	String2HEX(strPW, RuleInfo.m_Password);
	ForEachAdd(strPW, 0x33);//��3��
	ConvertNetByteOrder(strPW);

	String2HEX(strOP, RuleInfo.m_OpCode);
	ForEachAdd(strOP, 0x33);//��3��
	ConvertNetByteOrder(strOP);

	if (strOP.size() != 4 || strPW.size() != 4)
		return 0;

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR07_CTRL_CLEAR_METER);//�����֣�д
	Frame.push_back(0x08);//�������
	Frame.append(strPW.end()-1, strPW.end());//PA
	Frame.append(strPW.begin(), strPW.end()-1);//P0P1P2
	Frame.append(strOP);//C0C1C2C3
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_07MREULE::MakeFrameClearEvent(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr, strOP, strPW;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	String2HEX(strPW, RuleInfo.m_Password);
	ForEachAdd(strPW, 0x33);//��3��
	ConvertNetByteOrder(strPW);

	String2HEX(strOP, RuleInfo.m_OpCode);
	ForEachAdd(strOP, 0x33);//��3��
	ConvertNetByteOrder(strOP);

	strData.clear();
	String2HEX(strData, RuleInfo.m_data);
	ForEachAdd(strData, 0x33);//��3��
	ConvertNetByteOrder(strData);

	if (strOP.size() != 4 || strPW.size() != 4 || strData.size() != 4)
		return 0;

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR07_CTRL_CLEAR_EVENT);//�����֣�д
	Frame.push_back(0x0C);//�������
	Frame.append(strPW.end()-1, strPW.end());//PA
	Frame.append(strPW.begin(), strPW.end()-1);//P0P1P2
	Frame.append(strOP);//C0C1C2C3
	Frame.append(strData);//����DI0DI1DI2DI3
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

//DLTg45-1997
C_97MREULE::C_97MREULE(void):C_MRULE(C_MRULELOAD::m_MeterRule[MRULE_645_1997])
{
	m_pCtrlCode = Meter97CtrlCode;
	m_pMSetName = "����97��߼�����";
}

BOOL C_97MREULE::EnableFreezeWnd(int CtrlSN)
{
//	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return false;
}

BOOL C_97MREULE::EnableOpCodeWnd(int CtrlSN)
{
//	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return false;
}

BOOL C_97MREULE::EnablePasswordWnd(int CtrlSN)
{
	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return (CtrlCode == MR97_CTRL_CHANGE_PW);
}

BOOL C_97MREULE::EnableWriteDataWnd(int CtrlSN)
{
	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return (CtrlCode == MR97_CTRL_WRITE_DATA
		|| CtrlCode == MR97_CTRL_CHANGE_PW);
}

BOOL C_97MREULE::EnableAddressWnd(int CtrlSN)
{
//	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return true;
}

BOOL C_97MREULE::EnableDataIDWnd(int CtrlSN)
{
	unsigned char CtrlCode = m_pCtrlCode[CtrlSN].m_CtrlCode;
	return (CtrlCode == MR97_CTRL_READ_DATA
		|| CtrlCode == MR97_CTRL_WRITE_DATA);
}

int C_97MREULE::MakeMeterRuleFrame(string &Frame, unsigned char CtrlCode, S_MRULEINFO &RuleInfo)
{
	switch (CtrlCode)
	{
	case MR97_CTRL_READ_DATA:
		return MakeFrameReadData(Frame, RuleInfo);
	case MR97_CTRL_WRITE_DATA:
		return MakeFrameWritData(Frame, RuleInfo);
	case MR97_CTRL_ADJUST_TIME:
		return MakeFrameAdjsTime(Frame, RuleInfo);
	case MR97_CTRL_WRITE_ADDR:
		return MakeFrameWritAddr(Frame, RuleInfo);
	case MR97_CTRL_CHANGE_PW:
		return MakeFrameChangePW(Frame, RuleInfo);
	case MR97_CTRL_CLEAR_NEED:
		return MakeFrameClearNeed(Frame, RuleInfo);
	}
	return 0;
}

int C_97MREULE::MakeFrameReadData(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr, strID;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	String2HEX(strID, RuleInfo.m_DataID);
	ForEachAdd(strID, 0x33);//��3��
	ConvertNetByteOrder(strID);

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR97_CTRL_READ_DATA);
	Frame.push_back(0x02);//�������
	Frame.append(strID);//������ID
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_97MREULE::MakeFrameWritData(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr, strID;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	String2HEX(strID, RuleInfo.m_DataID);
	ForEachAdd(strID, 0x33);//��3��
	ConvertNetByteOrder(strID);

	strData.clear();
	String2HEX(strData, RuleInfo.m_data);
	ForEachAdd(strData, 0x33);//��3��
	ConvertNetByteOrder(strData);

	if (strData.size() == 0)
		return 0;

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR97_CTRL_WRITE_DATA);
	Frame.push_back((char)(0x02 + strData.size()));//�������
	Frame.append(strID);//������ID
	Frame.append(strData);//����
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_97MREULE::MakeFrameWritAddr(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ForEachAdd(strAddr, 0x33);//��3��
	ConvertNetByteOrder(strAddr);

	Frame.push_back(0x68);//֡ͷ
	Frame.append((size_t)6, (char)0x99);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR97_CTRL_WRITE_ADDR);
	Frame.push_back(0x06);//�������
	Frame.append(strAddr);//Ԥ�ڵ�ַ
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_97MREULE::MakeFrameAdjsTime(string &Frame, S_MRULEINFO &RuleInfo)
{
	char BcdBuf[8];
	string strData, strAddr;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	string strTime(Time2BCDStr(time(NULL), BcdBuf), 0x06);
	ForEachAdd(strTime, 0x33);//��3��

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR97_CTRL_ADJUST_TIME);
	Frame.push_back(0x06);//�������
	Frame.append(strTime);//��ǰʱ��
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_97MREULE::MakeFrameChangePW(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr, strPW;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	String2HEX(strPW, RuleInfo.m_Password);
	ForEachAdd(strPW, 0x33);//��3��
	ConvertNetByteOrder(strPW);

	strData.clear();
	String2HEX(strData, RuleInfo.m_data);
	ForEachAdd(strData, 0x33);//��3��
	ConvertNetByteOrder(strData);

	if (strData.size() != 4 || strPW.size() != 4)
		return 0;

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR97_CTRL_CHANGE_PW);
	Frame.push_back(0x08);//�������
	Frame.append(strPW.end()-1, strPW.end());//PA
	Frame.append(strPW.begin(), strPW.end()-1);//P0P1P2
	Frame.append(strData.end()-1, strData.end());//New PA
	Frame.append(strData.begin(), strData.end()-1);//New P0P1P2
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

int C_97MREULE::MakeFrameClearNeed(string &Frame, S_MRULEINFO &RuleInfo)
{
	string strData, strAddr;

	FillMeterAddress(strData, RuleInfo.m_MeterAddr);
	String2HEX(strAddr, strData);
	ConvertNetByteOrder(strAddr);

	Frame.push_back(0x68);//֡ͷ
	Frame.append(strAddr);//��ַ
	Frame.push_back(0x68);//֡ͷ
	Frame.push_back(MR97_CTRL_CLEAR_NEED);
	Frame.push_back(0x00);//�������
	Frame.push_back(CheckSum(Frame));//check sum
	Frame.push_back(0x16);//֡β

	return (int)Frame.size();
}

//LGR Rule
C_LGRMREULE::C_LGRMREULE(void):C_MRULE(C_MRULELOAD::m_MeterRule[MRULE_LGR])
{
	m_pCtrlCode = NULL;
	m_pMSetName = "��������߼�����";
}

int C_LGRMREULE::MakeMeterRuleFrame(string &Frame, unsigned char CtrlCode, S_MRULEINFO &RuleInfo)
{
	if (RuleInfo.m_DataID.compare("21") == 0)
	{
		Frame.push_back('/');
		Frame.push_back('?');
		for (unsigned int i = 0; i < RuleInfo.m_MeterAddr.size(); i++)
			Frame.push_back(RuleInfo.m_MeterAddr[i]);
		Frame.push_back('!');
		Frame.push_back('\r');
		Frame.push_back('\n');

		return (int)Frame.size();
	}
	else if (RuleInfo.m_DataID.compare("0631") == 0)
	{
		Frame.push_back(0x06);
		Frame.push_back('0');
		Frame.push_back('5');
		Frame.push_back('0');
		Frame.push_back('\r');
		Frame.push_back('\n');

		return (int)Frame.size();
	}

	return 0;
}


