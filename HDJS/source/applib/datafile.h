/**
 *  @file datafile.h 
 *  @brief 集中器能量数据文件存取操作集
 *  @author jinhailiao
 *  @date 2009-11-24
 *  @version v1.0
 *  @note
 *  1、同一个FN费率须以最大的那个为准，可以依FN不同
 *  2、同一个重点户FN数据点数只能固定为某个数据值（24的倍数）。可以依FN不同
 *  3、本数据文件存取的是国网上行规约定义的二类数据。
 */
#ifndef __DATA_FILE_H__
#define __DATA_FILE_H__

#include "haidef.h"
#include "haicfg.h"

/** @brief 能量数据结构定义 */
struct S_EnergyData
{
	INT8U   m_DataFlag;
	INT16U  m_MeterMP; ///< 电表测量点号
	string m_Data; ///< 能量数据

public:
	S_EnergyData(void){m_DataFlag=0xFF; m_MeterMP=0x00;}
//	S_EnergyData(INT8U DataFlag, INT16U MeterMP, bstring &Data);
};
typedef vector<S_EnergyData> S_EnergyDatas;

/**
 *  @class S_VIPT DataFile.h "share/DataFile.h"
 *  @brief 重点户时标定义
 *  @see 《上行规约》5.13.1.3.2 曲线类数据时标Td_c
 */
struct S_VIPT
{
	time_t m_time;
	INT8U  m_DataDensity;
	INT8U  m_DataDots;
};

/** @brief 数据文件格式
 * 文件头格式排列 
 * |文件标识|最大测量点号|电表保存记录数|记录长度|记录标准时间|记录间隔时间|记录间隔单位|保留|校验|
 * |---16---|------2-----|-------2------|---2----|-----8------|------1-----|------1-----|-14-|--2-|
 * 数据文件头共48个字节，考虑了适当的冗余
 *
 * 能量数据区格式
 * |能量数据区标识|电能表地址|记录队尾|记录队头|
 * |-------10-----|-----6----|----1---|---1----|
 * |记录开始标志|数据标志|记录查找时标|记录1能量数据|
 * |------1-----|----1---|-----3or4---|------N------|
 * |记录开始标志|数据标志|记录查找时标|记录2能量数据|
 * |------1-----|----1---|-----3or4---|------N------|
 * |记录开始标志|数据标志|记录查找时标|记录x能量数据|
 * |------1-----|----1---|-----3or4---|------N------|
 * 共有“最大测量点号”个能量数据区
 */

/** @brief 文件头偏移定义 */
#define DF_SIGNATURE_OFF	0
#define DF_SIGNATURE_SIZE	16
#define DF_MAXMP_OFF		(DF_SIGNATURE_OFF+DF_SIGNATURE_SIZE)
#define DF_MAXMP_SIZE		2
#define DF_PIECEPM_OFF		(DF_MAXMP_OFF+DF_MAXMP_SIZE)
#define DF_PIECEPM_SIZE		2
#define DF_RECORDLEN_OFF	(DF_PIECEPM_OFF+DF_PIECEPM_SIZE)
#define DF_RECORDLEN_SIZE	2
#define DF_RECORDSTD_OFF	(DF_RECORDLEN_OFF+DF_RECORDLEN_SIZE)
#define DF_RECORDSTD_SIZE	8
#define DF_RINTERVAL_OFF	(DF_RECORDSTD_OFF+DF_RECORDSTD_SIZE)
#define DF_RINTERVAL_SIZE	1
#define DF_RINTERUNT_OFF	(DF_RINTERVAL_OFF+DF_RINTERVAL_SIZE)
#define DF_RINTERUNT_SIZE	1
#define DF_RESERVE_OFF		(DF_RINTERUNT_OFF+DF_RINTERUNT_SIZE)
#define DF_RESERVE_SIZE		14
#define DF_CHECKSUM_OFF		(DF_RESERVE_OFF+DF_RESERVE_SIZE)
#define DF_CHECKSUM_SIZE	2
#define DF_HEADER_SIZE		(DF_SIGNATURE_SIZE+DF_MAXMP_SIZE+DF_PIECEPM_SIZE+DF_RECORDLEN_SIZE+DF_RECORDSTD_SIZE+DF_RINTERVAL_SIZE+DF_RINTERUNT_SIZE+DF_RESERVE_SIZE+DF_CHECKSUM_SIZE)

