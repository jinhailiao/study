//
//
//
#pragma once

#include <string>
#include <fstream>
#include <assert.h>
using namespace std;
#include <time.h>

const int MRULE_DATAID_MAX = 256;

enum {MRULE_645_1997, MRULE_645_2007, MRULE_LGR, MRULE_MAX};

//国标07规约控制字
#define MR07_CTRL_READ_DATA		0x11
#define MR07_CTRL_WRITE_DATA	0x14
#define MR07_CTRL_READ_ADDR		0x13
#define MR07_CTRL_WRITE_ADDR	0x15
#define MR07_CTRL_ADJUST_TIME	0x08
#define MR07_CTRL_FREEZE_CMD	0x16
#define MR07_CTRL_CHANGE_PW		0x18
#define MR07_CTRL_CLEAR_NEED	0x19
#define MR07_CTRL_CLEAR_METER	0x1A
#define MR07_CTRL_CLEAR_EVENT	0x1B
//国标97规约控制字
#define MR97_CTRL_READ_DATA		0x01
#define MR97_CTRL_WRITE_DATA	0x04
#define MR97_CTRL_ADJUST_TIME	0x08
#define MR97_CTRL_WRITE_ADDR	0x0A
#define MR97_CTRL_CHANGE_PW		0x0F
#define MR97_CTRL_CLEAR_NEED	0x10

//
typedef struct tagMDATAID
{
	string m_dataID;
	string m_regName;
}
S_MDATAID, *SP_MDATAID;

typedef struct tagMRule
{
	string m_ruleName;
	int IDCount;
	S_MDATAID m_dataIDS[MRULE_DATAID_MAX];
}
S_MRULE, *SP_MRULE;

struct S_DLTCTRLCODE
{
	const char *m_pName;
	unsigned char m_CtrlCode;
};

class C_MRULELOAD
{
public:
	enum {PMRS_START,PMRS_RULENAME,PMRS_RULEREG,PMRS_RULERNAME,PMRS_END};
	static bool LoadAll(void);

private:
	static int ParseMeterReg(const char *plinebuf, char *preg, char *pname);
	static int FindMeterRule(const char *prule);

public:
	static S_MRULE m_MeterRule[MRULE_MAX];
};

struct S_MRULEINFO
{
	string m_MeterAddr;
	string m_DataID;
	string m_OpCode;
	string m_Password;
	string m_data;
	string m_FreezeTime;
};

class C_MRULE
{
public:
	C_MRULE(S_MRULE &MeterRule);
	virtual ~C_MRULE();

public:
	const char *GetMSetName(void){return m_pMSetName;}
	const S_DLTCTRLCODE *GetCtrlCodeGroup(void){return m_pCtrlCode;}
	const char *GetMRuleName(void);
	const char *GetMRuleDataID(int ID);
	const char *GetMRuleRegName(int ID);
	int GetMRuleDataIDCount(void);
	int FillMeterAddress(string &OutAddr, const string &InAddr);

public:
	virtual BOOL EnableFreezeWnd(int CtrlSN){return false;}
	virtual BOOL EnableOpCodeWnd(int CtrlSN){return false;}
	virtual BOOL EnablePasswordWnd(int CtrlSN){return false;}
	virtual BOOL EnableWriteDataWnd(int CtrlSN){return false;}
	virtual BOOL EnableAddressWnd(int CtrlSN){return false;}
	virtual BOOL EnableDataIDWnd(int CtrlSN){return false;}

	virtual bool IsPasswordCtrlCode(int CtrlSN){return false;}
	virtual int MakeMeterRuleFrame(string &Frame, unsigned char CtrlCode, S_MRULEINFO &RuleInfo) = 0;

public:
	static char Ascii2Digit(char ch);
	static char Digit2Ascii(char val);
	static char *BCD2String(char bcd, char *pString);
	static char String2BCD(const char *pString);
	static int String2HEX(string &strBCD, const string &strNum);
	static int ConvertNetByteOrder(string &strData);
	static int ForEachAdd(string &strData, char num);
	static unsigned int CheckSum(string &frame);
	static char* Time2BCDStr(time_t time, char *bcdStr7B);
	static char cbcd(char Hex){return ((Hex%100)/10)*16+(Hex%10);}


protected:
	S_MRULE &m_MeterRule;
	const S_DLTCTRLCODE *m_pCtrlCode;
	const char *m_pMSetName;
};

class C_07MREULE:public C_MRULE
{
public:
	C_07MREULE(void);
	~C_07MREULE(){}

public:
	virtual BOOL EnableFreezeWnd(int CtrlSN);
	virtual BOOL EnableOpCodeWnd(int CtrlSN);
	virtual BOOL EnablePasswordWnd(int CtrlSN);
	virtual BOOL EnableWriteDataWnd(int CtrlSN);
	virtual BOOL EnableAddressWnd(int CtrlSN);
	virtual BOOL EnableDataIDWnd(int CtrlSN);

	virtual bool IsPasswordCtrlCode(int CtrlSN){return m_pCtrlCode[CtrlSN].m_CtrlCode==MR07_CTRL_CHANGE_PW?true:false;}
	virtual int MakeMeterRuleFrame(string &Frame, unsigned char CtrlCode, S_MRULEINFO &RuleInfo);

protected:
	int MakeFrameReadData(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameWritData(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameReadAddr(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameWritAddr(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameAdjsTime(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameFreezCmd(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameChangePW(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameClearNeed(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameClearMeter(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameClearEvent(string &Frame, S_MRULEINFO &RuleInfo);
};

class C_97MREULE:public C_MRULE
{
public:
	C_97MREULE(void);
	~C_97MREULE(){}

public:
	virtual BOOL EnableFreezeWnd(int CtrlSN);
	virtual BOOL EnableOpCodeWnd(int CtrlSN);
	virtual BOOL EnablePasswordWnd(int CtrlSN);
	virtual BOOL EnableWriteDataWnd(int CtrlSN);
	virtual BOOL EnableAddressWnd(int CtrlSN);
	virtual BOOL EnableDataIDWnd(int CtrlSN);

	virtual bool IsPasswordCtrlCode(int CtrlSN){return m_pCtrlCode[CtrlSN].m_CtrlCode==MR97_CTRL_CHANGE_PW?true:false;}
	virtual int MakeMeterRuleFrame(string &Frame, unsigned char CtrlCode, S_MRULEINFO &RuleInfo);

protected:
	int MakeFrameReadData(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameWritData(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameWritAddr(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameAdjsTime(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameChangePW(string &Frame, S_MRULEINFO &RuleInfo);
	int MakeFrameClearNeed(string &Frame, S_MRULEINFO &RuleInfo);
};

class C_LGRMREULE:public C_MRULE
{
public:
	C_LGRMREULE(void);
	~C_LGRMREULE(){}

public:
	virtual BOOL EnableAddressWnd(int CtrlSN){return true;}
	virtual BOOL EnableDataIDWnd(int CtrlSN){return true;}

	virtual bool IsPasswordCtrlCode(int CtrlSN){return false;}
	virtual int MakeMeterRuleFrame(string &Frame, unsigned char CtrlCode, S_MRULEINFO &RuleInfo);
};

extern const char *pMeterRule[];
extern C_07MREULE MRule07;
extern C_97MREULE MRule97;
extern C_LGRMREULE MRuleLgr;
extern C_MRULE *pCurMrule;

