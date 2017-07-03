/**
 *  @file datafile.h 
 *  @brief ���������������ļ���ȡ������
 *  @author jinhailiao
 *  @date 2009-11-24
 *  @version v1.0
 *  @note
 *  1��ͬһ��FN�������������Ǹ�Ϊ׼��������FN��ͬ
 *  2��ͬһ���ص㻧FN���ݵ���ֻ�̶ܹ�Ϊĳ������ֵ��24�ı�������������FN��ͬ
 *  3���������ļ���ȡ���ǹ������й�Լ����Ķ������ݡ�
 */
#ifndef __DATA_FILE_H__
#define __DATA_FILE_H__

#include "haidef.h"
#include "haicfg.h"

/** @brief �������ݽṹ���� */
struct S_EnergyData
{
	INT8U   m_DataFlag;
	INT16U  m_MeterMP; ///< ���������
	string m_Data; ///< ��������

public:
	S_EnergyData(void){m_DataFlag=0xFF; m_MeterMP=0x00;}
//	S_EnergyData(INT8U DataFlag, INT16U MeterMP, bstring &Data);
};
typedef vector<S_EnergyData> S_EnergyDatas;

/**
 *  @class S_VIPT DataFile.h "share/DataFile.h"
 *  @brief �ص㻧ʱ�궨��
 *  @see �����й�Լ��5.13.1.3.2 ����������ʱ��Td_c
 */
struct S_VIPT
{
	time_t m_time;
	INT8U  m_DataDensity;
	INT8U  m_DataDots;
};

/** @brief �����ļ���ʽ
 * �ļ�ͷ��ʽ���� 
 * |�ļ���ʶ|���������|������¼��|��¼����|��¼��׼ʱ��|��¼���ʱ��|��¼�����λ|����|У��|
 * |---16---|------2-----|-------2------|---2----|-----8------|------1-----|------1-----|-14-|--2-|
 * �����ļ�ͷ��48���ֽڣ��������ʵ�������
 *
 * ������������ʽ
 * |������������ʶ|���ܱ��ַ|��¼��β|��¼��ͷ|
 * |-------10-----|-----6----|----1---|---1----|
 * |��¼��ʼ��־|���ݱ�־|��¼����ʱ��|��¼1��������|
 * |------1-----|----1---|-----3or4---|------N------|
 * |��¼��ʼ��־|���ݱ�־|��¼����ʱ��|��¼2��������|
 * |------1-----|----1---|-----3or4---|------N------|
 * |��¼��ʼ��־|���ݱ�־|��¼����ʱ��|��¼x��������|
 * |------1-----|----1---|-----3or4---|------N------|
 * ���С���������š�������������
 */

/** @brief �ļ�ͷƫ�ƶ��� */
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

/** @brief ����������ƫ�� */
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


/** @brief ��¼������������ */
#define RECORD_STORE_DAY_MAX	31
#define RECORD_STORE_MON_MAX	12
#define RECORD_STORE_VIP_MAX	10
#define RECORD_STORE_CBR_MAX	31

/** @brief ��¼�����λ */
#define RECORD_INTER_UNIT_MIN	('m')
#define RECORD_INTER_UNIT_HOUR	('H')
#define RECORD_INTER_UNIT_DAY	('D')
#define RECORD_INTER_UNIT_MON	('M')

#define DF_RECORD_VALID_FLAG		('R')
#define DF_RECORD_INVALID_FLAG		('N')

/** @brief ͨ�õ������ļ�ͷ�ṹ���� */
struct S_DFHeader
{
	INT8U  m_Signature[DF_SIGNATURE_SIZE]; ///< �����ļ���־��Ŀǰ���ļ���
	INT16U m_MaxMP; ///< ��ǰ�ļ������ܱ�������
	INT16U m_PiecePerMeter; ///< ���ļ���ÿ�������ļ�¼��
	INT16U m_RecordLen; ///< ��¼����
	INT8U  m_RecordInterVal; ///< ��¼ʱ����,�ص㻧��ָ�����ݵļ��
	INT8U  m_RecordInterUnt; ///< ��¼ʱ������λ,�ص㻧��ָ�����ݼ����λ
	time_t m_RecordStdT; ///< ��¼�ο�ʱ��
	INT16U m_CheckSum; ///< �ļ�ͷУ��

public:
	/** @brief ������ṹ���ֵ���ļ�ͷ�ṹ��ʽ�������� */
	int OutputBuffer(INT8U *pBuffer, int size);
	/** @brief �ӻ����л�ȡ�ļ�ͷ����ֵ */
	int GetFromBuffer(const INT8U *pBuffer, int size);
	/** @brief ����check sum */
	int CalculateCheckSum(const INT8U *pBuffer, int size);
};

