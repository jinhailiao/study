/** @file
 *  @brief 国网规约解析
 *  @author jinhailiao
 *  @date 2012/05/04
 *  @version 0.1
 */
#ifndef __GW_MS_RULE_H__
#define __GW_MS_RULE_H__
#include "basedef.h"
#include "frame.h"
/** @brief 字节内位数统计 */
static char C_BITSET_ByteBitCnt[] =
{
	0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

/** @brief 位处理类
 *  @note
 *  与STL bitset区别:1,类型参数是字节数; 2,可以返回位数据; 3,缓冲按BYTE存储
 */
template <int ByteCnt>
class C_BITSET
{
public:
	C_BITSET(void){ClearBit();}
	C_BITSET(const unsigned char *pString){ClearBit();ParseStr(pString);}
	C_BITSET(const unsigned char *pData, int Bytes){ClearBit();SetBit(pData, Bytes);}
	C_BITSET(const C_BITSET &r){if (this != &r) SetBit(r.m_data, sizeof(r.m_data));}
	~C_BITSET(){}

	/** @brief 取字节数 */
	int GetByteCnt(void)const{return ByteCnt;}
	/** @brief 取位数 */
	int GetBitSize(void)const{return GetByteCnt()*8;}
	/** @brief 取位缓冲 */
	const unsigned char *GetData(void)const{return m_data;}
	/** @brief 取位缓冲 */
	unsigned char *GetData(void){return m_data;}

	/** @brief pos位置0 */
	void ClearBit(int pos){m_data[pos/8] &= (unsigned char)(~(1<<(pos%8)));}
	/** @brief 全部位置0 */
	void ClearBit(void){memset(m_data, 0x00, sizeof(m_data));}
	/** @brief 测试pos位是否为1 */
	bool TestBit(int pos)const{return (m_data[pos/8] & (1<<(pos%8)))?true:false;}
	/** @brief 按pData缓冲设置位 */
	void SetBit(const unsigned char *pData, int Bytes){memcpy(m_data, pData, Bytes);}
	/** @brief pos位置1 */
	void SetBit(int pos){m_data[pos/8] |= (unsigned char)(1<<(pos%8));}
	/** @brief 全部位置1 */
	void SetBit(void){memset(m_data, 0xFF, sizeof(m_data));}
	/** @brief 设置from-to区间为1 */
	void SetBit(int from, int to){for (; from <= to; from++) SetBit(from);}

	/** @brief 第一个有效位*/
	int FirstBit(void)const{return _NextBit(0);}
	/** @brief 下一个有效位*/
	int NextBit(int prev)const{return _NextBit(prev+1);}

	/** @brief 统计置1的位数 */
	int CountBit(void)const
	{
		int AllBit = 0;
		for (int i = 0; i < ByteCnt; i++)
			AllBit += C_BITSET_ByteBitCnt[m_data[i]];
		return AllBit;
	}

protected:
	/** @brief 寻找有效位*/
	int _NextBit(int prev)const
	{
		for (; prev < GetBitSize(); prev++)
		{
			if (TestBit(prev) == true) return prev;
		}
		return GetBitSize();
	}
protected:
	unsigned char m_data[ByteCnt]; ///< 位缓冲
};

//
//DA解析
//
class C_DA
{
public:
	C_DA(INT16U DA):m_DA1((INT8U*)&DA, 1),m_DA2(DA>>8){}
	~C_DA(){}
	
	/** @brief MP统计
	 *  @return 0 终端测量点
	 *  @return 1-8 表计等测量点
	 *  @return 0xFF 所有测量点
	 */
	INT8U GetMpCount(void);
	void ResetMP(int pos){m_DA1.ClearBit(pos);}
	/** @brief MP计算 */
	bool TestMP(int pos);
	INT16U GetMP(int pos);

	INT8U GetDA1(void){INT8U *p = m_DA1.GetData(); return *p;}
	INT8U GetDA2(void){return m_DA2;}
	INT16U GetDA(void){INT8U *p = m_DA1.GetData(); return (m_DA2<<8)|*p;}

private:
	C_BITSET<1> m_DA1;
	INT8U m_DA2;
};

//
//DT 解析
//
class C_DT
{
public:
	C_DT(S_WORD DT):m_DT1((S_BYTE*)&DT, 1),m_DT2(DT>>8){}
	~C_DT(){}
	
	/** @brief 功能点统计
	 *  @return 0 无功能点
	 *  @return 1-8 功能点置位
	 */
	S_BYTE GetFnCount(void){return m_DT1.CountBit();}
	void ResetFN(int pos){m_DT1.ClearBit(pos);}
	/** @brief 功能点计算 pos[0-7]*/
	bool TestFN(int pos){return m_DT1.TestBit(pos);}
	S_WORD GetFN(int pos){return m_DT2*8 + (pos+1);}

	S_BYTE GetDT1(void){INT8U *p = m_DT1.GetData(); return *p;}
	S_BYTE GetDT2(void){return m_DT2;}
	S_WORD GetDT(void){INT8U *p = m_DT1.GetData(); return (m_DT2<<8)|*p;}

private:
	C_BITSET<1> m_DT1;
	S_BYTE m_DT2;
};

class C_DAPN
{
public:
	/** @brief 将PN转换成DA
	 * @param[in] PN 测量点号
	 * @return 有效DA
	 */
	static S_WORD PN2DA(S_WORD PN);
	/** @brief 将DA转换成PN
	 * @param[in] DA 信息点
	 * @return 有效PN
	 */
	static S_WORD DA2PN(S_WORD DA);
};

class C_DTFN
{
public:
	/** @brief 将FN转换成DT
	 * @param[in] FN 信息功能点
	 * @return 有效DT
	 */
	static S_WORD FN2DT(S_WORD FN);
	/** @brief 将DT转换成FN
	 * @param[in] DT 信息类
	 * @return 有效FN
	 */
	static S_WORD DT2FN(S_WORD DT);
};

#define GW_PROTOCOL_FLAG		2

class C_CtrlCode
{
public:
	C_CtrlCode(void){m_CtrlCode = 0x00; m_EC1 = 0x00; m_EC2 = 0x00;}
	C_CtrlCode(INT8U code){m_CtrlCode = code; m_EC1 = 0x00; m_EC2 = 0x00;}
	~C_CtrlCode(){}

public:
	INT8U GetAll(void){return m_CtrlCode;}
	INT8U GetDIR(void){return (m_CtrlCode>>7)&0x01;}
	INT8U GetPRM(void){return (m_CtrlCode>>6)&0x01;}
	INT8U GetFCB(void){return (m_CtrlCode>>5)&0x01;}
	INT8U GetFCV(void){return (m_CtrlCode>>4)&0x01;}
	INT8U GetFUN(void){return (m_CtrlCode>>0)&0x0F;}
	INT8U GetACD(void){return (m_CtrlCode>>5)&0x01;}
	INT8U GetEC1(void){return m_EC1;}
	INT8U GetEC2(void){return m_EC2;}
	INT8U GetPrimaryFun(INT8U AFN);
	INT8U GetSlaveFun(INT8U PreAFN, INT8U NowAFN);

	void SetDIR(INT8U dir){m_CtrlCode = dir==0?(m_CtrlCode&0x7F):(m_CtrlCode|0x80);}
	void SetPRM(INT8U prm){m_CtrlCode = prm==0?(m_CtrlCode&0xBF):(m_CtrlCode|0x40);}
	void SetACD(INT8U acd){m_CtrlCode = acd==0?(m_CtrlCode&0xDF):(m_CtrlCode|0x20);}
	void SetFUN(INT8U fun){m_CtrlCode = (m_CtrlCode&0xF0)|(fun&0x0F);}
	void SetEC(S_BYTE EC1, S_BYTE EC2){m_EC1 = EC1; m_EC2 = EC2;}

private:
	INT8U m_CtrlCode;
	INT8U m_EC1;
	INT8U m_EC2;
};

class C_RTUA
{
public:
	C_RTUA(void){SetAreaCode(0),SetDevAddr(0),SetMSA(0),SetGroup(0);}
	C_RTUA(S_DWORD rtua){SetRTUA(rtua),SetMSA(0),SetGroup(0);}
	C_RTUA(S_WORD AreaCode, S_WORD DevAddr, S_BYTE MSA=0, S_BYTE Group=0){SetAreaCode(AreaCode),SetDevAddr(DevAddr),SetMSA(MSA),SetGroup(Group);}

	S_DWORD GetRTUA(void)const{return KGE_COMBDWORD(m_DevAddr, m_AreaCode);}
	S_WORD GetAreaCode(void)const{return m_AreaCode;}
	S_WORD GetDevAddr(void)const{return m_DevAddr;}
	S_BYTE GetMSA(void)const{return m_MSA;}
	S_BYTE GetGroup(void)const{return m_Group;}

	void SetRTUA(S_DWORD rtua){m_AreaCode=KGE_GETLOWORD(rtua),m_DevAddr=KGE_GETHIWORD(rtua);}
	void SetAreaCode(S_WORD AreaCode){m_AreaCode = AreaCode;}
	void SetDevAddr(S_WORD DevAddr){m_DevAddr = DevAddr;}
	void SetMSA(S_BYTE MSA){m_MSA = MSA;}
	void SetGroup(S_BYTE Group){m_Group = Group;}

	bool operator== (C_RTUA &other)const{return m_AreaCode==other.m_AreaCode && m_DevAddr==other.m_DevAddr;}
	void ParseString(const string &strAddr);
	void PrintString(string &strAddr)const;
	void ParseAscii(const char *pRTUA);
	char *PrintAscii(char *pRTUA)const;

private:
	S_WORD m_AreaCode;///<行政区划码
	S_WORD m_DevAddr;///<装置地址
	S_BYTE m_MSA;///<主站地址
	S_BYTE m_Group; ///< 组标志
};

class C_FrameSeq
{
public:
	C_FrameSeq(void){m_SEQ=0x00;}
	C_FrameSeq(INT8U seq){m_SEQ=seq;}

public:
	INT8U GetAll(void){return m_SEQ;}
	INT8U GetTpV(void){return (m_SEQ>>7)&0x01;}
	INT8U GetFIR(void){return (m_SEQ>>6)&0x01;}
	INT8U GetFIN(void){return (m_SEQ>>5)&0x01;}
	INT8U GetCON(void){return (m_SEQ>>4)&0x01;}
	INT8U GetRSEQ(void){return (m_SEQ>>0)&0x0F;}

	void SetTpV(INT8U tpv){m_SEQ = tpv==0?(m_SEQ&0x7F):(m_SEQ|0x80);}
	void SetFIR(INT8U fir){m_SEQ = fir==0?(m_SEQ&0xBF):(m_SEQ|0x40);}
	void SetFIN(INT8U fin){m_SEQ = fin==0?(m_SEQ&0xDF):(m_SEQ|0x20);}
	void SetCON(INT8U con){m_SEQ = con==0?(m_SEQ&0xEF):(m_SEQ|0x10);}
	void SetRSEQ(INT8U seq){m_SEQ = (m_SEQ&0xF0)|(seq&0x0F);}

private:
	INT8U m_SEQ;
};

class C_FrameTP
{
public:
	C_FrameTP(void){m_timeFrameSend=0,m_IntervalM=0,m_PFC=0;}
	C_FrameTP(INT8U PFC, S_TIME timeFrameSend, INT8U IntervalM){m_PFC=PFC,m_timeFrameSend=timeFrameSend,m_IntervalM=IntervalM;}


	void ParseString(const string &strTP);
	void PrintString(string &strTP);
	bool Valid(void);

private:
	S_TIME m_timeFrameSend;
	INT8U m_IntervalM;
	INT8U m_PFC;
};


class C_MSRULE
{
public:
	C_MSRULE(int ProtocolCode){m_ProtocolCode = ProtocolCode;}
	virtual ~C_MSRULE(){}

public:
	bool CheckData(const S_FrameData &srcData, S_FrameData &dstData);
	bool SetFrameData(const string &strPassword, const string &strTermAddr, S_BYTE MasterAddr);

	/** 组织命令帧 */
	int Make(const S_AFN &AFN, C_CtrlCode &CtrlCode, C_RTUA &FrameAddr, C_FrameSeq &FrameSeq, C_FrameTP &FrameTP, string &strCmdInfo, string &strCmd);
	int MakePrimary(const S_AFN &AFN, C_FrameSeq &FrameSeq, C_FrameTP &FrameTP, string &strCmdInfo, string &strCmd);

protected:
	int m_ProtocolCode;
	S_BYTE m_MasterAddr;
	string m_strTermAddr;
	string m_strPassword;
};

#endif//__GW_MS_RULE_H__