/** @brief 能量数据区偏移 */
#define DF_EAREA_SIGNATURE_OFF		0
#define DF_EAREA_SIGNATURE_SIZE		10
#define DF_EAREA_MADDR_OFF			(DF_EAREA_SIGNATURE_OFF+DF_EAREA_SIGNATURE_SIZE)
#define DF_EAREA_MADDR_SIZE			6
#define DF_EAREA_QTAIL_OFF			(DF_EAREA_MADDR_OFF+DF_EAREA_MADDR_SIZE)
#define DF_EAREA_QTAIL_SIZE			1
#define DF_EAREA_QHEAD_OFF			(DF_EAREA_QTAIL_OFF+DF_EAREA_QTAIL_SIZE)
#define DF_EAREA_QHEAD_SIZE			1

#define DF_RECORD_FLAG_SIZE		1
#define DF_DATA_FLAG_SIZE		1
#define DF_DAY_TIME_SIZE		4
#define DF_MON_TIME_SIZE		3

#define DF_EAREA_INFO_SIZE		(DF_EAREA_SIGNATURE_SIZE+DF_EAREA_MADDR_SIZE+DF_EAREA_QTAIL_SIZE+DF_EAREA_QHEAD_SIZE)


/** @brief 记录保存条数定义 */
#define RECORD_STORE_DAY_MAX	31
#define RECORD_STORE_MON_MAX	12
#define RECORD_STORE_VIP_MAX	10
#define RECORD_STORE_CBR_MAX	31

/** @brief 记录间隔单位 */
#define RECORD_INTER_UNIT_MIN	('m')
#define RECORD_INTER_UNIT_HOUR	('H')
#define RECORD_INTER_UNIT_DAY	('D')
#define RECORD_INTER_UNIT_MON	('M')

#define DF_RECORD_VALID_FLAG		('R')
#define DF_RECORD_INVALID_FLAG		('N')

/** @brief 通用的数据文件头结构定义 */
struct S_DFHeader
{
	INT8U  m_Signature[DF_SIGNATURE_SIZE]; ///< 数据文件标志，目前存文件名
	INT16U m_MaxMP; ///< 当前文件最大电能表测量点号
	INT16U m_PiecePerMeter; ///< 本文件中每块电表保留的记录数
	INT16U m_RecordLen; ///< 记录长度
	INT8U  m_RecordInterVal; ///< 记录时间间隔,重点户中指点数据的间隔
	INT8U  m_RecordInterUnt; ///< 记录时间间隔单位,重点户中指点数据间隔单位
	time_t m_RecordStdT; ///< 记录参考时间
	INT16U m_CheckSum; ///< 文件头校验

public:
	/** @brief 输出本结构体的值按文件头结构形式到缓冲中 */
	int OutputBuffer(INT8U *pBuffer, int size);
	/** @brief 从缓冲中获取文件头的数值 */
	int GetFromBuffer(const INT8U *pBuffer, int size);
	/** @brief 计算check sum */
	int CalculateCheckSum(const INT8U *pBuffer, int size);
};

/** @brief 通用的能量数据区信息结构定义 */
struct S_EAreaInfo
{
	INT8U Sign[DF_EAREA_SIGNATURE_SIZE];
	INT8U MAddr[DF_EAREA_MADDR_SIZE];
	INT8U QTail;
	INT8U QHead;

	/** @brief 从缓冲中获取数据 */
	bool GetFromBuffer(const char *pbuf);
	/** @brief 输出数据到缓冲 */
	bool OutputBuffer(char *pbuf);
};

/**
 *  @class C_DataFileRW DataFile.h "share/DataFile.h"
 *  @brief the father of data file IO; 
 */