/** @brief ͨ�õ�������������Ϣ�ṹ���� */
struct S_EAreaInfo
{
	INT8U Sign[DF_EAREA_SIGNATURE_SIZE];
	INT8U MAddr[DF_EAREA_MADDR_SIZE];
	INT8U QTail;
	INT8U QHead;

	/** @brief �ӻ����л�ȡ���� */
	bool GetFromBuffer(const char *pbuf);
	/** @brief ������ݵ����� */
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
	/** @brief �����ļ���ʼ�� */
	virtual bool InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP) = 0;
	/** @brief ��ȡһ��д�����ݵĴ�С */
	virtual INT16U GetOnceWriteDataSize(void){return GetRecordLen();}
	/** @brief ��ʱ�䰴�������ͽ��� */
	virtual time_t DataTimeTrim(time_t tm) = 0;
	/** @brief �ϲ����ݵ���¼ */
	virtual int CombineRecord(char *pEnergyArea, const S_EnergyData &EnergyData, time_t DataTime);
	/** @brief ����������ȡ��¼ */
	virtual int ExtractRecord(char *pEnergyArea, S_EnergyData &EnergyData, time_t DataTime);
	/** @brief ���ҷ���ʱ��ļ�¼ */
	virtual int FindRecord(const S_EAreaInfo &EAI, const char *pRecord, const char *TimeTag);
	/** @brief ����ʱ�� */
	virtual char *MakeTimeTag(char *TimeTag, time_t time);
	/** @brief ��ȡ��¼ʱ�곤�� */
	virtual int GetTimeTagLength(void){return DF_DAY_TIME_SIZE;}

	/** @brief ��ȡһ�����ݼ�¼ */
	int  Read(FILE *fp, S_EnergyData &EnergyData, time_t DataTime);
	/** @brief д��һ�����ݼ�¼ */
	int  Write(FILE *fp, const S_EnergyData &EnergyData, time_t DataTime);
	/** @brief �����Ӵ��ļ� */
	bool AppendDataFile(FILE *fp, INT16U MaxMP);
	/** @brief ��ȡ��¼���� */
	INT16U GetRecordLen(void){return m_Header.m_RecordLen;}
	/** @brief ��ȡ��������� */
	INT16U GetMaxMP(void){return m_Header.m_MaxMP;}
	/** @brief д�����ļ�ͷ��ʶ */
	void InitDataFileSignature(INT8U *pbuff, INT16U FN, char *psign){sprintf((char*)pbuff, "class2f%d_%s", FN, psign);}
	/** @brief д������������ʶ */
	void InitEAreaSignature(INT8U *pbuff, INT16U MP){sprintf((char*)pbuff, "meter%04d", MP);}
	/** @brief ��ʼ���������������� */
	int  InitEnergyArea(INT8U *pbuff, INT16U size, INT16U MP);
	/** @brief ��ȡ�ļ���Ϣ */
	int  GetFileInfoHeader(FILE *fp);
	/** @brief �����ص㻧FNһ������ݵ��� */
	INT16U SetVipPoint(INT16U VipPointPerDay){m_VipPointPerDay = VipPointPerDay?VipPointPerDay:m_VipPointPerDay;return m_VipPointPerDay;}
	/** @brief ���ó�ʼ���ݶ� */
	INT16U SetEnergyDataLen(INT16U EnergyDataLen){return m_EnergyDataLen = EnergyDataLen;}
	/** @brief ��ȡ������������ */
	int GetEnergyAreaSize(void);

protected:
	/** @brief ��ȡFNҪ��ļ�¼���� */
	INT16U GetRecordLen(INT16U FN);
	/** @brief reset �����ļ� */ 
	bool ResetDataFile(FILE *fp);

protected:
	S_DFHeader m_Header;
	INT16U m_VipPointPerDay;
	INT16U m_EnergyDataLen;
};

/**
 *  @class C_DataFileRW_D DataFile.h "share/DataFile.h"
 *  @brief �������ļ�����IO; 
 */
