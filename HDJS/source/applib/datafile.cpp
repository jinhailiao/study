/**
 *  @file datafile.cpp 
 *  @brief 集中器能量数据文件存取操作集
 *  @author jinhailiao
 *  @date 2009-11-24
 *  @version v1.0
 *  @note
 *  1、同一个FN费率须以最大的那个为准，可以依FN不同
 *  2、同一个重点户FN数据点数只能固定为某个数据值（24的倍数）。可以依FN不同
 *  3、本数据文件存取的是国网上行规约定义的二类数据。
 */
#include "datafile.h"
#include "haidef.h"
#include "timeop.h"
#include "fileop.h"

/** @brief 二类数据最大长度
  *  @note
  *   0 表示无效值，出现此值应注意
  *  -1 表示此项不须关注，以实际为准
  *  其它为数据长度对齐值
  */
const short Class2FnLen[] =
{
	 0,//F0 无效FN
	//组1  电能示值、最大需量及电能量 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F1 正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12） 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F2 反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12） 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F3 正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12） 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F4 反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12） 
	(1+4+4*GW_TARIFF_MAX),//F5  正向有功电能量（总、费率1～M） 
	(1+4+4*GW_TARIFF_MAX),//F6  正向无功电能量（总、费率1～M） 
	(1+4+4*GW_TARIFF_MAX),//F7  反向有功电能量（总、费率1～M） 
	(1+4+4*GW_TARIFF_MAX),//F8  反向无功电能量（总、费率1～M） 
	//组2  电能示值、最大需量 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F9 正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12） 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F10 反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12） 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F11 正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12） 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F12 反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12） 
	0,0,0,0,//F13～F16  备用 
	//组3  电能示值、最大需量及电能量 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F17 正向有/无功电能示值、一/四象限无功电能示值（总、费率1～M，1≤M≤12） 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F18 反向有/无功电能示值、二/三象限无功电能示值（总、费率1～M，1≤M≤12） 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F19 正向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12） 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F20 反向有/无功最大需量及发生时间（总、费率1～M，1≤M≤12） 
	(1+4+4*GW_TARIFF_MAX),//F21  正向有功电能量（总、费率1～M） 
	(1+4+4*GW_TARIFF_MAX),//F22  正向无功电能量（总、费率1～M） 
	(1+4+4*GW_TARIFF_MAX),//F23  反向有功电能量（总、费率1～M） 
	(1+4+4*GW_TARIFF_MAX),//F24  反向无功电能量（总、费率1～M） 
	//组4   
	-1,//F25 日总及分相最大有功功率及发生时间、有功功率为零时间 
	-1,//F26  日总及分相最大需量及发生时间 
	-1,//F27  日电压统计数据 
	-1,//F28  日不平衡度越限累计时间 
	-1,//F29  日电流越限统计 
	-1,//F30  日视在功率越限累计时间 
	-1,//F31  日负载率率统计 
	-1,//F32  日电能表断相数据 
	//组5   
	-1,//F33 月总及分相最大有功功率及发生时间、有功功率为零时间 
	-1,//F34  月总及分相有功最大需量及发生时间 
	-1,//F35  月电压统计数据 
	-1,//F36  月不平衡度越限累计时间 
	-1,//F37  月电流越限统计 
	-1,//F38  月视在功率越限累计时间 
	-1,//F39  月负载率率统计 
	 0,//F40  备用 
	//组6   
	-1,//F41  电容器投入累计时间和次数 
	-1,//F42  日、月电容器累计补偿的无功电能量 
	-1,//F43  日功率因数区段累计时间 
	-1,//F44  月功率因数区段累计时间 
	-1,//F45  铜损、铁损有功电能示值 
	-1,//F46  铜损、铁损有功电能示值 
	 0,0,//F47～F48  备用 
	//组7  终端统计数据 
	-1,//F49  终端日供电时间、日复位累计次数 
	-1,//F50  终端日控制统计数据 
	-1,//F51  终端月供电时间、月复位累计次数 
	-1,//F52  终端月控制统计数据 
	-1,//F53  终端与主站日通信流量 
	-1,//F54  终端与主站月通信流量 
	 0,0,//F55～F56  备用 
	//组8  总加组统计数据 
	-1,//F57 总加组日最大、最小有功功率及其发生时间，有功功率为零日累计时间 
	(1+4+4*GW_TARIFF_MAX),//F58  总加组日累计有功电能量（总、费率1～M） 
	(1+4+4*GW_TARIFF_MAX),//F59  总加组日累计无功电能量（总、费率1～M） 
	-1,//F60 总加组月最大、最小有功功率及其发生时间，有功功率为零月累计时间 
	(1+4+4*GW_TARIFF_MAX),//F61  总加组月累计有功电能量（总、费率1～M） 
	(1+4+4*GW_TARIFF_MAX),//F62  总加组月累计无功电能量（总、费率1～M） 
	 0,0,//F63～F64  备用 
	//组9  总加组越限统计数据 
	-1,//F65  总加组超功率定值的月累计时间、月累计电能量 
	-1,//F66 总加组超月电能量定值的月累计时间、累计电能量 
	 0,0,0,0,0,0,//F67～F72  备用 
	//组10  总加组曲线 
	-1,//F73  总加组有功功率曲线 
	-1,//F74  总加组无功功率曲线 
	-1,//F75  总加组有功电能量曲线 
	-1,//F76  总加组无功电能量曲线 
	 0,0,0,0,//F77～F80  备用 
	//组11  功率曲线 
	-1,//F81  有功功率曲线 
	-1,//F82  A相有功功率曲线 
	-1,//F83  B相有功功率曲线 
	-1,//F84  C相有功功率曲线 
	-1,//F85  无功功率曲线 
	-1,//F86  A相无功功率曲线 
	-1,//F87  B相无功功率曲线 
	-1,//F88  C相无功功率曲线 
	//组12  电压电流曲线 
	-1,//F89  A相电压曲线 
	-1,//F90  B相电压曲线 
	-1,//F91  C相电压曲线 
	-1,//F92  A相电流曲线 
	-1,//F93  B相电流曲线 
	-1,//F94  C相电流曲线 
	-1,//F95  零序电流曲线 
	 0,//F96  备用 
	//组13  总电能量、总电能示值曲线 
	-1,//F97  正向有功总电能量 
	-1,//F98  正向无功总电能量 
	-1,//F99  反向有功总电能量 
	-1,//F100  反向无功总电能量 
	-1,//F101  正向有功总电能示值 
	-1,//F102  正向无功总电能示值 
	-1,//F103  反向有功总电能示值 
	-1,//F104  反向无功总电能示值 
	//组14  功率因数、电压相位角曲线、电流相位角曲线 
	-1,//F105  总功率因数 
	-1,//F106  A相功率因数 
	-1,//F107  B相功率因数 
	-1,//F108  C相功率因数 
	-1,//F109  电压相位角曲线 
	-1,//F110  电流相位角曲线 
	 0,0,//F111～F112  备用 
	//组15  谐波监测统计数据 
	-1,//F113  A相2～19次谐波电流日最大值及发生时间 
	-1,//F114  B相2～19次谐波电流日最大值及发生时间 
	-1,//F115  C相2～19次谐波电流日最大值及发生时间 
	-1,//F116 A相2～19次谐波电压含有率及总畸变率日最大值及发生时间 
	-1,//F117 B相2～19次谐波电压含有率及总畸变率日最大值及发生时间 
	-1,//F118 C相2～19次谐波电压含有率及总畸变率日最大值及发生时间 
	 0,0,//F119～F120  备用 
	//组16  谐波越限统计数据 
	-1,//F121  A相谐波越限日统计数据 
	-1,//F122  B相谐波越限日统计数据 
	-1,//F123  C相谐波越限日统计数据 
	 0,0,0,0,0,//F124～F128  备用 
	//组17  直流模拟量数据 
	-1,//F129 直流模拟量越限日累计时间、最大/最小值及发生时间 
	-1,//F130 直流模拟量越限月累计时间、最大/最小值及发生时间 
	 0,0,0,0,0,0,//F131～F136  备用 
	//组18  直流模拟量数据曲线 
	 0,//F137  备用 
	-1,//F138 直流模拟量数据曲线 
	 0,0,0,0,0,0,//F139～F144  备用 
	//组19  四个象限无功总电能示值曲线 
	-1,//F145  一象限无功总电能示值曲线 
	-1,//F146  四象限无功总电能示值曲线 
	-1,//F147  二象限无功总电能示值曲线 
	-1,//F148  三象限无功总电能示值曲线 
	 0,0,0,0,//F149～F152  备用 
	//组20  分相电能示值 
	-1,//F153  日冻结分相正向有功电能示值 
	-1,//F154  日冻结分相正向无功电能示值 
	-1,//F155  日冻结分相反向有功电能示值 
	-1,//F156  日冻结分相反向无功电能示值 
	-1,//F157  月冻结分相正向有功电能示值 
	-1,//F158  月冻结分相正向无功电能示值 
	-1,//F159  月冻结分相反向有功电能示值 
	-1,//F160  月冻结分相反向无功电能示值 
	//组21   
	(5+1+5+5*GW_TARIFF_MAX),//F161  正向有功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F162 正向无功（组合无功1）电能示值（总、费率1～M） 
	(5+1+5+5*GW_TARIFF_MAX),//F163  反向有功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F164 反向无功（组合无功1）电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F165  一象限无功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F166  二象限无功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F167  三象限无功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F168  四象限无功电能示值（总、费率1～M） 
	//组22   
	(5+1+5+5*GW_TARIFF_MAX),//F169  正向有功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F170 正向无功（组合无功1）电能示值（总、费率1～M） 
	(5+1+5+5*GW_TARIFF_MAX),//F171  反向有功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F172 反向无功（组合无功1）电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F173  一象限无功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F174  二象限无功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F175  三象限无功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F176  四象限无功电能示值（总、费率1～M） 
	//组23   
	(5+1+5+5*GW_TARIFF_MAX),//F177  正向有功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F178  正向无功（组合无功1）电能示值（总、费率1～M）  
	(5+1+5+5*GW_TARIFF_MAX),//F179  反向有功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F180  反向无功（组合无功1）电能示值（总、费率1～M）  
	(5+1+4+4*GW_TARIFF_MAX),//F181  一象限无功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F182  二象限无功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F183  三象限无功电能示值（总、费率1～M） 
	(5+1+4+4*GW_TARIFF_MAX),//F184  四象限无功电能示值（总、费率1～M） 
	//组24   
	(5+1+7+7*GW_TARIFF_MAX),//F185  正向有功最大需量及发生时间（总、费率1～M）  
	(5+1+7+7*GW_TARIFF_MAX),//F186  正向无功最大需量及发生时间（总、费率1～M）  
	(5+1+7+7*GW_TARIFF_MAX),//F187  反向有功最大需量及发生时间（总、费率1～M）  
	(5+1+7+7*GW_TARIFF_MAX),//F188  反向无功最大需量及发生时间（总、费率1～M）  
	(5+1+7+7*GW_TARIFF_MAX),//F189  正向有功最大需量及发生时间（总、费率1～M）  
	(5+1+7+7*GW_TARIFF_MAX),//F190  正向无功最大需量及发生时间（总、费率1～M）  
	(5+1+7+7*GW_TARIFF_MAX),//F191  反向有功最大需量及发生时间（总、费率1～M）  
	(5+1+7+7*GW_TARIFF_MAX),//F192  反向无功最大需量及发生时间（总、费率1～M）  
	//组25   
	(5+1+7+7*GW_TARIFF_MAX),//F193  正向有功最大需量及发生时间（总、费率1～M）  
	(5+1+7+7*GW_TARIFF_MAX),//F194  正向无功最大需量及发生时间（总、费率1～M）  
	(5+1+7+7*GW_TARIFF_MAX),//F195  反向有功最大需量及发生时间（总、费率1～M）  
	(5+1+7+7*GW_TARIFF_MAX),//F196  反向无功最大需量及发生时间（总、费率1～M）  
	 0,0,0,0,//F197～F200  备用 
	//组26  1～8时区正向有功电能示值 
	(5+1+5+5*GW_TARIFF_MAX),//F201 第一时区冻结正向有功电能示值（总、费率1～M） 
	(5+1+5+5*GW_TARIFF_MAX),//F202 第二时区冻结正向有功电能示值（总、费率1～M） 
	(5+1+5+5*GW_TARIFF_MAX),//F203 第三时区冻结正向有功电能示值（总、费率1～M） 
	(5+1+5+5*GW_TARIFF_MAX),//F204 第四时区冻结正向有功电能示值（总、费率1～M） 
	(5+1+5+5*GW_TARIFF_MAX),//F205 第五时区冻结正向有功电能示值（总、费率1～M） 
	(5+1+5+5*GW_TARIFF_MAX),//F206 第六时区冻结正向有功电能示值（总、费率1～M） 
	(5+1+5+5*GW_TARIFF_MAX),//F207 第七时区冻结正向有功电能示值（总、费率1～M） 
	(5+1+5+5*GW_TARIFF_MAX),//F208 第八时区冻结正向有功电能示值（总、费率1～M） 
	//组27   
	-1,//F209  电能表远程控制通断电状态及记录 
	 0,0,0,//F210～F212  备用 
	-1,//F213  电能表开关操作次数及时间 
	-1,//F214  电能表参数修改次数及时间 
	-1,//F215  电能表购、用电信息 
	(5+1+5+5*GW_TARIFF_MAX+5+5*GW_TARIFF_MAX),//F216  电能表结算信息 
	//组28   
	-1,//F217  台区集中抄表载波主节点白噪声曲线 
	-1,//F218  台区集中抄表载波主节点色噪声曲线 
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0//F219～F248  备用 
};