class C_DataFileRW
{
public:
	C_DataFileRW(void){m_VipPointPerDay = VIPDATA_POINT_PERDAY; m_EnergyDataLen = 0;}
	virtual ~C_DataFileRW(){}

public:
	/** @brief 数据文件初始化 */
	virtual bool InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP) = 0;
	/** @brief 获取一次写入数据的大小 */
	virtual INT16U GetOnceWriteDataSize(void){return GetRecordLen();}
	/** @brief 将时间按数据类型截齐 */
	virtual time_t DataTimeTrim(time_t tm) = 0;
	/** @brief 合并数据到记录 */
	virtual int CombineRecord(char *pEnergyArea, const S_EnergyData &EnergyData, time_t DataTime);
	/** @brief 从数据区提取记录 */
	virtual int ExtractRecord(char *pEnergyArea, S_EnergyData &EnergyData, time_t DataTime);
	/** @brief 查找符合时标的记录 */
	virtual int FindRecord(const S_EAreaInfo &EAI, const char *pRecord, const char *TimeTag);
	/** @brief 制作时标 */
	virtual char *MakeTimeTag(char *TimeTag, time_t time);
	/** @brief 获取记录时标长度 */
	virtual int GetTimeTagLength(void){return DF_DAY_TIME_SIZE;}

	/** @brief 读取一条数据记录 */
	int  Read(FILE *fp, S_EnergyData &EnergyData, time_t DataTime);
	/** @brief 写入一条数据记录 */
	int  Write(FILE *fp, const S_EnergyData &EnergyData, time_t DataTime);
	/** @brief 增表后加大文件 */
	bool AppendDataFile(FILE *fp, INT16U MaxMP);
	/** @brief 获取记录长度 */
	INT16U GetRecordLen(void){return m_Header.m_RecordLen;}
	/** @brief 获取最大测量点号 */
	INT16U GetMaxMP(void){return m_Header.m_MaxMP;}
	/** @brief 写数据文件头标识 */
	void InitDataFileSignature(INT8U *pbuff, INT16U FN, char *psign){sprintf((char*)pbuff, "class2f%d_%s", FN, psign);}
	/** @brief 写能量数据区标识 */
	void InitEAreaSignature(INT8U *pbuff, INT16U MP){sprintf((char*)pbuff, "meter%04d", MP);}
	/** @brief 初始化能量数据区缓冲 */
	int  InitEnergyArea(INT8U *pbuff, INT16U size, INT16U MP);
	/** @brief 读取文件信息 */
	int  GetFileInfoHeader(FILE *fp);
	/** @brief 设置重点户FN一天的数据点数 */
	INT16U SetVipPoint(INT16U VipPointPerDay){m_VipPointPerDay = VipPointPerDay?VipPointPerDay:m_VipPointPerDay;return m_VipPointPerDay;}
	/** @brief 设置初始数据度 */
	INT16U SetEnergyDataLen(INT16U EnergyDataLen){return m_EnergyDataLen = EnergyDataLen;}
	/** @brief 获取能量数据容量 */
	int GetEnergyAreaSize(void);

protected:
	/** @brief 获取FN要求的记录长度 */
	INT16U GetRecordLen(INT16U FN);
	/** @brief reset 数据文件 */ 
	bool ResetDataFile(FILE *fp);

protected:
	S_DFHeader m_Header;
	INT16U m_VipPointPerDay;
	INT16U m_EnergyDataLen;
};

/**
 *  @class C_DataFileRW_D DataFile.h "share/DataFile.h"
 *  @brief 日数据文件操作IO; 
 */
class C_DataFileRW_D:public C_DataFileRW
{
public:
	C_DataFileRW_D(void){}
	~C_DataFileRW_D(){}

public:
	/** @brief 数据文件初始化 */
	bool InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP);
	/** @brief 将时间按数据类型截齐 */
	time_t DataTimeTrim(time_t tm);
};

/**
 *  @class C_DataFileRW_M DataFile.h "share/DataFile.h"
 *  @brief 月数据文件操作IO; 
 */
