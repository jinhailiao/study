//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			mscmd.h
// Description:		master station command
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-04-28  Create                  Kingsea
//---------------------------------------------------------------------------------
#ifndef __MSCOMMAND_H__
#define __MSCOMMAND_H__

#include "haidef.h"

enum
{
	CMDPARSE_ERROR,
	CMDPARSE_CONFIRM,
	CMDPARSE_DENY,
	CMDPARSE_CONFIRM_F3,

	CMDPARSE_MAKE_CONFIRM,
	CMDPARSE_MAKE_DENY,
	CMDPARSE_MAKE_RESPONSE,

	CMDPARSE_RESEND,//same FCV
	CMDPARSE_MAX,
};


class C_PNFN
{
public:
	C_PNFN(string &PnFn);

	S_BYTE GetFnCnt(void) const{return m_fncnt;}
	S_BYTE GetPnCnt(void) const{return m_pncnt;}
	S_WORD GetFN(int pos) const{return m_fn[pos];}
	S_WORD GetPN(int pos) const{return m_pn[pos];}
	const S_BYTE *GetDA(void) const {return m_da;}
	const S_BYTE *GetDT(void) const {return m_dt;}

public:
	static S_WORD GetDA(S_WORD PN);
	static S_WORD GetDT(S_WORD FN);
	static bool ComparePn(string &dstStr, string &srcStr);
	static bool CompareFn(string &dstStr, string &srcStr);
	static bool CombinePnFn(string &dstData, strings &srcPnFnData);
	/** @brief 将数据单元标识拆分成多PN单FN形式 */
	static int PnFnParseMultPnOneFn(const string &DaDt, strings &PnFns);
	static int PnFnParseOnePnOneFn(string &srcPnFn, strings &dstPnFn);
	static int MakePnValue(S_BYTE DataPoint, S_BYTE DataGroup);
	static int MakeFnValue(S_BYTE DataPoint, S_BYTE DataGroup);

private:
	S_BYTE m_pncnt, m_fncnt;
	S_BYTE m_da[2], m_dt[2];
	S_WORD m_pn[8], m_fn[8];
};

class C_FNOBJ
{
public:
	virtual int FnProc(S_WORD Pn, const string &DataUnit, strings &strReplys) = 0;
};

class C_AFNOBJ
{
public:
	C_AFNOBJ(void){}
	virtual ~C_AFNOBJ(){}

public:
	virtual S_BOOL NeedPassword(void){return false;}
	virtual int AfnDataProc(const string &PnfnData, strings &strReplys);
	virtual int PnfnProc(strings &strReplys);
	virtual C_FNOBJ *GetFnObj(S_WORD FN){return NULL;}

protected:
	string::const_iterator beg, end;
};

class C_CONF_CMD:public C_AFNOBJ
{
public:
	virtual int PnfnProc(strings &strReplys);
	int ConfirmF3Parse(string::const_iterator &beg, string::const_iterator &end, strings &strReplys);
};

class C_RESETCMD:public C_AFNOBJ
{
public:
	virtual S_BOOL NeedPassword(void){return true;}
	virtual int PnfnProc(strings &strReplys);
};


#endif //__MSCOMMAND_H__