/** @brief 从缓冲中获取文件头的数值
 *  @param[in] size buffer size
 *  @param[in] pBuffer param buffer
 *  @return byte size
 */
int S_DFHeader::GetFromBuffer(const INT8U *pBuffer, int size)
{
	if (pBuffer != NULL && size >= DF_HEADER_SIZE)
	{
		memcpy(&m_Signature,pBuffer+DF_SIGNATURE_OFF, sizeof(m_Signature));
		memcpy(&m_MaxMP,    pBuffer+DF_MAXMP_OFF,    sizeof(m_MaxMP));
		memcpy(&m_PiecePerMeter, pBuffer+DF_PIECEPM_OFF, sizeof(m_PiecePerMeter));
		memcpy(&m_RecordLen,pBuffer+DF_RECORDLEN_OFF, sizeof(m_RecordLen));
		m_RecordStdT = C_TIME::BcdStr2Time((char*)pBuffer+DF_RECORDSTD_OFF);
		memcpy(&m_RecordInterVal,pBuffer+DF_RINTERVAL_OFF, sizeof(m_RecordInterVal));
		memcpy(&m_RecordInterUnt,pBuffer+DF_RINTERUNT_OFF, sizeof(m_RecordInterUnt));
		memcpy(&m_CheckSum,pBuffer+DF_CHECKSUM_OFF, sizeof(m_CheckSum));
	}
	return DF_HEADER_SIZE;
}