class C_DataFileRW_D:public C_DataFileRW
{
public:
	C_DataFileRW_D(void){}
	~C_DataFileRW_D(){}

public:
	/** @brief �����ļ���ʼ�� */
	bool InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP);
	/** @brief ��ʱ�䰴�������ͽ��� */
	time_t DataTimeTrim(time_t tm);
};

/**
 *  @class C_DataFileRW_M DataFile.h "share/DataFile.h"
 *  @brief �������ļ�����IO; 
 */
class C_DataFileRW_M:public C_DataFileRW
{
public:
	C_DataFileRW_M(void){}
	~C_DataFileRW_M(){}

public:
	/** @brief �����ļ���ʼ�� */
	bool InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP);
	/** @brief ��ʱ�䰴�������ͽ��� */
	time_t DataTimeTrim(time_t tm);
	/** @brief ����ʱ�� */
	char *MakeTimeTag(char *TimeTag, time_t time);
	/** @brief ��ȡ��¼ʱ�곤�� */
	int GetTimeTagLength(void);
};

/**
 *  @class C_DataFileRW_V DataFile.h "share/DataFile.h"
 *  @brief �ص㻧�����ļ�����IO; 
 */
class C_DataFileRW_V:public C_DataFileRW
{
public:
	C_DataFileRW_V(void){}
	~C_DataFileRW_V(){}

public:
	/** @brief �����ļ���ʼ�� */
	bool InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP);
	/** @brief ��ȡһ��д�����ݵĴ�С */
	INT16U GetOnceWriteDataSize(void){return (GetRecordLen()/((24*60)/m_Header.m_RecordInterVal)/*VIPDATA_POINT_PERDAY*/);}
	/** @brief ��ʱ�䰴�������ͽ��� */
	time_t DataTimeTrim(time_t tm);
	/** @brief �ϲ����ݵ���¼ */
	int CombineRecord(char *pEnergyArea, const S_EnergyData &EnergyData, time_t DataTime);
};

/**
 *  @class C_DataFileRW_G DataFile.h "share/DataFile.h"
 *  @brief �����������ļ�����IO; 
 */
class C_DataFileRW_G:public C_DataFileRW
{
public:
	C_DataFileRW_G(void){}
	~C_DataFileRW_G(){}

public:
	/** @brief �����ļ���ʼ�� */
	bool InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP);
	/** @brief ��ʱ�䰴�������ͽ��� */
	time_t DataTimeTrim(time_t tm);
};



/**
 *  @class C_DFRWFACT DataFile.h "share/DataFile.h"
 *  @brief �����������дʵ������
 */
class C_DFRWFACT
{
public:
	enum{CLASS2_UNKNOW, CLASS2_VIP, CLASS2_DAY, CLASS2_MON, CLASS2_GTH};
	enum{CLASS2PN_UNKNOW, CLASS2PN_MPOINT, CLASS2PN_TGROUP, CLASS2PN_PORT, CLASS2PN_TERM};
	/** @brief ��ȡһ�����ݶ�дʵ�� */
	static C_DataFileRW *GetDataFileRW(INT16U FN);
	/** @brief �ͷ�һ�����ݶ�дʵ�� */
	static void FreeDataFileRW(C_DataFileRW *pDFRW);
	/** @brief ��ȡ��������FN���� */
	static int  GetType4Class2FN(INT16U FN);
	/** @brief  ��ȡFN��Ӧ��PN����*/
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
	/** @brief ��ȡһ����¼ */
	int  Read(S_EnergyData &EnergyData, time_t DataTime);
	/** @brief ��ȡ�ص㻧һ����¼ */
	int  ReadVip(S_EnergyData &EnergyData, const S_VIPT &VipTime);
	/** @brief д��һ����¼ */
	int  Write(const S_EnergyData &EnergyData, time_t DataTime);
	/** @brief �ж�ʵ���Ƿ�ɲ��� */
	bool operator!(void){return (m_fp==NULL || m_pdfrw==NULL);}
	/** @brief �ж��Ƿ��½� */
	bool IsNew(void);
	/** @brief ��ʼ�������ļ� */
	bool InitDataFile(INT16U MaxMP);
	/** @brief ��ȡ�ļ���Ϣͷ */
	int  GetFileHeader(void);
	/** @brief ��ʱ�䰴�������ͽ��� */
	time_t DataTimeTrim(time_t tm);
	/** @brief �����ص㻧FNһ������ݵ��� */
	INT16U SetVipPoint(INT16U VipPointPerDay){return m_pdfrw->SetVipPoint(VipPointPerDay);}
	/** @brief ���ó�ʼ���ݶ� */
	INT16U SetEnergyDataLen(INT16U EnergyDataLen){return m_pdfrw->SetEnergyDataLen(EnergyDataLen);}
	/** @brief �ж��Ƿ�Ϊ��ЧFN */
	bool IsInvalidMP(INT16U MP){return !(MP>0 && MP<GW_MP_MAX);}
	/** @brief ��ȡ�ص㻧����ʱ���� */
	int  GetVipFreezeIntervalM(INT8U FreezeCode);

private:
	C_DATAFILE(void);
	/** @brief �������ļ� */
	bool Open(const char *pfile, INT8U io);
	/** @brief �ر������ļ� */
	bool Close(void);

private:
	INT16U m_FN;
	FILE   *m_fp;
	C_DataFileRW *m_pdfrw;
};


