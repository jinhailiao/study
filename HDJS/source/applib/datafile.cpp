/**
 *  @file datafile.cpp 
 *  @brief ���������������ļ���ȡ������
 *  @author jinhailiao
 *  @date 2009-11-24
 *  @version v1.0
 *  @note
 *  1��ͬһ��FN�������������Ǹ�Ϊ׼��������FN��ͬ
 *  2��ͬһ���ص㻧FN���ݵ���ֻ�̶ܹ�Ϊĳ������ֵ��24�ı�������������FN��ͬ
 *  3���������ļ���ȡ���ǹ������й�Լ����Ķ������ݡ�
 */
#include "datafile.h"
#include "haidef.h"
#include "timeop.h"
#include "fileop.h"

/** @brief ����������󳤶�
  *  @note
  *   0 ��ʾ��Чֵ�����ִ�ֵӦע��
  *  -1 ��ʾ������ע����ʵ��Ϊ׼
  *  ����Ϊ���ݳ��ȶ���ֵ
  */
const short Class2FnLen[] =
{
	 0,//F0 ��ЧFN
	//��1  ����ʾֵ����������������� 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F1 ������/�޹�����ʾֵ��һ/�������޹�����ʾֵ���ܡ�����1��M��1��M��12�� 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F2 ������/�޹�����ʾֵ����/�������޹�����ʾֵ���ܡ�����1��M��1��M��12�� 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F3 ������/�޹��������������ʱ�䣨�ܡ�����1��M��1��M��12�� 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F4 ������/�޹��������������ʱ�䣨�ܡ�����1��M��1��M��12�� 
	(1+4+4*GW_TARIFF_MAX),//F5  �����й����������ܡ�����1��M�� 
	(1+4+4*GW_TARIFF_MAX),//F6  �����޹����������ܡ�����1��M�� 
	(1+4+4*GW_TARIFF_MAX),//F7  �����й����������ܡ�����1��M�� 
	(1+4+4*GW_TARIFF_MAX),//F8  �����޹����������ܡ�����1��M�� 
	//��2  ����ʾֵ��������� 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F9 ������/�޹�����ʾֵ��һ/�������޹�����ʾֵ���ܡ�����1��M��1��M��12�� 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F10 ������/�޹�����ʾֵ����/�������޹�����ʾֵ���ܡ�����1��M��1��M��12�� 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F11 ������/�޹��������������ʱ�䣨�ܡ�����1��M��1��M��12�� 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F12 ������/�޹��������������ʱ�䣨�ܡ�����1��M��1��M��12�� 
	0,0,0,0,//F13��F16  ���� 
	//��3  ����ʾֵ����������������� 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F17 ������/�޹�����ʾֵ��һ/�������޹�����ʾֵ���ܡ�����1��M��1��M��12�� 
	(5+1+5+5*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F18 ������/�޹�����ʾֵ����/�������޹�����ʾֵ���ܡ�����1��M��1��M��12�� 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F19 ������/�޹��������������ʱ�䣨�ܡ�����1��M��1��M��12�� 
	(5+1+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX+3+3*GW_TARIFF_MAX+4+4*GW_TARIFF_MAX),//F20 ������/�޹��������������ʱ�䣨�ܡ�����1��M��1��M��12�� 
	(1+4+4*GW_TARIFF_MAX),//F21  �����й����������ܡ�����1��M�� 
	(1+4+4*GW_TARIFF_MAX),//F22  �����޹����������ܡ�����1��M�� 
	(1+4+4*GW_TARIFF_MAX),//F23  �����й����������ܡ�����1��M�� 
	(1+4+4*GW_TARIFF_MAX),//F24  �����޹����������ܡ�����1��M�� 
	//��4   
	-1,//F25 ���ܼ���������й����ʼ�����ʱ�䡢�й�����Ϊ��ʱ�� 
	-1,//F26  ���ܼ������������������ʱ�� 
	-1,//F27  �յ�ѹͳ������ 
	-1,//F28  �ղ�ƽ���Խ���ۼ�ʱ�� 
	-1,//F29  �յ���Խ��ͳ�� 
	-1,//F30  �����ڹ���Խ���ۼ�ʱ�� 
	-1,//F31  �ո�������ͳ�� 
	-1,//F32  �յ��ܱ�������� 
	//��5   
	-1,//F33 ���ܼ���������й����ʼ�����ʱ�䡢�й�����Ϊ��ʱ�� 
	-1,//F34  ���ܼ������й��������������ʱ�� 
	-1,//F35  �µ�ѹͳ������ 
	-1,//F36  �²�ƽ���Խ���ۼ�ʱ�� 
	-1,//F37  �µ���Խ��ͳ�� 
	-1,//F38  �����ڹ���Խ���ۼ�ʱ�� 
	-1,//F39  �¸�������ͳ�� 
	 0,//F40  ���� 
	//��6   
	-1,//F41  ������Ͷ���ۼ�ʱ��ʹ��� 
	-1,//F42  �ա��µ������ۼƲ������޹������� 
	-1,//F43  �չ������������ۼ�ʱ�� 
	-1,//F44  �¹������������ۼ�ʱ�� 
	-1,//F45  ͭ�������й�����ʾֵ 
	-1,//F46  ͭ�������й�����ʾֵ 
	 0,0,//F47��F48  ���� 
	//��7  �ն�ͳ������ 
	-1,//F49  �ն��չ���ʱ�䡢�ո�λ�ۼƴ��� 
	-1,//F50  �ն��տ���ͳ������ 
	-1,//F51  �ն��¹���ʱ�䡢�¸�λ�ۼƴ��� 
	-1,//F52  �ն��¿���ͳ������ 
	-1,//F53  �ն�����վ��ͨ������ 
	-1,//F54  �ն�����վ��ͨ������ 
	 0,0,//F55��F56  ���� 
	//��8  �ܼ���ͳ������ 
	-1,//F57 �ܼ����������С�й����ʼ��䷢��ʱ�䣬�й�����Ϊ�����ۼ�ʱ�� 
	(1+4+4*GW_TARIFF_MAX),//F58  �ܼ������ۼ��й����������ܡ�����1��M�� 
	(1+4+4*GW_TARIFF_MAX),//F59  �ܼ������ۼ��޹����������ܡ�����1��M�� 
	-1,//F60 �ܼ����������С�й����ʼ��䷢��ʱ�䣬�й�����Ϊ�����ۼ�ʱ�� 
	(1+4+4*GW_TARIFF_MAX),//F61  �ܼ������ۼ��й����������ܡ�����1��M�� 
	(1+4+4*GW_TARIFF_MAX),//F62  �ܼ������ۼ��޹����������ܡ�����1��M�� 
	 0,0,//F63��F64  ���� 
	//��9  �ܼ���Խ��ͳ������ 
	-1,//F65  �ܼ��鳬���ʶ�ֵ�����ۼ�ʱ�䡢���ۼƵ����� 
	-1,//F66 �ܼ��鳬�µ�������ֵ�����ۼ�ʱ�䡢�ۼƵ����� 
	 0,0,0,0,0,0,//F67��F72  ���� 
	//��10  �ܼ������� 
	-1,//F73  �ܼ����й��������� 
	-1,//F74  �ܼ����޹��������� 
	-1,//F75  �ܼ����й����������� 
	-1,//F76  �ܼ����޹����������� 
	 0,0,0,0,//F77��F80  ���� 
	//��11  �������� 
	-1,//F81  �й��������� 
	-1,//F82  A���й��������� 
	-1,//F83  B���й��������� 
	-1,//F84  C���й��������� 
	-1,//F85  �޹��������� 
	-1,//F86  A���޹��������� 
	-1,//F87  B���޹��������� 
	-1,//F88  C���޹��������� 
	//��12  ��ѹ�������� 
	-1,//F89  A���ѹ���� 
	-1,//F90  B���ѹ���� 
	-1,//F91  C���ѹ���� 
	-1,//F92  A��������� 
	-1,//F93  B��������� 
	-1,//F94  C��������� 
	-1,//F95  ����������� 
	 0,//F96  ���� 
	//��13  �ܵ��������ܵ���ʾֵ���� 
	-1,//F97  �����й��ܵ����� 
	-1,//F98  �����޹��ܵ����� 
	-1,//F99  �����й��ܵ����� 
	-1,//F100  �����޹��ܵ����� 
	-1,//F101  �����й��ܵ���ʾֵ 
	-1,//F102  �����޹��ܵ���ʾֵ 
	-1,//F103  �����й��ܵ���ʾֵ 
	-1,//F104  �����޹��ܵ���ʾֵ 
	//��14  ������������ѹ��λ�����ߡ�������λ������ 
	-1,//F105  �ܹ������� 
	-1,//F106  A�๦������ 
	-1,//F107  B�๦������ 
	-1,//F108  C�๦������ 
	-1,//F109  ��ѹ��λ������ 
	-1,//F110  ������λ������ 
	 0,0,//F111��F112  ���� 
	//��15  г�����ͳ������ 
	-1,//F113  A��2��19��г�����������ֵ������ʱ�� 
	-1,//F114  B��2��19��г�����������ֵ������ʱ�� 
	-1,//F115  C��2��19��г�����������ֵ������ʱ�� 
	-1,//F116 A��2��19��г����ѹ�����ʼ��ܻ����������ֵ������ʱ�� 
	-1,//F117 B��2��19��г����ѹ�����ʼ��ܻ����������ֵ������ʱ�� 
	-1,//F118 C��2��19��г����ѹ�����ʼ��ܻ����������ֵ������ʱ�� 
	 0,0,//F119��F120  ���� 
	//��16  г��Խ��ͳ������ 
	-1,//F121  A��г��Խ����ͳ������ 
	-1,//F122  B��г��Խ����ͳ������ 
	-1,//F123  C��г��Խ����ͳ������ 
	 0,0,0,0,0,//F124��F128  ���� 
	//��17  ֱ��ģ�������� 
	-1,//F129 ֱ��ģ����Խ�����ۼ�ʱ�䡢���/��Сֵ������ʱ�� 
	-1,//F130 ֱ��ģ����Խ�����ۼ�ʱ�䡢���/��Сֵ������ʱ�� 
	 0,0,0,0,0,0,//F131��F136  ���� 
	//��18  ֱ��ģ������������ 
	 0,//F137  ���� 
	-1,//F138 ֱ��ģ������������ 
	 0,0,0,0,0,0,//F139��F144  ���� 
	//��19  �ĸ������޹��ܵ���ʾֵ���� 
	-1,//F145  һ�����޹��ܵ���ʾֵ���� 
	-1,//F146  �������޹��ܵ���ʾֵ���� 
	-1,//F147  �������޹��ܵ���ʾֵ���� 
	-1,//F148  �������޹��ܵ���ʾֵ���� 
	 0,0,0,0,//F149��F152  ���� 
	//��20  �������ʾֵ 
	-1,//F153  �ն�����������й�����ʾֵ 
	-1,//F154  �ն�����������޹�����ʾֵ 
	-1,//F155  �ն�����෴���й�����ʾֵ 
	-1,//F156  �ն�����෴���޹�����ʾֵ 
	-1,//F157  �¶�����������й�����ʾֵ 
	-1,//F158  �¶�����������޹�����ʾֵ 
	-1,//F159  �¶�����෴���й�����ʾֵ 
	-1,//F160  �¶�����෴���޹�����ʾֵ 
	//��21   
	(5+1+5+5*GW_TARIFF_MAX),//F161  �����й�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F162 �����޹�������޹�1������ʾֵ���ܡ�����1��M�� 
	(5+1+5+5*GW_TARIFF_MAX),//F163  �����й�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F164 �����޹�������޹�1������ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F165  һ�����޹�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F166  �������޹�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F167  �������޹�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F168  �������޹�����ʾֵ���ܡ�����1��M�� 
	//��22   
	(5+1+5+5*GW_TARIFF_MAX),//F169  �����й�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F170 �����޹�������޹�1������ʾֵ���ܡ�����1��M�� 
	(5+1+5+5*GW_TARIFF_MAX),//F171  �����й�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F172 �����޹�������޹�1������ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F173  һ�����޹�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F174  �������޹�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F175  �������޹�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F176  �������޹�����ʾֵ���ܡ�����1��M�� 
	//��23   
	(5+1+5+5*GW_TARIFF_MAX),//F177  �����й�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F178  �����޹�������޹�1������ʾֵ���ܡ�����1��M��  
	(5+1+5+5*GW_TARIFF_MAX),//F179  �����й�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F180  �����޹�������޹�1������ʾֵ���ܡ�����1��M��  
	(5+1+4+4*GW_TARIFF_MAX),//F181  һ�����޹�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F182  �������޹�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F183  �������޹�����ʾֵ���ܡ�����1��M�� 
	(5+1+4+4*GW_TARIFF_MAX),//F184  �������޹�����ʾֵ���ܡ�����1��M�� 
	//��24   
	(5+1+7+7*GW_TARIFF_MAX),//F185  �����й��������������ʱ�䣨�ܡ�����1��M��  
	(5+1+7+7*GW_TARIFF_MAX),//F186  �����޹��������������ʱ�䣨�ܡ�����1��M��  
	(5+1+7+7*GW_TARIFF_MAX),//F187  �����й��������������ʱ�䣨�ܡ�����1��M��  
	(5+1+7+7*GW_TARIFF_MAX),//F188  �����޹��������������ʱ�䣨�ܡ�����1��M��  
	(5+1+7+7*GW_TARIFF_MAX),//F189  �����й��������������ʱ�䣨�ܡ�����1��M��  
	(5+1+7+7*GW_TARIFF_MAX),//F190  �����޹��������������ʱ�䣨�ܡ�����1��M��  
	(5+1+7+7*GW_TARIFF_MAX),//F191  �����й��������������ʱ�䣨�ܡ�����1��M��  
	(5+1+7+7*GW_TARIFF_MAX),//F192  �����޹��������������ʱ�䣨�ܡ�����1��M��  
	//��25   
	(5+1+7+7*GW_TARIFF_MAX),//F193  �����й��������������ʱ�䣨�ܡ�����1��M��  
	(5+1+7+7*GW_TARIFF_MAX),//F194  �����޹��������������ʱ�䣨�ܡ�����1��M��  
	(5+1+7+7*GW_TARIFF_MAX),//F195  �����й��������������ʱ�䣨�ܡ�����1��M��  
	(5+1+7+7*GW_TARIFF_MAX),//F196  �����޹��������������ʱ�䣨�ܡ�����1��M��  
	 0,0,0,0,//F197��F200  ���� 
	//��26  1��8ʱ�������й�����ʾֵ 
	(5+1+5+5*GW_TARIFF_MAX),//F201 ��һʱ�����������й�����ʾֵ���ܡ�����1��M�� 
	(5+1+5+5*GW_TARIFF_MAX),//F202 �ڶ�ʱ�����������й�����ʾֵ���ܡ�����1��M�� 
	(5+1+5+5*GW_TARIFF_MAX),//F203 ����ʱ�����������й�����ʾֵ���ܡ�����1��M�� 
	(5+1+5+5*GW_TARIFF_MAX),//F204 ����ʱ�����������й�����ʾֵ���ܡ�����1��M�� 
	(5+1+5+5*GW_TARIFF_MAX),//F205 ����ʱ�����������й�����ʾֵ���ܡ�����1��M�� 
	(5+1+5+5*GW_TARIFF_MAX),//F206 ����ʱ�����������й�����ʾֵ���ܡ�����1��M�� 
	(5+1+5+5*GW_TARIFF_MAX),//F207 ����ʱ�����������й�����ʾֵ���ܡ�����1��M�� 
	(5+1+5+5*GW_TARIFF_MAX),//F208 �ڰ�ʱ�����������й�����ʾֵ���ܡ�����1��M�� 
	//��27   
	-1,//F209  ���ܱ�Զ�̿���ͨ�ϵ�״̬����¼ 
	 0,0,0,//F210��F212  ���� 
	-1,//F213  ���ܱ��ز���������ʱ�� 
	-1,//F214  ���ܱ�����޸Ĵ�����ʱ�� 
	-1,//F215  ���ܱ����õ���Ϣ 
	(5+1+5+5*GW_TARIFF_MAX+5+5*GW_TARIFF_MAX),//F216  ���ܱ������Ϣ 
	//��28   
	-1,//F217  ̨�����г����ز����ڵ���������� 
	-1,//F218  ̨�����г����ز����ڵ�ɫ�������� 
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0//F219��F248  ���� 
};


/** @brief �ӻ����л�ȡ�ļ�ͷ����ֵ
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

/** @brief ������ṹ���ֵ���ļ�ͷ�ṹ��ʽ��������
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


/** @brief ����check sum
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

/** @brief �ӻ����л�ȡ���� */
bool S_EAreaInfo::GetFromBuffer(const char *pbuf)
{
	memcpy(Sign, pbuf+DF_EAREA_SIGNATURE_OFF, sizeof(Sign));
	memcpy(MAddr, pbuf+DF_EAREA_MADDR_OFF, sizeof(MAddr));
	memcpy(&QTail, pbuf+DF_EAREA_QTAIL_OFF, sizeof(QTail));
	memcpy(&QHead, pbuf+DF_EAREA_QHEAD_OFF, sizeof(QHead));
	return true;
}

/** @brief ������ݵ����� */
bool S_EAreaInfo::OutputBuffer(char *pbuf)
{
	memcpy(pbuf+DF_EAREA_SIGNATURE_OFF, Sign, sizeof(Sign));
	memcpy(pbuf+DF_EAREA_MADDR_OFF, MAddr, sizeof(MAddr));
	memcpy(pbuf+DF_EAREA_QTAIL_OFF, &QTail, sizeof(QTail));
	memcpy(pbuf+DF_EAREA_QHEAD_OFF, &QHead, sizeof(QHead));
	return true;
}


//
//�����ļ�IO
//
/** @brief ��ȡFNҪ��ļ�¼����
 *  @param[in] FN ����Ҫ��FN
 *  @return ��¼����
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

/** @brief ��ʼ����������������
 *  @param[in] pbuff ��������������
 *  @param[in] FN ����Ҫ��FN
 *  @return 0 �ɹ� -1 ʧ��
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

/** @brief ��ȡ�ļ���Ϣ */
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

/** @brief reset �����ļ�
 *  @param[in] fp �ļ�ָ��
 *  @return true �ɹ�;false ʧ��
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

/** @brief ��ȡ������������
 *  @param ��
 *  @return ������������������λ��byte��
*/
int C_DataFileRW::GetEnergyAreaSize(void)
{
	return (DF_EAREA_INFO_SIZE //Energy area info header
		+(DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+GetTimeTagLength()+m_Header.m_RecordLen) * m_Header.m_PiecePerMeter);
}

/** @brief ���ҷ���ʱ��ļ�¼
 *  @param[in] EAI ������������Ϣ
 *  @param[in] pRecord ��¼����ʼָ��
 *  @param[in] TimeTag ʱ�꣬��ʽ20090808(4ByteBCD)
 *  @return -1 δ���֣������ɹ�
 *  @note �¶��᲻���ã��գ��ص㻧�����������ݿ���
 */
int C_DataFileRW::FindRecord(const S_EAreaInfo &EAI, const char *pRecord, const char *TimeTag)
{
	int head = (int)EAI.QHead, tail = (int)EAI.QTail;

	if (head == tail) //empty queue
		return -1;

	while (head != tail)
	{
		if (head == 0)//��ͷ�Ͽ�ʼ��Ч�ʸ�
			head = m_Header.m_PiecePerMeter - 1;
		else
			head -= 1;
		//��λ��¼λ��
		const char *ptm = pRecord + head*(DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+GetTimeTagLength()+m_Header.m_RecordLen);
		ptm += DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE; //ƫ�Ƶ�ʱ��λ�ô�
		if (memcmp(ptm, TimeTag, GetTimeTagLength()) == 0)//compare successful
			return head;
	}
	return -1;
}

/** @brief ����ʱ��
 *  @param[out] TimeTag ʱ�����������4�ֽ�[20090808]BCD
 *  @param[in] time ʱ��
 *  @return ָ��TimeTag��ָ��
 *  @note �¶��᲻���ã��գ��ص㻧�����������ݿ���
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


/** @brief �����Ӵ��ļ�
 *  @param[in] fp �ļ�ָ��
 *  @param[in] MaxMP ���������
 *  @return true �ɹ�;false ʧ��
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

/** @brief д��һ�����ݼ�¼
 *  @param[in] fp �ļ�ָ��
 *  @param[in] EnergyData �������ݣ��ṹ�μ�S_EnergyData
 *  @param[in] DataTime ʱ��
 *  @return 0 �ɹ�����0 ʧ��
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

/** @brief ��ȡһ�����ݼ�¼ */
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

/** @brief �ϲ����ݵ���¼
 *  @param[inout] pEnergyArea ��ʷ���ݼ�¼����
 *  @param[in] EnergyData ��ǰд�ŵ�����
 *  @param[in] DataTime ʱ��
 *  @return 0 �ɹ�; ��0 ʧ��
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

/** @brief ����������ȡ��¼ 
 *  @param[in] pEnergyArea ��������ʼָ��
 *  @param[out] EnergyData �����������
 *  @param[in] DataTime ʱ��
 */
int C_DataFileRW::ExtractRecord(char *pEnergyArea, S_EnergyData &EnergyData, time_t DataTime)
{
	int pos;
	char TimeTag[DF_DAY_TIME_SIZE];
	S_EAreaInfo EAI;

	EAI.GetFromBuffer(pEnergyArea);
	MakeTimeTag(TimeTag, DataTime);

	EnergyData.m_DataFlag = 0xFF;
	if ((pos = FindRecord(EAI, pEnergyArea+DF_EAREA_INFO_SIZE, TimeTag)) != -1)//�ҵ����ݼ�¼
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
//�����ݶ�д��
//
/** @brief �����ļ���ʼ��
 *  @param[in] fp �ļ�ָ��
 *  @param[in] FN ����Ҫ��FN
 *  @param[in] MaxMP ���������
 *  @return true �ɹ���false ʧ��
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

/** @brief ��ʱ�䰴�������ͽ���
 *  @param[in] tm ʱ��
 *  @return ������ʱ��
 */
time_t C_DataFileRW_D::DataTimeTrim(time_t tm)
{
	return C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, tm);
}

//
//�����ݶ�д��
//
/** @brief �����ļ���ʼ��
 *  @param[in] fp �ļ�ָ��
 *  @param[in] FN ����Ҫ��FN
 *  @param[in] MaxMP ���������
 *  @return true �ɹ���false ʧ��
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

/** @brief ��ʱ�䰴�������ͽ���
 *  @param[in] tm ʱ��
 *  @return ������ʱ��
 */
time_t C_DataFileRW_M::DataTimeTrim(time_t tm)
{
	return C_TIME::DateTrim(C_TIME::TIME_UNIT_MONTH, tm);
}

/** @brief ����ʱ��
 *  @param[out] TimeTag ʱ�����������4�ֽ�[20090808]BCD
 *  @param[in] time ʱ��
 *  @return ָ��TimeTag��ָ��
 */
char *C_DataFileRW_M::MakeTimeTag(char *TimeTag, time_t time)
{
	struct tm *curtm = localtime(&time);

	TimeTag[0] = cbcd((curtm->tm_year+1900)/100);
	TimeTag[1] = cbcd((curtm->tm_year+1900)%100);
	TimeTag[2] = cbcd(curtm->tm_mon+1);;

	return TimeTag;
}

/** @brief ��ȡ��¼ʱ�곤�� */
int C_DataFileRW_M::GetTimeTagLength(void)
{
	return DF_MON_TIME_SIZE;
}

//
//�ص㻧���ݶ�д��
//
/** @brief �����ļ���ʼ��
 *  @param[in] fp �ļ�ָ��
 *  @param[in] FN ����Ҫ��FN
 *  @param[in] MaxMP ���������
 *  @return true �ɹ���false ʧ��
 */
bool C_DataFileRW_V::InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP)
{
	assert(m_VipPointPerDay != 0);
	memset(&m_Header, 0x00, sizeof(m_Header));
	InitDataFileSignature(m_Header.m_Signature, FN, "vip");
	m_Header.m_MaxMP = MaxMP;
	m_Header.m_PiecePerMeter = RECORD_STORE_VIP_MAX+1;
	m_Header.m_RecordLen = GetRecordLen(FN);
	m_Header.m_RecordInterVal = (24*60)/m_VipPointPerDay;//VIPDATA_POINT_PERDAY;//һ����ӳ���һ�����
	m_Header.m_RecordInterUnt = RECORD_INTER_UNIT_MIN;
	m_Header.m_RecordStdT = C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, time(NULL));