/** @brief 输出本结构体的值按文件头结构形式到缓冲中
 *  @param[in] size buffer size
 *  @param[out] pBuffer out buffer
 *  @return byte size
 */
int S_DFHeader::OutputBuffer(INT8U *pBuffer, int size)
{
	char *pBuf = (char *)pBuffer;
	if (pBuffer != NULL && size >= DF_HEADER_SIZE)
	{
		memset(pBuffer, 0x00, DF_HEADER_SIZE);
		memcpy(pBuf+DF_SIGNATURE_OFF, &m_Signature, sizeof(m_Signature));
		memcpy(pBuf+DF_MAXMP_OFF,     &m_MaxMP, sizeof(m_MaxMP));
		memcpy(pBuf+DF_PIECEPM_OFF,   &m_PiecePerMeter, sizeof(m_PiecePerMeter));
		memcpy(pBuf+DF_RECORDLEN_OFF, &m_RecordLen, sizeof(m_RecordLen));
		C_TIME::Time2BCDStr(m_RecordStdT, pBuf+DF_RECORDSTD_OFF);
		memcpy(pBuf+DF_RINTERVAL_OFF, &m_RecordInterVal,sizeof(m_RecordInterVal));
		memcpy(pBuf+DF_RINTERUNT_OFF, &m_RecordInterUnt,sizeof(m_RecordInterUnt));
		m_CheckSum = CalculateCheckSum((INT8U*)pBuf, DF_HEADER_SIZE-DF_CHECKSUM_SIZE);
		memcpy(pBuf+DF_CHECKSUM_OFF,  &m_CheckSum,sizeof(m_CheckSum));
	}
	return DF_HEADER_SIZE;
}


/** @brief 计算check sum
 *  @param[in] size buffer size
 *  @param[in] pBuffer param buffer
 *  @return check sum
 */
int S_DFHeader::CalculateCheckSum(const INT8U *pBuffer, int size)
{
	if (pBuffer == NULL)
		return -1;

	int ChkSum = 0;
	for (int i = 0; i < size; i++)
		ChkSum += *(pBuffer + i);
	return ChkSum;
}

/** @brief 从缓冲中获取数据 */
bool S_EAreaInfo::GetFromBuffer(const char *pbuf)
{
	memcpy(Sign, pbuf+DF_EAREA_SIGNATURE_OFF, sizeof(Sign));
	memcpy(MAddr, pbuf+DF_EAREA_MADDR_OFF, sizeof(MAddr));
	memcpy(&QTail, pbuf+DF_EAREA_QTAIL_OFF, sizeof(QTail));
	memcpy(&QHead, pbuf+DF_EAREA_QHEAD_OFF, sizeof(QHead));
	return true;
}

/** @brief 输出数据到缓冲 */
bool S_EAreaInfo::OutputBuffer(char *pbuf)
{
	memcpy(pbuf+DF_EAREA_SIGNATURE_OFF, Sign, sizeof(Sign));
	memcpy(pbuf+DF_EAREA_MADDR_OFF, MAddr, sizeof(MAddr));
	memcpy(pbuf+DF_EAREA_QTAIL_OFF, &QTail, sizeof(QTail));
	memcpy(pbuf+DF_EAREA_QHEAD_OFF, &QHead, sizeof(QHead));
	return true;
}


//
//数据文件IO
//
/** @brief 获取FN要求的记录长度
 *  @param[in] FN 国网要求FN
 *  @return 记录长度
*/
INT16U C_DataFileRW::GetRecordLen(INT16U FN)
{
	int type = C_DFRWFACT::GetType4Class2FN(FN);
	switch (type)
	{
	case C_DFRWFACT::CLASS2_VIP:
		return (m_EnergyDataLen*m_VipPointPerDay);
	case C_DFRWFACT::CLASS2_DAY:
	case C_DFRWFACT::CLASS2_MON:
	case C_DFRWFACT::CLASS2_GTH:
		return m_EnergyDataLen;
	default:
		assert(0);
	}
	return 0;
}

/** @brief 初始化能量数据区缓冲
 *  @param[in] pbuff 能量数据区缓冲
 *  @param[in] FN 国网要求FN
 *  @return 0 成功 -1 失败
 */
int  C_DataFileRW::InitEnergyArea(INT8U *pbuff, INT16U size, INT16U MP)
{
	if (pbuff == NULL || size < DF_EAREA_INFO_SIZE)
		return -1;
	
	memset(pbuff, 0x00, DF_EAREA_INFO_SIZE);
	memset(pbuff+DF_EAREA_INFO_SIZE, 0xEE, size-DF_EAREA_INFO_SIZE);

	if (MP != 0 && MP < GW_MP_MAX)
		InitEAreaSignature(pbuff, MP);

	return 0;
}

/** @brief 读取文件信息 */
int  C_DataFileRW::GetFileInfoHeader(FILE *fp)
{
	INT8U InfoBuf[DF_HEADER_SIZE];
	
	fseek(fp, 0x00, SEEK_SET);
	if (fread(InfoBuf, sizeof(InfoBuf), 1, fp) != 1)
		return -1;

	m_Header.GetFromBuffer(InfoBuf, sizeof(InfoBuf));
	INT16U chksum = m_Header.CalculateCheckSum(InfoBuf, sizeof(InfoBuf)-DF_CHECKSUM_SIZE);

	if (chksum != m_Header.m_CheckSum)
	{
		loget<<"Data file header is bad!"<<endl;
		return -1;
	}
	return 0;
}

/** @brief reset 数据文件
 *  @param[in] fp 文件指针
 *  @return true 成功;false 失败
 */
bool C_DataFileRW::ResetDataFile(FILE *fp)
{
	char *pEnergyArea, FileInfo[DF_HEADER_SIZE];
	int AreaSize = GetEnergyAreaSize();
	m_Header.OutputBuffer((INT8U*)FileInfo, sizeof(FileInfo));//prepare FileInfo Buffer
	pEnergyArea = new char[AreaSize];
	if (pEnergyArea == NULL)
		return false;

	InitEnergyArea((INT8U*)pEnergyArea, AreaSize, 0); //prepare EnergyArea Buffer

	fseek(fp, 0x00, SEEK_SET);
	fwrite(FileInfo, sizeof(FileInfo), 1, fp);//write file info
	for (INT16U i = 0; i < m_Header.m_MaxMP; ++i)//write energy area one by one 
	{
		InitEAreaSignature((INT8U*)pEnergyArea, i+1);//measure point is base on 1
		fwrite(pEnergyArea, AreaSize, 1, fp);
	}

	delete pEnergyArea;

	return true;
}

/** @brief 获取能量数据容量
 *  @param 无
 *  @return 能量数据区容量（单位：byte）
*/
int C_DataFileRW::GetEnergyAreaSize(void)
{
	return (DF_EAREA_INFO_SIZE //Energy area info header
		+(DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+GetTimeTagLength()+m_Header.m_RecordLen) * m_Header.m_PiecePerMeter);
}

/** @brief 查找符合时标的记录
 *  @param[in] EAI 能量数据区信息
 *  @param[in] pRecord 记录区开始指针
 *  @param[in] TimeTag 时标，格式20090808(4ByteBCD)
 *  @return -1 未发现，其它成功
 *  @note 月冻结不可用，日，重点户，抄表日数据可用
 */