//
// �û��ӿڶ���
//
/**
 *  @class C_DataFileOP DataFile.h "share/DataFile.h"
 *  @brief the interface of energy data file IO.
 */
class C_DataFileOP
{
public:
	
	enum{m_FillChar = 0xFF};

	/** @brief ��������д�뺯��
	 *  @param[in] FN ����Ҫ��Ķ�������FN
	 *  @param[in] MeterDatas ���������飬�μ�S_EnergyDatas����
	 *  @param[in] DataTime ʱ��
	 *  @param[in] VipPointPerDay �ص㻧FNÿ����Ҫ�����ݵ����ݣ������ص㻧FN��Ч
	 *  @return д��ļ�¼����
	 */
	static int WriteClass2Data(INT16U FN, const S_EnergyDatas &MeterDatas, time_t DataTime=0, int VipPointPerDay=0);
	/** @brief �������ݶ�ȡ����
	 *  @param[in] FN ����Ҫ��Ķ�������FN
	 *  @param[inout] MeterDatas ���������飬�μ�S_EnergyDatas����
	 *  @param[in] DataTime ʱ��
	 *  @return ��ȡ�ļ�¼�������ص㻧һ����¼��ָһ������ݣ�
	 */
	static int ReadClass2Data(INT16U FN, S_EnergyDatas &MeterDatas, time_t DataTime=0);
	/** @brief �����ص㻧���ݶ�ȡ����
	 *  @param[in] FN ����Ҫ��Ķ�������FN�������߱��뱣֤Ϊ�ص㻧FN
	 *  @param[inout] MeterDatas ���������飬�μ�S_EnergyDatas����
	 *  @param[in] VipTime �ص㻧ʱ��
	 *  @return ��ȡ�ļ�¼����
	 */
	static int ReadClass2DataVIP(INT16U FN, S_EnergyDatas &MeterDatas, const S_VIPT &VipTime);

private:
	/** @brief ��������д�뺯��
	 *  @param[in] FN ����Ҫ��Ķ�������FN
	 *  @param[in] MeterDatas ���������飬�μ�S_EnergyDatas����
	 *  @param[in] DataTime ʱ��
	 *  @param[in] VipPointPerDay �ص㻧FNÿ����Ҫ�����ݵ����ݣ������ص㻧FN��Ч
	 *  @return д��ļ�¼����
	 */
	static int _WriteClass2Data(INT16U FN, const S_EnergyDatas &MeterDatas, time_t DataTime=0, int VipPointPerDay=0);
	/** @brief �������ݶ�ȡ����
	 *  @param[in] FN ����Ҫ��Ķ�������FN
	 *  @param[inout] MeterDatas ���������飬�μ�S_EnergyDatas����
	 *  @param[in] DataTime ʱ��
	 *  @return ��ȡ�ļ�¼�������ص㻧һ����¼��ָһ������ݣ�
	 */
	static int _ReadClass2Data(INT16U FN, S_EnergyDatas &MeterDatas, time_t DataTime=0);
	/** @brief ���������ļ�·��
	 *  @param[out] fpath �ļ�·��������壬��С260�ֽ�
	 *  @param[in] FN ����Ҫ���������FN
	 *  @return fpath��ָ��
	 */
	static char *MakeDataFilePath(char *fPath, INT16U FN);
};


#endif //__DATA_FILE_H__