//	m_Header.m_CheckSum = 0;

	return ResetDataFile(fp);
}

/** @brief ��ʱ�䰴�������ͽ���
 *  @param[in] tm ʱ��
 *  @return ������ʱ��
 */
time_t C_DataFileRW_V::DataTimeTrim(time_t tm)
{
	return tm;
}

/** @brief �ϲ����ݵ���¼
 *  @param[inout] pEnergyArea ��ʷ���ݼ�¼����
 *  @param[in] EnergyData ��ǰд�ŵ�����
 *  @param[in] DataTime ʱ��
 *  @return 0 �ɹ�; ��0 ʧ��
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
		memset(pRecordBeg, 0xEE, DF_RECORD_FLAG_SIZE+DF_DATA_FLAG_SIZE+m_Header.m_RecordLen);//�����ʷ��¼
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
//���������ݶ�д��
//
/** @brief �����ļ���ʼ��
 *  @param[in] fp �ļ�ָ��
 *  @param[in] FN ����Ҫ��FN
 *  @param[in] MaxMP ���������
 *  @return true �ɹ���false ʧ��
 */
bool C_DataFileRW_G::InitDataFile(FILE *fp, INT16U FN, INT16U MaxMP)
{
	memset(&m_Header, 0x00, sizeof(m_Header));
	InitDataFileSignature(m_Header.m_Signature, FN, "gth");
	m_Header.m_MaxMP = MaxMP;
	m_Header.m_PiecePerMeter = RECORD_STORE_CBR_MAX+1;//�ö���ʵ����һ������
	m_Header.m_RecordLen = GetRecordLen(FN);
	m_Header.m_RecordInterVal = 0xFF;
	m_Header.m_RecordInterUnt = RECORD_INTER_UNIT_DAY;
	m_Header.m_RecordStdT = C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, time(NULL));