int C_DataFileRW::FindRecord(const S_EAreaInfo &EAI, const char *pRecord, const char *TimeTag)
{
	int head = (int)EAI.QHead, tail = (int)EAI.QTail;

	if (head == tail) //empty queue
		return -1;

	while (head != tail)
	{
		if (head == 0)//从头上开始找效率高
			head = m_Header.m_PiecePerMeter - 1;
		else
			head -= 1;
		//定位记录位置
		const char *ptm = pRecord + head*(DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+GetTimeTagLength()+m_Header.m_RecordLen);
		ptm += DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE; //偏移到时标位置处
		if (memcmp(ptm, TimeTag, GetTimeTagLength()) == 0)//compare successful
			return head;
	}
	return -1;
}

/** @brief 制作时标
 *  @param[out] TimeTag 时标输出，最少4字节[20090808]BCD
 *  @param[in] time 时间
 *  @return 指向TimeTag的指针
 *  @note 月冻结不可用，日，重点户，抄表日数据可用
 */
char *C_DataFileRW::MakeTimeTag(char *TimeTag, time_t time)
{
	struct tm *curtm = localtime(&time);

	TimeTag[0] = cbcd((curtm->tm_year+1900)/100);
	TimeTag[1] = cbcd((curtm->tm_year+1900)%100);
	TimeTag[2] = cbcd(curtm->tm_mon+1);;
	TimeTag[3] = cbcd(curtm->tm_mday);

	return TimeTag;
}


/** @brief 增表后加大文件
 *  @param[in] fp 文件指针
 *  @param[in] MaxMP 最大测量点号
 *  @return true 成功;false 失败
 */
bool C_DataFileRW::AppendDataFile(FILE *fp, INT16U MaxMP)
{
	char *pEnergyArea, FileInfo[DF_HEADER_SIZE];
	int AreaSize = GetEnergyAreaSize();
	pEnergyArea = new char[AreaSize];
	if (pEnergyArea == NULL)
		return false;

	InitEnergyArea((INT8U*)pEnergyArea, AreaSize, 0); //prepare EnergyArea Buffer
	fseek(fp, 0x00, SEEK_END);
	for (INT16U i = m_Header.m_MaxMP; i < MaxMP; ++i)//write energy area one by one 
	{
		InitEAreaSignature((INT8U*)pEnergyArea, i+1);//measure point is base on 1
		fwrite(pEnergyArea, AreaSize, 1, fp);
	}
	delete pEnergyArea;

	if (m_Header.m_MaxMP < MaxMP)
		m_Header.m_MaxMP = MaxMP;
	m_Header.OutputBuffer((INT8U*)FileInfo, sizeof(FileInfo));
	fseek(fp, 0x00, SEEK_SET);
	fwrite(FileInfo, sizeof(FileInfo), 1, fp);//write file info

	return true;
}

/** @brief 写入一条数据记录
 *  @param[in] fp 文件指针
 *  @param[in] EnergyData 能量数据，结构参见S_EnergyData
 *  @param[in] DataTime 时标
 *  @return 0 成功；非0 失败
 */
int  C_DataFileRW::Write(FILE *fp, const S_EnergyData &EnergyData, time_t DataTime)
{
	int err = -1;
	if (EnergyData.m_MeterMP == 0)
		return -1;

	int AreaSize = GetEnergyAreaSize();
	char *pEnergyArea = new char[AreaSize];
	if (pEnergyArea == NULL)
		return -1;

	char EAreaSign[DF_EAREA_SIGNATURE_SIZE];
	InitEAreaSignature((INT8U *)EAreaSign, EnergyData.m_MeterMP);

	long CurEAreaOff = DF_HEADER_SIZE + (AreaSize * (EnergyData.m_MeterMP-1));
	fseek(fp, CurEAreaOff, SEEK_SET);
	fread(pEnergyArea, AreaSize, 1, fp);

	if (memcmp(pEnergyArea, EAreaSign, sizeof(EAreaSign)) != 0)//check Energy Area sign
	{//check failed,maybe offset is error
		loget<<"EnergyArea sign is error: "<<pEnergyArea<<" != "<<EAreaSign<<endl;
	}
	else if ((err = CombineRecord(pEnergyArea, EnergyData, DataTime)) == 0)
	{
		fseek(fp, CurEAreaOff, SEEK_SET);
		fwrite(pEnergyArea, AreaSize, 1, fp);
	}

	delete pEnergyArea;

	return err;
}

/** @brief 读取一条数据记录 */
int  C_DataFileRW::Read(FILE *fp, S_EnergyData &EnergyData, time_t DataTime)
{
	int err = -1;
	if (EnergyData.m_MeterMP == 0)
		return -1;

	int AreaSize = GetEnergyAreaSize();
	char *pEnergyArea = new char[AreaSize];
	if (pEnergyArea == NULL)
		return -1;

	char EAreaSign[DF_EAREA_SIGNATURE_SIZE];
	InitEAreaSignature((INT8U *)EAreaSign, EnergyData.m_MeterMP);

	long CurEAreaOff = DF_HEADER_SIZE + (AreaSize * (EnergyData.m_MeterMP-1));
	fseek(fp, CurEAreaOff, SEEK_SET);
	fread(pEnergyArea, AreaSize, 1, fp);

	if (memcmp(pEnergyArea, EAreaSign, sizeof(EAreaSign)) != 0)//check Energy Area sign
	{//check failed,maybe offset is error
		loget<<"EnergyArea sign is error: "<<pEnergyArea<<" != "<<EAreaSign<<endl;
	}
	else
	{
		err = ExtractRecord(pEnergyArea, EnergyData, DataTime);
	}

	delete pEnergyArea;

	return err;
}

/** @brief 合并数据到记录
 *  @param[inout] pEnergyArea 历史数据记录缓冲
 *  @param[in] EnergyData 当前写放的数据
 *  @param[in] DataTime 时标
 *  @return 0 成功; 非0 失败
 */
int C_DataFileRW::CombineRecord(char *pEnergyArea, const S_EnergyData &EnergyData, time_t DataTime)
{
	int pos;
	char TimeTag[DF_DAY_TIME_SIZE], *pRecordBeg;
	S_EAreaInfo EAI;

	EAI.GetFromBuffer(pEnergyArea);
	MakeTimeTag(TimeTag, DataTime);

	if ((pos = FindRecord(EAI, pEnergyArea+DF_EAREA_INFO_SIZE, TimeTag)) != -1)//combine data
	{
		pRecordBeg  = pEnergyArea+DF_EAREA_INFO_SIZE+pos*(DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+GetTimeTagLength()+m_Header.m_RecordLen);
		*pRecordBeg = DF_RECORD_VALID_FLAG;
		*(pRecordBeg+DF_RECORD_FLAG_SIZE) = EnergyData.m_DataFlag;
		pRecordBeg += DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+GetTimeTagLength();//local data begin
		for (INT16U i = 0; i < EnergyData.m_Data.size(); i++)
		{
			if (EnergyData.m_Data[i] != 0xEE)
				*(pRecordBeg+i) = EnergyData.m_Data[i];
		}
	}
	else//first write
	{
		pRecordBeg  = pEnergyArea+DF_EAREA_INFO_SIZE+EAI.QHead*(DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+GetTimeTagLength()+m_Header.m_RecordLen);
		*pRecordBeg = DF_RECORD_VALID_FLAG;
		*(pRecordBeg+DF_RECORD_FLAG_SIZE) = EnergyData.m_DataFlag;
		memcpy(pRecordBeg+DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE, TimeTag, GetTimeTagLength());
		memcpy(pRecordBeg+DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+GetTimeTagLength(), EnergyData.m_Data.c_str(), EnergyData.m_Data.size());

		EAI.QHead += 1;
		if (EAI.QHead == m_Header.m_PiecePerMeter) EAI.QHead = 0;
		if (EAI.QTail == EAI.QHead) EAI.QTail += 1;
		if (EAI.QTail == m_Header.m_PiecePerMeter) EAI.QTail = 0;
		EAI.OutputBuffer(pEnergyArea);
	}

	return 0;
}