class C_DataFileRW_M:public C_DataFileRW
{
public:
	C_DataFileRW_M(void){}
	~C_DataFileRW_M(){}

public:
	/** @brief 数据文件初始化 */
	bool InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP);
	/** @brief 将时间按数据类型截齐 */
	time_t DataTimeTrim(time_t tm);
	/** @brief 制作时标 */
	char *MakeTimeTag(char *TimeTag, time_t time);
	/** @brief 获取记录时标长度 */
	int GetTimeTagLength(void);
};

/**
 *  @class C_DataFileRW_V DataFile.h "share/DataFile.h"
 *  @brief 重点户数据文件操作IO; 
 */
class C_DataFileRW_V:public C_DataFileRW
{
public:
	C_DataFileRW_V(void){}
	~C_DataFileRW_V(){}

public:
	/** @brief 数据文件初始化 */
	bool InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP);
	/** @brief 获取一次写入数据的大小 */
	INT16U GetOnceWriteDataSize(void){return (GetRecordLen()/((24*60)/m_Header.m_RecordInterVal)/*VIPDATA_POINT_PERDAY*/);}
	/** @brief 将时间按数据类型截齐 */
	time_t DataTimeTrim(time_t tm);
	/** @brief 合并数据到记录 */
	int CombineRecord(char *pEnergyArea, const S_EnergyData &EnergyData, time_t DataTime);
};

/**
 *  @class C_DataFileRW_G DataFile.h "share/DataFile.h"
 *  @brief 抄表日数据文件操作IO; 
 */
class C_DataFileRW_G:public C_DataFileRW
{
public:
	C_DataFileRW_G(void){}
	~C_DataFileRW_G(){}

public:
	/** @brief 数据文件初始化 */
	bool InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP);
	/** @brief 将时间按数据类型截齐 */
	time_t DataTimeTrim(time_t tm);
};



/**
 *  @class C_DFRWFACT DataFile.h "share/DataFile.h"
 *  @brief 能量数据类读写实例工厂
 */
class C_DFRWFACT
{
public:
	enum{CLASS2_UNKNOW, CLASS2_VIP, CLASS2_DAY, CLASS2_MON, CLASS2_GTH};
	enum{CLASS2PN_UNKNOW, CLASS2PN_MPOINT, CLASS2PN_TGROUP, CLASS2PN_PORT, CLASS2PN_TERM};
	/** @brief 获取一个数据读写实例 */
	static C_DataFileRW *GetDataFileRW(INT16U FN);
	/** @brief 释放一个数据读写实例 */
	static void FreeDataFileRW(C_DataFileRW *pDFRW);
	/** @brief 获取二类数据FN类型 */
	static int  GetType4Class2FN(INT16U FN);
	/** @brief  获取FN对应的PN意义*/
	static int GetPnMean4Class2Fn(INT16U FN);
};

/**
 *  @class C_DATAFILE DataFile.h "share/DataFile.h"
 *  @brief General data file OP.
 */
class C_DATAFILE
{
public:
	enum{OPEN_READ, OPEN_WRITE};

public:
	C_DATAFILE(const char *fPath, INT8U io, INT16U FN);
	~C_DATAFILE();

public:
	/** @brief 读取一条记录 */
	int  Read(S_EnergyData &EnergyData, time_t DataTime);
	/** @brief 读取重点户一条记录 */
	int  ReadVip(S_EnergyData &EnergyData, const S_VIPT &VipTime);
	/** @brief 写入一条记录 */
	int  Write(const S_EnergyData &EnergyData, time_t DataTime);
	/** @brief 判断实例是否可操作 */
	bool operator!(void){return (m_fp==NULL || m_pdfrw==NULL);}
	/** @brief 判断是否新建 */
	bool IsNew(void);
	/** @brief 初始化数据文件 */
	bool InitDataFile(INT16U MaxMP);
	/** @brief 获取文件信息头 */
	int  GetFileHeader(void);
	/** @brief 将时间按数据类型截齐 */
	time_t DataTimeTrim(time_t tm);
	/** @brief 设置重点户FN一天的数据点数 */
	INT16U SetVipPoint(INT16U VipPointPerDay){return m_pdfrw->SetVipPoint(VipPointPerDay);}
	/** @brief 设置初始数据度 */
	INT16U SetEnergyDataLen(INT16U EnergyDataLen){return m_pdfrw->SetEnergyDataLen(EnergyDataLen);}
	/** @brief 判断是否为无效FN */
	bool IsInvalidMP(INT16U MP){return !(MP>0 && MP<GW_MP_MAX);}
	/** @brief 获取重点户冻结时间间隔 */
	int  GetVipFreezeIntervalM(INT8U FreezeCode);

private:
	C_DATAFILE(void);
	/** @brief 打开数据文件 */
	bool Open(const char *pfile, INT8U io);
	/** @brief 关闭数据文件 */
	bool Close(void);

private:
	INT16U m_FN;
	FILE   *m_fp;
	C_DataFileRW *m_pdfrw;
};