//	m_Header.m_CheckSum = 0;

	return ResetDataFile(fp);
}

/** @brief ��ʱ�䰴�������ͽ���
 *  @param[in] tm ʱ��
 *  @return ������ʱ��
 */
time_t C_DataFileRW_G::DataTimeTrim(time_t tm)
{
	return C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, tm);
}

//
//�������ļ���дʵ������
//
/** @brief ��ȡһ�����ݶ�дʵ��
 *  @param[in] FN ����Ҫ���������FN
 *  @return NULL ʧ��; ��NULL �ɹ�
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

/** @brief �ͷ�һ�����ݶ�дʵ��
 *  @param[in] pDFRW ���ݶ�дʵ��ָ��
 *  @return ��
 */
void C_DFRWFACT::FreeDataFileRW(C_DataFileRW *pDFRW)
{
	delete pDFRW;
}

/** @brief ��ȡ��������FN���� 
 *  @param[in] FN ����Ҫ���������FN
 *  @return CLASS2_UNKNOW, CLASS2_VIP, CLASS2_DAY, CLASS2_MON, CLASS2_GTH
 */
int  C_DFRWFACT::GetType4Class2FN(INT16U FN)
{
	if ((FN>=73&&FN<=110) || (FN>=138&&FN<=148) || (FN>=153&&FN<=156) || (FN==217) || (FN==218))//�ص㻧
		return CLASS2_VIP;
	else if ((FN>=1&&FN<=8) || (FN>=25&&FN<=32) || (FN>=41&&FN<=43) || (FN==45) || (FN==49) || (FN==50)//��
		|| (FN==53) || (FN>=57&&FN<=59) || (FN>=113&&FN<=129) || (FN>=161&&FN<=168) || (FN>=185&&FN<=188)
		|| (FN==209))//��
		return CLASS2_DAY;
	else if ((FN>=9&&FN<=12) || (FN>=169&&FN<=176) || (FN>=189&&FN<=192))//������
		return CLASS2_GTH;
	else if ((FN>=17&&FN<=24) || (FN>=33&&FN<=39) || (FN==44) || (FN==46) || (FN==51) || (FN==52) || (FN==54)//��
		|| (FN>=60&&FN<=66) || (FN==130) || (FN>=157&&FN<=160) || (FN>=177&&FN<=184) || (FN>=193&&FN<=208)
		|| (FN>=213&&FN<=216))//��
		return CLASS2_MON;

	return CLASS2_UNKNOW;
}