/** @brief 从数据区提取记录 
 *  @param[in] pEnergyArea 数据区开始指针
 *  @param[out] EnergyData 数据输出缓冲
 *  @param[in] DataTime 时标
 */
int C_DataFileRW::ExtractRecord(char *pEnergyArea, S_EnergyData &EnergyData, time_t DataTime)
{
	int pos;
	char TimeTag[DF_DAY_TIME_SIZE];
	S_EAreaInfo EAI;

	EAI.GetFromBuffer(pEnergyArea);
	MakeTimeTag(TimeTag, DataTime);

	EnergyData.m_DataFlag = 0xFF;
	if ((pos = FindRecord(EAI, pEnergyArea+DF_EAREA_INFO_SIZE, TimeTag)) != -1)//找到数据记录
	{
		char *pRecordBeg = pEnergyArea+DF_EAREA_INFO_SIZE+pos*(DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+GetTimeTagLength()+m_Header.m_RecordLen);
		if (*pRecordBeg == DF_RECORD_VALID_FLAG)
		{
			EnergyData.m_DataFlag = *(pRecordBeg+DF_RECORD_FLAG_SIZE);
			EnergyData.m_Data.append(pRecordBeg+DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+GetTimeTagLength(), m_Header.m_RecordLen);
		}
		else
		{//return nothing
			logwt<<"Data Begin Flag is invalid!"<<endl;
//			EnergyData.m_Data.append((size_t)m_Header.m_RecordLen, 0xEE);
		}
	}
	else
	{//return nothing
		logwt<<"DataTime is invalid!"<<endl;
//		EnergyData.m_Data.append((size_t)m_Header.m_RecordLen, 0xEE);
	}

	return 0;
}


//
//日数据读写类
//
/** @brief 数据文件初始化
 *  @param[in] fp 文件指针
 *  @param[in] FN 国网要求FN
 *  @param[in] MaxMP 最大测量点号
 *  @return true 成功；false 失败
 */
bool C_DataFileRW_D::InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP)
{
	memset(&m_Header, 0x00, sizeof(m_Header));
	InitDataFileSignature(m_Header.m_Signature, FN, "day");
	m_Header.m_MaxMP = MaxMP;
	m_Header.m_PiecePerMeter = RECORD_STORE_DAY_MAX+1;//Queue
	m_Header.m_RecordLen = GetRecordLen(FN);
	m_Header.m_RecordInterVal = 1;
	m_Header.m_RecordInterUnt = RECORD_INTER_UNIT_DAY;
	m_Header.m_RecordStdT = C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, time(NULL));
//	m_Header.m_CheckSum = 0;

	return ResetDataFile(fp);
}

/** @brief 将时间按数据类型截齐
 *  @param[in] tm 时标
 *  @return 截齐后的时标
 */
time_t C_DataFileRW_D::DataTimeTrim(time_t tm)
{
	return C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, tm);
}

//
//月数据读写类
//
/** @brief 数据文件初始化
 *  @param[in] fp 文件指针
 *  @param[in] FN 国网要求FN
 *  @param[in] MaxMP 最大测量点号
 *  @return true 成功；false 失败
 */
bool C_DataFileRW_M::InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP)
{
	memset(&m_Header, 0x00, sizeof(m_Header));
	InitDataFileSignature(m_Header.m_Signature, FN, "mon");
	m_Header.m_MaxMP = MaxMP;
	m_Header.m_PiecePerMeter = RECORD_STORE_MON_MAX+1;
	m_Header.m_RecordLen = GetRecordLen(FN);
	m_Header.m_RecordInterVal = 1;
	m_Header.m_RecordInterUnt = RECORD_INTER_UNIT_MON;
	m_Header.m_RecordStdT = C_TIME::DateTrim(C_TIME::TIME_UNIT_MONTH, time(NULL));
//	m_Header.m_CheckSum = 0;

	return ResetDataFile(fp);
}

/** @brief 将时间按数据类型截齐
 *  @param[in] tm 时标
 *  @return 截齐后的时标
 */
time_t C_DataFileRW_M::DataTimeTrim(time_t tm)
{
	return C_TIME::DateTrim(C_TIME::TIME_UNIT_MONTH, tm);
}

/** @brief 制作时标
 *  @param[out] TimeTag 时标输出，最少4字节[20090808]BCD
 *  @param[in] time 时间
 *  @return 指向TimeTag的指针
 */
char *C_DataFileRW_M::MakeTimeTag(char *TimeTag, time_t time)
{
	struct tm *curtm = localtime(&time);

	TimeTag[0] = cbcd((curtm->tm_year+1900)/100);
	TimeTag[1] = cbcd((curtm->tm_year+1900)%100);
	TimeTag[2] = cbcd(curtm->tm_mon+1);;

	return TimeTag;
}

/** @brief 获取记录时标长度 */
int C_DataFileRW_M::GetTimeTagLength(void)
{
	return DF_MON_TIME_SIZE;
}

//
//重点户数据读写类
//
/** @brief 数据文件初始化
 *  @param[in] fp 文件指针
 *  @param[in] FN 国网要求FN
 *  @param[in] MaxMP 最大测量点号
 *  @return true 成功；false 失败
 */
bool C_DataFileRW_V::InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP)
{
	assert(m_VipPointPerDay != 0);
	memset(&m_Header, 0x00, sizeof(m_Header));
	InitDataFileSignature(m_Header.m_Signature, FN, "vip");
	m_Header.m_MaxMP = MaxMP;
	m_Header.m_PiecePerMeter = RECORD_STORE_VIP_MAX+1;
	m_Header.m_RecordLen = GetRecordLen(FN);
	m_Header.m_RecordInterVal = (24*60)/m_VipPointPerDay;//VIPDATA_POINT_PERDAY;//一天分钟除以一天点数
	m_Header.m_RecordInterUnt = RECORD_INTER_UNIT_MIN;
	m_Header.m_RecordStdT = C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, time(NULL));
//	m_Header.m_CheckSum = 0;

	return ResetDataFile(fp);
}

/** @brief 将时间按数据类型截齐
 *  @param[in] tm 时标
 *  @return 截齐后的时标
 */
time_t C_DataFileRW_V::DataTimeTrim(time_t tm)
{
	return tm;
}

/** @brief 合并数据到记录
 *  @param[inout] pEnergyArea 历史数据记录缓冲
 *  @param[in] EnergyData 当前写放的数据
 *  @param[in] DataTime 时标
 *  @return 0 成功; 非0 失败
 */
