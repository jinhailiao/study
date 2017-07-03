//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			mscmd.cpp
// Description:		master station command
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2010-04-28  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "mscmd.h"
#include "msrule.h"

C_PNFN::C_PNFN(string &PnFn)
{
	m_pncnt = 0;
	m_fncnt = 0;
	m_da[0] = PnFn[0], m_da[1] = PnFn[1];
	m_dt[0] = PnFn[2], m_dt[1] = PnFn[3];

	for (int i = 0; i < 8; ++i)
	{
		m_pn[i] = 0;//init
		if ((m_da[0] >> i) & 0x01)
			m_pn[m_pncnt++] = MakePnValue(1<<i, m_da[1]);
	}

	for (int i = 0; i < 8; ++i)
	{
		m_fn[i] = 0;//init
		if ((m_dt[0] >> i) & 0x01)
			m_fn[m_fncnt++] = MakeFnValue(1<<i, m_dt[1]);
	}

	if (m_fncnt && !m_pncnt)//case P0
		m_pncnt = 1;
}

S_WORD C_PNFN::GetDA(S_WORD PN)
{
	S_WORD DA = 0;

	if (PN != 0)
	{
		PN -= 1;
		DA = (((PN/8+1)<<8)&0xFF00) | ((1<<(PN%8))&0x00FF);
	}

	return DA;
}

S_WORD C_PNFN::GetDT(S_WORD FN)
{
	S_WORD DT = 0;
	if (FN != 0)
	{
		FN -= 1;
		DT = (((FN/8)<<8)&0xFF00) | ((1<<(FN%8))&0x00FF);
	}
	return DT;
}

int C_PNFN::MakePnValue(unsigned char DataPoint, unsigned char DataGroup)
{
	int i;

	if (DataPoint==0 || DataGroup==0)
		return 0;
	
	for (i = 0; i < 8; i++)
	{//������Ϣ��
		if ((DataPoint>>i)&0x01)
			break;
	}

	return (int)(DataGroup-1)*8+(i+1);
}

int C_PNFN::MakeFnValue(unsigned char DataPoint, unsigned char DataGroup)
{
	int i;

	if (DataPoint == 0)
		return 0;
	
	for (i = 0; i < 8; i++)
	{//������Ϣ��
		if ((DataPoint>>i)&0x01)
			break;
	}

	return (int)DataGroup*8+(i+1);
}

bool C_PNFN::ComparePn(string &dstStr, string &srcStr)
{//Pn��ͬFn����ͬ
	if (dstStr[0]==srcStr[0] && dstStr[1]==srcStr[1] //Pn��ͬ
		&& dstStr[3] == srcStr[3]) //Fn����ͬ
		return true;
	return false;
}

bool C_PNFN::CompareFn(string &dstStr, string &srcStr)
{//Fn��ͬPn����ͬ
	if (dstStr[2]==srcStr[2] && dstStr[3]==srcStr[3] //Fn��ͬ
		&& dstStr[1] == srcStr[1]) //Pn����ͬ
		return true;
	return false;
}

bool C_PNFN::CombinePnFn(string &dstData, strings &srcPnFnData)
{
	strings strsComb;
	string strComb;
	//�ϲ�Pn
	for (strings::iterator iterStr = srcPnFnData.begin(); iterStr < srcPnFnData.end(); iterStr++)
	{
		string &onePnFn = *iterStr;
		if (onePnFn.size() <= 4)
			continue;

		if (strComb.size() == 0)//��һ��
		{
			strComb.append(onePnFn);
		}
		else if (ComparePn(strComb, onePnFn) == true)//�ϲ�
		{
			strComb[2] = strComb[2]|onePnFn[2];//�ϲ�Fn��
			strComb.append(onePnFn.begin()+4, onePnFn.end());
		}
		else//�洢ǰһ���ϲ������¿�һ������
		{
			strsComb.push_back(strComb);
			strComb = onePnFn;
		}
	}
	strsComb.push_back(strComb);
	strComb.clear();
	//�ϲ�FN
	for (strings::iterator iterStr = strsComb.begin(); iterStr < strsComb.end(); iterStr++)
	{
		string &onePnFn = *iterStr;
		if (onePnFn.size() <= 4)
			continue;

		if (strComb.size() == 0)//��һ��
		{
			strComb.append(onePnFn);
		}
		else if (CompareFn(strComb, onePnFn) == true)//�ϲ�
		{
			strComb[0] = strComb[0]|onePnFn[0];//�ϲ�Fn��
			strComb.append(onePnFn.begin()+4, onePnFn.end());
		}
		else//�洢ǰһ���ϲ������¿�һ������
		{
			dstData.append(strComb);
			strComb = onePnFn;
		}
	}

	dstData.append(strComb);

	return true;
}