/** @brief  ��ȡFN��Ӧ��PN����
 *  @param[in] FN ����Ҫ���������FN
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
//�����ļ������������	
//
/** @brief C_DATAFILE���캯��
 *  @param[in] fPath �ļ�����·��
 *  @param[in] io OPEN_READ or OPEN_WRITE
 *  @param[in] FN ����Ҫ��FN
 */
C_DATAFILE::C_DATAFILE(const char *fPath, INT8U io, INT16U FN)
{
	m_FN = FN;
	m_fp = NULL;
	m_pdfrw = C_DFRWFACT::GetDataFileRW(FN);
	if (m_pdfrw)
        Open(fPath, io);
}

/** @brief C_DATAFILE��������
 */
C_DATAFILE::~C_DATAFILE()
{
	Close();
	if (m_pdfrw != NULL)
		C_DFRWFACT::FreeDataFileRW(m_pdfrw);
	m_pdfrw = NULL;
}

/** @brief �������ļ�
 *  @param[in] fPath �ļ�����·��
 *  @param[in] io OPEN_READ or OPEN_WRITE
 *  @return true if the file is successfully opened
 *  @return false if failed
 */
bool C_DATAFILE::Open(const char *pfile, INT8U io)
{
	if (io == OPEN_READ)//������
	{
		m_fp = C_FILE::OpenFile(pfile,"rb", false);
	}
	else if (io == OPEN_WRITE)//д����
	{
		m_fp = C_FILE::OpenFile(pfile,"r+b", true);
		if (m_fp == NULL)
			m_fp = C_FILE::OpenFile(pfile,"w+b", true);
	}
	else//����
	{
		m_fp = NULL;
	}

	return (m_fp != NULL);
}