int C_DataFileRW_V::CombineRecord(char *pEnergyArea, const S_EnergyData &EnergyData, time_t DataTime)
{
	int pos;
	char TimeTag[DF_DAY_TIME_SIZE], *pRecordBeg = NULL;
	S_EAreaInfo EAI;

	EAI.GetFromBuffer(pEnergyArea);
	MakeTimeTag(TimeTag, DataTime);

	if ((pos = FindRecord(EAI, pEnergyArea+DF_EAREA_INFO_SIZE, TimeTag)) != -1)//combine data
	{
		pRecordBeg  = pEnergyArea+DF_EAREA_INFO_SIZE+pos*(DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+GetTimeTagLength()+m_Header.m_RecordLen);
		*pRecordBeg = DF_RECORD_VALID_FLAG;
		*(pRecordBeg+DF_RECORD_FLAG_SIZE) = EnergyData.m_DataFlag;
		struct tm *curtm = localtime(&DataTime);
		int VipPoint = ((curtm->tm_hour*60) + curtm->tm_min)/m_Header.m_RecordInterVal;
		memcpy(pRecordBeg+DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+(VipPoint*EnergyData.m_Data.size()), EnergyData.m_Data.c_str(), EnergyData.m_Data.size());
	}
	else//first write
	{
		memset(pRecordBeg, 0xEE, DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+m_Header.m_RecordLen);//清除历史记录
		*pRecordBeg = DF_RECORD_VALID_FLAG;
		*(pRecordBeg+DF_RECORD_FLAG_SIZE) = EnergyData.m_DataFlag;
		struct tm *curtm = localtime(&DataTime);
		int VipPoint = ((curtm->tm_hour*60) + curtm->tm_min)/m_Header.m_RecordInterVal;
		memcpy(pRecordBeg+DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+(VipPoint*EnergyData.m_Data.size()), EnergyData.m_Data.c_str(), EnergyData.m_Data.size());

		EAI.QHead += 1;
		if (EAI.QHead == m_Header.m_PiecePerMeter) EAI.QHead = 0;
		if (EAI.QTail == EAI.QHead) EAI.QTail += 1;
		if (EAI.QTail == m_Header.m_PiecePerMeter) EAI.QTail = 0;
		EAI.OutputBuffer(pEnergyArea);
	}

	return 0;
}


//
//抄表日数据读写类
//
/** @brief 数据文件初始化
 *  @param[in] fp 文件指针
 *  @param[in] FN 国网要求FN
 *  @param[in] MaxMP 最大测量点号
 *  @return true 成功；false 失败
 */
bool C_DataFileRW_G::InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP)
{
	memset(&m_Header, 0x00, sizeof(m_Header));
	InitDataFileSignature(m_Header.m_Signature, FN, "gth");
	m_Header.m_MaxMP = MaxMP;
	m_Header.m_PiecePerMeter = RECORD_STORE_CBR_MAX+1;//用队例实现有一个冗余
	m_Header.m_RecordLen = GetRecordLen(FN);
	m_Header.m_RecordInterVal = 0xFF;
	m_Header.m_RecordInterUnt = RECORD_INTER_UNIT_DAY;
	m_Header.m_RecordStdT = C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, time(NULL));
//	m_Header.m_CheckSum = 0;

	return ResetDataFile(fp);
}

/** @brief 将时间按数据类型截齐
 *  @param[in] tm 时标
 *  @return 截齐后的时标
 */
time_t C_DataFileRW_G::DataTimeTrim(time_t tm)
{
	return C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, tm);
}

//
//数据类文件读写实例工厂
//
/** @brief 获取一个数据读写实例
 *  @param[in] FN 国网要求二类数据FN
 *  @return NULL 失败; 非NULL 成功
 */
C_DataFileRW *C_DFRWFACT::GetDataFileRW(INT16U FN)
{
	int type = GetType4Class2FN(FN);
	switch (type)
	{
	case CLASS2_VIP:
		return new C_DataFileRW_V;
	case CLASS2_DAY:
		return new C_DataFileRW_D;
	case CLASS2_GTH:
		return new C_DataFileRW_G;
	case CLASS2_MON:
		return new C_DataFileRW_M;
	default:
		return NULL;
	}
	return NULL;
}

/** @brief 释放一个数据读写实例
 *  @param[in] pDFRW 数据读写实例指针
 *  @return 无
 */
void C_DFRWFACT::FreeDataFileRW(C_DataFileRW *pDFRW)
{
	delete pDFRW;
}

/** @brief 获取二类数据FN类型 
 *  @param[in] FN 国网要求二类数据FN
 *  @return CLASS2_UNKNOW, CLASS2_VIP, CLASS2_DAY, CLASS2_MON, CLASS2_GTH
 */
int  C_DFRWFACT::GetType4Class2FN(INT16U FN)
{
	if ((FN>=73&&FN<=110) || (FN>=138&&FN<=148) || (FN>=153&&FN<=156) || (FN==217) || (FN==218))//重点户
		return CLASS2_VIP;
	else if ((FN>=1&&FN<=8) || (FN>=25&&FN<=32) || (FN>=41&&FN<=43) || (FN==45) || (FN==49) || (FN==50)//日
		|| (FN==53) || (FN>=57&&FN<=59) || (FN>=113&&FN<=129) || (FN>=161&&FN<=168) || (FN>=185&&FN<=188)
		|| (FN==209))//日
		return CLASS2_DAY;
	else if ((FN>=9&&FN<=12) || (FN>=169&&FN<=176) || (FN>=189&&FN<=192))//抄表日
		return CLASS2_GTH;
	else if ((FN>=17&&FN<=24) || (FN>=33&&FN<=39) || (FN==44) || (FN==46) || (FN==51) || (FN==52) || (FN==54)//月
		|| (FN>=60&&FN<=66) || (FN==130) || (FN>=157&&FN<=160) || (FN>=177&&FN<=184) || (FN>=193&&FN<=208)
		|| (FN>=213&&FN<=216))//月
		return CLASS2_MON;

	return CLASS2_UNKNOW;
}

/** @brief  获取FN对应的PN意义
 *  @param[in] FN 国网要求二类数据FN
 *  @return CLASS2PN_UNKNOW, CLASS2PN_MPOINT, CLASS2PN_TGROUP, CLASS2PN_PORT, CLASS2PN_TERM
 */
int C_DFRWFACT::GetPnMean4Class2Fn(INT16U FN)
{
	if ((FN >= 1 && FN <= 46) ||(FN >= 81 && FN <= 123) || (FN >= 145 && FN <= 216))
		return CLASS2PN_MPOINT;
	else if ((FN >= 49 && FN <= 54) || (FN >= 217 && FN <= 218))
		return CLASS2PN_TERM;
	else if (FN >= 57 && FN <= 76)
		return CLASS2PN_TGROUP;
	else if (FN >= 129 && FN <= 138)
		return CLASS2PN_PORT;

	return CLASS2PN_UNKNOW;
}


//
//数据文件具体操作定义	
//
/** @brief C_DATAFILE构造函数
 *  @param[in] fPath 文件名和路径
 *  @param[in] io OPEN_READ or OPEN_WRITE
 *  @param[in] FN 国网要求FN
 */
C_DATAFILE::C_DATAFILE(const char *fPath, INT8U io, INT16U FN)
{
	m_FN = FN;
	m_fp = NULL;
	m_pdfrw = C_DFRWFACT::GetDataFileRW(FN);
	if (m_pdfrw)
        Open(fPath, io);
}

/** @brief C_DATAFILE析构函数
 */
C_DATAFILE::~C_DATAFILE()
{
	Close();
	if (m_pdfrw != NULL)
		C_DFRWFACT::FreeDataFileRW(m_pdfrw);
	m_pdfrw = NULL;
}

/** @brief 打开数据文件
 *  @param[in] fPath 文件名和路径
 *  @param[in] io OPEN_READ or OPEN_WRITE
 *  @return true if the file is successfully opened
 *  @return false if failed
 */
bool C_DATAFILE::Open(const char *pfile, INT8U io)
{
	if (io == OPEN_READ)//读数据
	{
		m_fp = C_FILE::OpenFile(pfile,"rb", false);
	}
	else if (io == OPEN_WRITE)//写数据
	{
		m_fp = C_FILE::OpenFile(pfile,"r+b", true);
		if (m_fp == NULL)
			m_fp = C_FILE::OpenFile(pfile,"w+b", true);
	}
	else//错误
	{
		m_fp = NULL;
	}

	return (m_fp != NULL);
}

/** @brief 关闭数据文件
 *  @param[in] fPath 文件名和路径
 *  @param[in] io OPEN_READ or OPEN_WRITE
 *  @return true if the file is successfully closed
 *  @return false if failed
 */