/** @brief �����ݵ�Ԫ��ʶ��ֳɶ�PN��FN��ʽ
  *  @param[in] DaDt ���ݵ�Ԫ��ʶ
  *  @param[out] PnFns ��PN��FN��ʽ��
  *  @return ��ֺ�ĸ���
  */
int C_PNFN::PnFnParseMultPnOneFn(const string &DaDt, strings &PnFns)
{
	string tmp(DaDt);
	S_BYTE DA[2], DT[2];
	DA[0] = DaDt[0], DA[1] = DaDt[1];
	DT[0] = DaDt[2], DT[1] = DaDt[3];

	for (int i = 0; i < 8; ++i)
	{
		if ((DT[0] >> i) & 0x01)//������Ԫ��λ
		{
			tmp[2] = (S_BYTE)(1 << i);
			PnFns.push_back(tmp);
		}
	}

	return (int)(PnFns.size());
}

int C_PNFN::PnFnParseOnePnOneFn(string &srcPnFn, strings &dstPnFn)
{
	unsigned char DA1 = srcPnFn[0];
	unsigned char DA2 = srcPnFn[1];
	unsigned char DT1 = srcPnFn[2];
	unsigned char DT2 = srcPnFn[3];

	if (DA1 == 0 || DA2 == 0)
	{
		for (int i = 0; i < 8; i++)
		{
			if ((DT1>>i)&0x01)
			{
				string onePnFn;
				onePnFn.push_back(DA1);
				onePnFn.push_back(DA2);
				onePnFn.push_back((unsigned char)(1<<i));
				onePnFn.push_back(DT2);
				dstPnFn.push_back(onePnFn);
			}
		}
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			if ((DA1>>i)&0x01)
			{
				for (int j = 0; j < 8; j++)
				{
					if ((DT1>>j)&0x01)
					{
						string onePnFn;
						onePnFn.push_back((unsigned char)(1<<i));
						onePnFn.push_back(DA2);
						onePnFn.push_back((unsigned char)(1<<j));
						onePnFn.push_back(DT2);
						dstPnFn.push_back(onePnFn);
					}
				}
			}
		}
	}

	return (int)(dstPnFn.size());
}

int C_AFNOBJ::AfnDataProc(const string &PnfnData, strings &strReplys)
{
	strings strMyReplys;

	beg = PnfnData.begin();
	end = PnfnData.end();

	int r = PnfnProc(strMyReplys);

	string strTemp;
	for (size_t i = 0; i < strMyReplys.size(); i++)
	{
		strTemp.append(strMyReplys[i]);
		if (strTemp.size() > MSRULE.GetPacketMax())
		{
			strReplys.push_back(strTemp);
			strTemp.clear();
		}
	}
	if (!strTemp.empty())
		strReplys.push_back(strTemp);

	return r;
}

int C_AFNOBJ::PnfnProc(strings &strReplys)
{
	C_FNOBJ *pFnObj;
	string strPNFN(beg, beg+4);
	string strDataUnit(beg+4, end);
	C_PNFN Pnfn(strPNFN);

	if ((pFnObj = GetFnObj(Pnfn.GetFN(0))) != NULL)
		return pFnObj->FnProc(Pnfn.GetPN(0), strDataUnit, strReplys);

	return CMDPARSE_ERROR;
}



int C_CONF_CMD::PnfnProc(strings &strReplys)
{
	string strPnFn(beg, beg+4);
	beg += 4;

	C_PNFN pnfn(strPnFn);
	switch (pnfn.GetFN(0))
	{
	case 1:return CMDPARSE_CONFIRM;
	case 2:return CMDPARSE_DENY;
	case 3:return ConfirmF3Parse(beg, end, strReplys);
	}

	return CMDPARSE_ERROR;
}

int C_CONF_CMD::ConfirmF3Parse(string::const_iterator &beg, string::const_iterator &end, strings &strReplys)
{
	if (end - beg < 6)
		return CMDPARSE_ERROR;
	S_CHAR AFN = *beg++;
	while (end - beg >= 5)
	{
		string strF3;
		strF3.push_back(AFN);
		strF3.append(beg, beg+4);
		if (*(beg+4) == 0x00)//right
			strF3.push_back(CMDPARSE_CONFIRM);
		else
			strF3.push_back(CMDPARSE_DENY);
		beg += 5;
		strReplys.push_back(strF3);
	}
	return CMDPARSE_CONFIRM_F3;
}

int C_RESETCMD::PnfnProc(strings &strReplys)
{
	if (end - beg < 4)
		return CMDPARSE_ERROR;

	string strPnFn(beg, beg+4);
	beg += 4;

	C_PNFN pnfn(strPnFn);
	switch (pnfn.GetFN(0))
	{
	case 1:return CMDPARSE_MAKE_CONFIRM;
	case 2:return CMDPARSE_MAKE_CONFIRM;
	case 3:return CMDPARSE_MAKE_CONFIRM;
	case 4:return CMDPARSE_MAKE_CONFIRM;
	}

	return CMDPARSE_ERROR;
}