/** @brief �ر������ļ�
 *  @param[in] fPath �ļ�����·��
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

/** @brief ��ȡһ����¼
 *  @param[inout] EnergyData �����������
 *  @param[in] DataTime ʱ��
 *  @return 0 �ɹ�����0 ʧ��
 */
int  C_DATAFILE::Read(S_EnergyData &EnergyData, time_t DataTime)
{
	if (IsInvalidMP(EnergyData.m_MeterMP))//invalid MP
	{
		loget<<EnergyData.m_MeterMP<<" is invalid!!!"<<endl;
		return -1;
	}
	if (EnergyData.m_MeterMP > m_pdfrw->GetMaxMP())//�������ڵ�ǰ�ļ�����
	{
		loget<<"DataFile MaxMP = "<<m_pdfrw->GetMaxMP()<<", Current MP = "<<EnergyData.m_MeterMP<<endl;
		return -1;
	}

	//��ȡ
	return m_pdfrw->Read(m_fp, EnergyData, DataTime);
}

/** @brief ��ȡ�ص㻧һ����¼
 *  @param[inout] EnergyData �����������
 *  @param[in] VipTime �ص㻧ʱ��
 *  @return 0 �ɹ�����0 ʧ��
 */
int  C_DATAFILE::ReadVip(S_EnergyData &EnergyData, const S_VIPT &VipTime)
{
	if (IsInvalidMP(EnergyData.m_MeterMP))//invalid MP
	{
		loget<<EnergyData.m_MeterMP<<" is invalid!!!"<<endl;
		return -1;
	}
	if (EnergyData.m_MeterMP > m_pdfrw->GetMaxMP())//�������ڵ�ǰ�ļ�����
	{
		loget<<"DataFile MaxMP = "<<m_pdfrw->GetMaxMP()<<", Current MP = "<<EnergyData.m_MeterMP<<endl;
		return -1;
	}
	if (VipTime.m_DataDensity == 0 || VipTime.m_DataDots == 0 || GetVipFreezeIntervalM(VipTime.m_DataDensity) == 0)
	{
		return -1;
	}

	//��ȡ
	int days = 0, size = 0;
	S_EnergyData Record;
	Record.m_MeterMP = EnergyData.m_MeterMP;
	time_t MaxTime = VipTime.m_time + (GetVipFreezeIntervalM(VipTime.m_DataDensity) * 60) * VipTime.m_DataDots;
	for (time_t MinTime = C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, VipTime.m_time); MinTime < MaxTime; MinTime += 86400)
	{
		days += 1;
		size  = (int)Record.m_Data.size();
		if (m_pdfrw->Read(m_fp, Record, MinTime) != 0)
			return -1;//��ȡʧ��
		if (size == (int)Record.m_Data.size())
			return -1;//û�ж�������
	}

	EnergyData.m_DataFlag = Record.m_DataFlag;
	//��ʱ����ȡ����
	INT16U DotLen = m_pdfrw->GetOnceWriteDataSize();
	int TotalActualDots = (int)Record.m_Data.size()/DotLen;//������ʵ�ʴ��ڵĵ���,������
	int TotalExpectedDots = days * ((24*60)/GetVipFreezeIntervalM(VipTime.m_DataDensity));//�����������ĵ�����������
	int BegDot = (int)((VipTime.m_time - C_TIME::DateTrim(C_TIME::TIME_UNIT_DAY, VipTime.m_time)) / (GetVipFreezeIntervalM(VipTime.m_DataDensity) * 60));
	for (int i = 0; i < VipTime.m_DataDots; i++)
	{
		int CurDot = (BegDot+i) * TotalActualDots / TotalExpectedDots;//���㵱ǰ��i���λ��
		EnergyData.m_Data.append(&Record.m_Data[CurDot*DotLen], DotLen);
	}

	return 0;
}