bool C_DATAFILE::Close(void)
{
	int err = 0;
	if (m_fp != NULL)
		err = C_FILE::CloseFile(m_fp);
	m_fp = NULL;

	return (err == 0);
}

/** @brief 读取一条记录
 *  @param[inout] EnergyData 数据输出缓冲
 *  @param[in] DataTime 时标
 *  @return 0 成功；非0 失败
 */
int  C_DATAFILE::Read(S_EnergyData &EnergyData, time_t DataTime)
{
	if (IsInvalidMP(EnergyData.m_MeterMP))//invalid MP
	{
		loget<<EnergyData.m_MeterMP<<" is invalid!!!"<<endl;
		return -1;
	}
	if (EnergyData.m_MeterMP > m_pdfrw->GetMaxMP())//测量点在当前文件中吗
	{
		loget<<"DataFile MaxMP = "<<m_pdfrw->GetMaxMP()<<", Current MP = "<<EnergyData.m_MeterMP<<endl;
		return -1;
	}

	//读取
	return m_pdfrw->Read(m_fp, EnergyData, DataTime);
}

/** @brief 读取重点户一条记录
 *  @param[inout] EnergyData 数据输出缓冲
 *  @param[in] VipTime 重点户时标
 *  @return 0 成功；非0 失败
 */
int  C_DATAFILE::ReadVip(S_EnergyData &EnergyData, const S_VIPT &VipTime)
{
	if (IsInvalidMP(EnergyData.m_MeterMP))//invalid MP
	{
		loget<<EnergyData.m_MeterMP<<" is invalid!!!"<<endl;
		return -1;
	}
	if (EnergyData.m_MeterMP > m_pdfrw->GetMaxMP())//测量点在当前文件中吗
	{
		loget<<"DataFile MaxMP = "<<m_pdfrw->GetMaxMP()<<", Current MP = "<<EnergyData.m_MeterMP<<endl;
		return -1;
	}
	if (VipTime.m_DataDensity == 0 || VipTime.m_DataDots == 0 || GetVipFreezeIntervalM(VipTime.m_DataDensity) == 0)
	{
		return -1;
	}

	//读取
	int days = 0, size = 0;
	S_EnergyData Record;
	Record.m_MeterMP = EnergyData.m_MeterMP;
	time_t MaxTime = VipTime.m_time + (GetVipFreezeIntervalM(VipTime.m_DataDensity) * 60) * VipTime.m_DataDots;
	for (time_t MinTime = C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, VipTime.m_time); MinTime < MaxTime; MinTime += 86400)
	{
		days += 1;
		size  = (int)Record.m_Data.size();
		if (m_pdfrw->Read(m_fp, Record, MinTime) != 0)
			return -1;//读取失败
		if (size == (int)Record.m_Data.size())
			return -1;//没有读到数据
	}

	EnergyData.m_DataFlag = Record.m_DataFlag;
	//按时标提取数据
	INT16U DotLen = m_pdfrw->GetOnceWriteDataSize();
	int TotalActualDots = (int)Record.m_Data.size()/DotLen;//数据中实际存在的点数,按天算
	int TotalExpectedDots = days * ((24*60)/GetVipFreezeIntervalM(VipTime.m_DataDensity));//数据中期望的点数，按天算
	int BegDot = (int)((VipTime.m_time - C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, VipTime.m_time)) / (GetVipFreezeIntervalM(VipTime.m_DataDensity) * 60));
	for (int i = 0; i < VipTime.m_DataDots; i++)
	{
		int CurDot = (BegDot+i) * TotalActualDots / TotalExpectedDots;//计算当前第i点的位置
		EnergyData.m_Data.append(&Record.m_Data[CurDot*DotLen], DotLen);
	}

	return 0;
}

/** @brief 写入一条记录
 *  @param[in] EnergyData 待写入数据
 *  @param[in] DataTime 时标
 *  @return 0 成功写入；非0 失败
 */
int  C_DATAFILE::Write(const S_EnergyData &EnergyData, time_t DataTime)
{
	if (IsInvalidMP(EnergyData.m_MeterMP))//invalid MP
	{
		loget<<EnergyData.m_MeterMP<<" is invalid!!!"<<endl;
		return -1;
	}
	if (EnergyData.m_MeterMP > m_pdfrw->GetMaxMP())//测量点在当前文件中吗
	{//扩充文件
		if (m_pdfrw->AppendDataFile(m_fp, EnergyData.m_MeterMP) == false)
		{
			loget<<"AppendDataFile fail, Current MP = "<<EnergyData.m_MeterMP<<endl;
			return -1;
		}
	}

	if (EnergyData.m_Data.size() != m_pdfrw->GetOnceWriteDataSize())	//记录大小相等吗
	{
//		assert(0);
		loget<<"预期的数据大小与实际的不符:"<<(int)EnergyData.m_Data.size()<<" != "<<m_pdfrw->GetOnceWriteDataSize()<<endl;
		return -1;//不等返回
	}
	
	//写入
	return m_pdfrw->Write(m_fp, EnergyData, DataTime);
}

/** @brief 判断是否新建
 *  @param 无
 *  @return true 新建文件
 *  @return false 已有数据文件
 */
bool C_DATAFILE::IsNew(void)
{
	fseek(m_fp, 0x00, SEEK_END);
	return ((ftell(m_fp)<=DF_HEADER_SIZE)? true:false);
}

/** @brief 初始化数据文件
 *  @param[in] MaxMP 最大测量点号
 *  @return true 成功
 *  @return false 失败
 */
bool C_DATAFILE::InitDataFile(INT16U MaxMP)
{
	if (MaxMP == 0)
		return false;
	return m_pdfrw->InitDataFile(m_fp, m_FN, MaxMP);
}

/** @brief 获取文件信息头
 *  @param 无
 *  @return 0 成功
 *  @return 非0 失败
 */
int  C_DATAFILE::GetFileHeader(void)
{
	return m_pdfrw->GetFileInfoHeader(m_fp);
}

/** @brief 将时间按数据类型截齐 */
time_t C_DATAFILE::DataTimeTrim(time_t tm)
{
	return m_pdfrw->DataTimeTrim(tm);
}
/** @brief 获取重点户冻结时间间隔
 *  @param[in] FreezeCode 上行规约要求冻结密度
 *  @return 冻结时间，以分钟为单位
 *  @see 《上行规约》附录C（规范性附录）数据冻结密度
 */
int  C_DATAFILE::GetVipFreezeIntervalM(INT8U FreezeCode)
{
	switch (FreezeCode)
	{
	case 1: return 15;
	case 2: return 30;
	case 3: return 60;
	case 254: return 5;
	case 255: return 1;
	}
	return 0;
}

//
// 用户接口定义
//
/** @brief 二类数据写入函数
 *  @param[in] FN 国网要求的FN
 *  @param[in] MeterDatas 能量数据组，参见S_EnergyDatas定义
 *  @param[in] DataTime 时标
 *  @param[in] VipPointPerDay 重点户FN每天须要的数据点数据，仅过重点户FN有效
 *  @return 写入的记录条数
 */