//
// 用户接口定义
//
/**
 *  @class C_DataFileOP DataFile.h "share/DataFile.h"
 *  @brief the interface of energy data file IO.
 */
class C_DataFileOP
{
public:
	
	enum{m_FillChar = 0xFF};

	/** @brief 二类数据写入函数
	 *  @param[in] FN 国网要求的二类数据FN
	 *  @param[in] MeterDatas 能量数据组，参见S_EnergyDatas定义
	 *  @param[in] DataTime 时标
	 *  @param[in] VipPointPerDay 重点户FN每天须要的数据点数据，仅过重点户FN有效
	 *  @return 写入的记录条数
	 */
	static int WriteClass2Data(INT16U FN, const S_EnergyDatas &MeterDatas, time_t DataTime=0, int VipPointPerDay=0);
	/** @brief 二类数据读取函数
	 *  @param[in] FN 国网要求的二类数据FN
	 *  @param[inout] MeterDatas 能量数据组，参见S_EnergyDatas定义
	 *  @param[in] DataTime 时标
	 *  @return 读取的记录条数（重点户一条记录是指一天的数据）
	 */
	static int ReadClass2Data(INT16U FN, S_EnergyDatas &MeterDatas, time_t DataTime=0);
	/** @brief 二类重点户数据读取函数
	 *  @param[in] FN 国网要求的二类数据FN，调用者必须保证为重点户FN
	 *  @param[inout] MeterDatas 能量数据组，参见S_EnergyDatas定义
	 *  @param[in] VipTime 重点户时标
	 *  @return 读取的记录条数
	 */
	static int ReadClass2DataVIP(INT16U FN, S_EnergyDatas &MeterDatas, const S_VIPT &VipTime);

private:
	/** @brief 二类数据写入函数
	 *  @param[in] FN 国网要求的二类数据FN
	 *  @param[in] MeterDatas 能量数据组，参见S_EnergyDatas定义
	 *  @param[in] DataTime 时标
	 *  @param[in] VipPointPerDay 重点户FN每天须要的数据点数据，仅过重点户FN有效
	 *  @return 写入的记录条数
	 */
	static int _WriteClass2Data(INT16U FN, const S_EnergyDatas &MeterDatas, time_t DataTime=0, int VipPointPerDay=0);
	/** @brief 二类数据读取函数
	 *  @param[in] FN 国网要求的二类数据FN
	 *  @param[inout] MeterDatas 能量数据组，参见S_EnergyDatas定义
	 *  @param[in] DataTime 时标
	 *  @return 读取的记录条数（重点户一条记录是指一天的数据）
	 */
	static int _ReadClass2Data(INT16U FN, S_EnergyDatas &MeterDatas, time_t DataTime=0);
	/** @brief 生成数据文件路径
	 *  @param[out] fpath 文件路径输出缓冲，最小260字节
	 *  @param[in] FN 国网要求二类数据FN
	 *  @return fpath的指针
	 */
	static char *MakeDataFilePath(char *fPath, INT16U FN);
};


#endif //__DATA_FILE_H__