/** @brief д��һ����¼
 *  @param[in] EnergyData ��д������
 *  @param[in] DataTime ʱ��
 *  @return 0 �ɹ�д�룻��0 ʧ��
 */
int  C_DATAFILE::Write(const S_EnergyData &EnergyData, time_t DataTime)
{
	if (IsInvalidMP(EnergyData.m_MeterMP))//invalid MP
	{
		loget<<EnergyData.m_MeterMP<<" is invalid!!!"<<endl;
		return -1;
	}
	if (EnergyData.m_MeterMP > m_pdfrw->GetMaxMP())//�������ڵ�ǰ�ļ�����
	{//�����ļ�
		if (m_pdfrw->AppendDataFile(m_fp, EnergyData.m_MeterMP) == false)
		{
			loget<<"AppendDataFile fail, Current MP = "<<EnergyData.m_MeterMP<<endl;
			return -1;
		}
	}

	if (EnergyData.m_Data.size() != m_pdfrw->GetOnceWriteDataSize())	//��¼��С�����
	{
//		assert(0);
		loget<<"Ԥ�ڵ����ݴ�С��ʵ�ʵĲ���:"<<(int)EnergyData.m_Data.size()<<" != "<<m_pdfrw->GetOnceWriteDataSize()<<endl;
		return -1;//���ȷ���
	}
	
	//д��
	return m_pdfrw->Write(m_fp, EnergyData, DataTime);
}