int C_DataFileOP::WriteClass2Data(INT16U FN, const S_EnergyDatas &MeterDatas, time_t DataTime, int VipPointPerDay)
{
	if (FN >= (sizeof(Class2FnLen)/sizeof(Class2FnLen[0])))
		return 0;

	S_EnergyDatas tMeterDatas = MeterDatas;
	if (C_DFRWFACT::GetPnMean4Class2Fn(FN) == C_DFRWFACT::CLASS2PN_TERM)//case P0: +1 
	{
		for (INT16U i = 0; i < tMeterDatas.size(); ++i)
			tMeterDatas[i].m_MeterMP += 1;
	}

	int FnLen = Class2FnLen[FN];
	if (FnLen != 0 && FnLen != -1)
	{
		for (INT16U i = 0; i < tMeterDatas.size(); ++i)
		{
			if (tMeterDatas[i].m_Data.empty())//不处理空的情况
				;
			else if (tMeterDatas[i].m_Data.size() < (unsigned)FnLen)
				tMeterDatas[i].m_Data.append((size_t)(FnLen-tMeterDatas[i].m_Data.size()), (char)m_FillChar);
		}
	}

	return _WriteClass2Data(FN, tMeterDatas, DataTime, VipPointPerDay);
}


/** @brief 二类数据读取函数
 *  @param[in] FN 国网要求的FN
 *  @param[in] MeterDatas 能量数据组，参见S_EnergyDatas定义
 *  @param[in] DataTime 时标
 *  @return 读取的记录条数（重点户一条记录是指一天的数据）
 */
int C_DataFileOP::ReadClass2Data(INT16U FN, S_EnergyDatas &MeterDatas, time_t DataTime)
{
	int PnMean = C_DFRWFACT::GetPnMean4Class2Fn(FN);

	if (PnMean == C_DFRWFACT::CLASS2PN_TERM)//case P0: +1 
	{
		for (INT16U i = 0; i < MeterDatas.size(); ++i)
			MeterDatas[i].m_MeterMP += 1;
	}

	int cnt = _ReadClass2Data(FN, MeterDatas, DataTime);

	if (PnMean == C_DFRWFACT::CLASS2PN_TERM)//case P0: restore 
	{
		for (INT16U i = 0; i < MeterDatas.size(); ++i)
			MeterDatas[i].m_MeterMP -= 1;
	}

	/** 去除附加的冗余数据0xFF
	 */
	for (INT16U i = 0; i < MeterDatas.size(); ++i)
	{
		string::iterator iter = find(MeterDatas[i].m_Data.begin(), MeterDatas[i].m_Data.end(), m_FillChar);
		if (iter != MeterDatas[i].m_Data.end())
			MeterDatas[i].m_Data.erase(iter, MeterDatas[i].m_Data.end());
	}
	
	return cnt;
}

/** @brief 二类重点户数据读取函数
 *  @param[in] FN 国网要求的二类数据FN，调用者必须保证为重点户FN
 *  @param[inout] MeterDatas 能量数据组，参见S_EnergyDatas定义
 *  @param[in] VipTime 重点户时标
 *  @return 读取的记录条数
 */
int C_DataFileOP::ReadClass2DataVIP(INT16U FN, S_EnergyDatas &MeterDatas, const S_VIPT &VipTime)
{
	char fPath[260];

	MakeDataFilePath(fPath, FN);

	C_DATAFILE DataFile(fPath, C_DATAFILE::OPEN_READ, FN);
	if (!DataFile)
	{
		loget<<fPath<<" open failed!"<<endl;
		return 0;
	}

	if (DataFile.GetFileHeader() != 0)//Oh, shit
	{
		//文件操作失败或校验失败???
		loget<<fPath<<" FileHeader fail."<<endl;
		return 0;
	}

	size_t i = 0;
	for (; i < MeterDatas.size(); ++i)//逐条读取数据
	{
		if (DataFile.ReadVip(MeterDatas[i], VipTime) != 0)
			break;//read failed
	}

	return (int)i;
}

/** @brief 二类数据写入函数
 *  @param[in] FN 国网要求的FN
 *  @param[in] MeterDatas 能量数据组，参见S_EnergyDatas定义
 *  @param[in] DataTime 时标
 *  @param[in] VipPointPerDay 重点户FN每天须要的数据点数据，仅过重点户FN有效
 *  @return 写入的记录条数
 *  @note user can't call.
 */
int C_DataFileOP::_WriteClass2Data(INT16U FN, const S_EnergyDatas &MeterDatas, time_t DataTime, int VipPointPerDay)
{
	char fPath[260];

	MakeDataFilePath(fPath, FN);

	C_DATAFILE DataFile(fPath, C_DATAFILE::OPEN_WRITE, FN);
	if (!DataFile)
	{
		loget<<fPath<<" open failed!"<<endl;
		return 0;
	}
	if (MeterDatas.empty() || MeterDatas[0].m_Data.empty())
	{
		loget<<"Data collection is empty!!!"<<endl;
		return 0;
	}

	if (DataFile.IsNew() == true)//是否为新建文件
	{
		if (VipPointPerDay <= 0)
			VipPointPerDay = VIPDATA_POINT_PERDAY;
		DataFile.SetVipPoint(VipPointPerDay);//set data property
		DataFile.SetEnergyDataLen((INT16U)MeterDatas[0].m_Data.size());//set data property
		
		INT16U MaxMP = 0;
		for (size_t i = 0; i < MeterDatas.size(); ++i)// find max measure point
		{
			if (MeterDatas[i].m_MeterMP > MaxMP)
				MaxMP = MeterDatas[i].m_MeterMP;
		}

		if (DataFile.InitDataFile(MaxMP) == false)
		{
			loget<<"InitDataFile:"<<fPath<<" failed."<<endl;
			return 0;
		}
	}

	if (DataFile.GetFileHeader() != 0)//Oh, shit
	{
		//文件操作失败或校验失败???
		loget<<fPath<<" FileHeader fail."<<endl;
		return 0;
	}

	if (DataTime == 0)
		DataTime = time(NULL);
	DataTime = DataFile.DataTimeTrim(DataTime);
	
	size_t i = 0;
	for (; i < MeterDatas.size(); ++i)//逐条写入数据
	{
		if (DataFile.Write(MeterDatas[i], DataTime) != 0)
			break;//write failed
	}

	return (int)i;
}


/** @brief 二类数据读取函数
 *  @param[in] FN 国网要求的FN
 *  @param[in] MeterDatas 能量数据组，参见S_EnergyDatas定义
 *  @param[in] DataTime 时标
 *  @return 读取的记录条数（重点户一条记录是指一天的数据）
 *  @note user can't call.
 */
int C_DataFileOP::_ReadClass2Data(INT16U FN, S_EnergyDatas &MeterDatas, time_t DataTime)
{
	char fPath[260];

	MakeDataFilePath(fPath, FN);

	C_DATAFILE DataFile(fPath, C_DATAFILE::OPEN_READ, FN);
	if (!DataFile)
	{
		loget<<fPath<<" open failed!"<<endl;
		return 0;
	}

	if (DataFile.GetFileHeader() != 0)//Oh, shit
	{
		//文件操作失败或校验失败???
		loget<<fPath<<" FileHeader fail."<<endl;
		return 0;
	}

	if (DataTime == 0)
		DataTime = time(NULL);
	DataTime = DataFile.DataTimeTrim(DataTime);
	
	size_t i = 0;
	for (; i < MeterDatas.size(); ++i)//逐条读取数据
	{
		if (DataFile.Read(MeterDatas[i], DataTime) != 0)
			break;//read failed
	}

	return (int)i;
}

/** @brief 生成数据文件路径
 *  @param[out] fpath 文件路径输出缓冲，最小260字节
 *  @param[in] FN 国网要求二类数据FN
 *  @return fpath的指针
 *  @note user can't call.
 */
char *C_DataFileOP::MakeDataFilePath(char *fPath, INT16U FN)
{
	sprintf(fPath, "%s/c2f%d.dat", CFG_FILE_EDATA_PATH, FN);

	return fPath;
}