/** @brief �ж��Ƿ��½�
 *  @param ��
 *  @return true �½��ļ�
 *  @return false ���������ļ�
 */
bool C_DATAFILE::IsNew(void)
{
	fseek(m_fp, 0x00, SEEK_END);
	return ((ftell(m_fp)<=DF_HEADER_SIZE)? true:false);
}

/** @brief ��ʼ�������ļ�
 *  @param[in] MaxMP ���������
 *  @return true �ɹ�
 *  @return false ʧ��
 */
bool C_DATAFILE::InitDataFile(INT16U MaxMP)
{
	if (MaxMP == 0)
		return false;
	return m_pdfrw->InitDataFile(m_fp, m_FN, MaxMP);
}

/** @brief ��ȡ�ļ���Ϣͷ
 *  @param ��
 *  @return 0 �ɹ�
 *  @return ��0 ʧ��
 */
int  C_DATAFILE::GetFileHeader(void)
{
	return m_pdfrw->GetFileInfoHeader(m_fp);
}

/** @brief ��ʱ�䰴�������ͽ��� */
time_t C_DATAFILE::DataTimeTrim(time_t tm)
{
	return m_pdfrw->DataTimeTrim(tm);
}
/** @brief ��ȡ�ص㻧����ʱ����
 *  @param[in] FreezeCode ���й�ԼҪ�󶳽��ܶ�
 *  @return ����ʱ�䣬�Է���Ϊ��λ
 *  @see �����й�Լ����¼C���淶�Ը�¼�����ݶ����ܶ�
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
// �û��ӿڶ���
//
/** @brief ��������д�뺯��
 *  @param[in] FN ����Ҫ���FN
 *  @param[in] MeterDatas ���������飬�μ�S_EnergyDatas����
 *  @param[in] DataTime ʱ��
 *  @param[in] VipPointPerDay �ص㻧FNÿ����Ҫ�����ݵ����ݣ������ص㻧FN��Ч
 *  @return д��ļ�¼����
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
			if (tMeterDatas[i].m_Data.empty())//������յ����
				;
			else if (tMeterDatas[i].m_Data.size() < (unsigned)FnLen)
				tMeterDatas[i].m_Data.append((size_t)(FnLen-tMeterDatas[i].m_Data.size()), (char)m_FillChar);
		}
	}

	return _WriteClass2Data(FN, tMeterDatas, DataTime, VipPointPerDay);
}


/** @brief �������ݶ�ȡ����
 *  @param[in] FN ����Ҫ���FN
 *  @param[in] MeterDatas ���������飬�μ�S_EnergyDatas����
 *  @param[in] DataTime ʱ��
 *  @return ��ȡ�ļ�¼�������ص㻧һ����¼��ָһ������ݣ�
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

	/** ȥ�����ӵ���������0xFF
	 */
	for (INT16U i = 0; i < MeterDatas.size(); ++i)
	{
		string::iterator iter = find(MeterDatas[i].m_Data.begin(), MeterDatas[i].m_Data.end(), m_FillChar);
		if (iter != MeterDatas[i].m_Data.end())
			MeterDatas[i].m_Data.erase(iter, MeterDatas[i].m_Data.end());
	}
	
	return cnt;
}

/** @brief �����ص㻧���ݶ�ȡ����
 *  @param[in] FN ����Ҫ��Ķ�������FN�������߱��뱣֤Ϊ�ص㻧FN
 *  @param[inout] MeterDatas ���������飬�μ�S_EnergyDatas����
 *  @param[in] VipTime �ص㻧ʱ��
 *  @return ��ȡ�ļ�¼����
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
		//�ļ�����ʧ�ܻ�У��ʧ��???
		loget<<fPath<<" FileHeader fail."<<endl;
		return 0;
	}

	size_t i = 0;
	for (; i < MeterDatas.size(); ++i)//������ȡ����
	{
		if (DataFile.ReadVip(MeterDatas[i], VipTime) != 0)
			break;//read failed
	}

	return (int)i;
}

/** @brief ��������д�뺯��
 *  @param[in] FN ����Ҫ���FN
 *  @param[in] MeterDatas ���������飬�μ�S_EnergyDatas����
 *  @param[in] DataTime ʱ��
 *  @param[in] VipPointPerDay �ص㻧FNÿ����Ҫ�����ݵ����ݣ������ص㻧FN��Ч
 *  @return д��ļ�¼����
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

	if (DataFile.IsNew() == true)//�Ƿ�Ϊ�½��ļ�
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
		//�ļ�����ʧ�ܻ�У��ʧ��???
		loget<<fPath<<" FileHeader fail."<<endl;
		return 0;
	}

	if (DataTime == 0)
		DataTime = time(NULL);
	DataTime = DataFile.DataTimeTrim(DataTime);
	
	size_t i = 0;
	for (; i < MeterDatas.size(); ++i)//����д������
	{
		if (DataFile.Write(MeterDatas[i], DataTime) != 0)
			break;//write failed
	}

	return (int)i;
}


/** @brief �������ݶ�ȡ����
 *  @param[in] FN ����Ҫ���FN
 *  @param[in] MeterDatas ���������飬�μ�S_EnergyDatas����
 *  @param[in] DataTime ʱ��
 *  @return ��ȡ�ļ�¼�������ص㻧һ����¼��ָһ������ݣ�
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
		//�ļ�����ʧ�ܻ�У��ʧ��???
		loget<<fPath<<" FileHeader fail."<<endl;
		return 0;
	}

	if (DataTime == 0)
		DataTime = time(NULL);
	DataTime = DataFile.DataTimeTrim(DataTime);
	
	size_t i = 0;
	for (; i < MeterDatas.size(); ++i)//������ȡ����
	{
		if (DataFile.Read(MeterDatas[i], DataTime) != 0)
			break;//read failed
	}

	return (int)i;
}

/** @brief ���������ļ�·��
 *  @param[out] fpath �ļ�·��������壬��С260�ֽ�
 *  @param[in] FN ����Ҫ���������FN
 *  @return fpath��ָ��
 *  @note user can't call.
 */
char *C_DataFileOP::MakeDataFilePath(char *fPath, INT16U FN)
{
	sprintf(fPath, "%s/c2f%d.dat", CFG_FILE_EDATA_PATH, FN);

	return fPath;
}

