/** @file
 *  @brief 时间操作集
 *  @author jinhailiao
 *  @date 2011/11/07
 *  @version 0.1
 *  1、时间操作类-符合国网规约格式ssmmhhDDMMYYYY
 *  2、时间操作类-符合南网规约格式YYYYMMDDhhmmss
 */
#ifndef __FND_TIMEOP_H__
#define __FND_TIMEOP_H__
#include "basedef.h"

struct S_DATE
{
	int m_Year; ///< 年份，取值区间为[0,9999]
	int m_Mon; ///< 月份（从一月开始，1代表一月） - 取值区间为[1,12] 
	int m_Day; ///< 一个月中的日期 - 取值区间为[1,31] 
	int m_Hour; ///< 时 - 取值区间为[0,23] 
	int m_Min; ///< 分 - 取值区间为[0,59] 
	int m_Sec; ///< 秒–取值区间为[0,59] 
	int m_WeekDay; ///< 星期–取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推
	int m_YearDay; ///< 从每年的1月1日开始的天数–取值区间为[1,366]，其中1代表1月1日，2代表1月2日，以此类推

	S_DATE(void){memset(this, 0x00, sizeof(*this));}
};

/** 时间单位 */
enum S_TIME_UNIT{
	TIME_UNIT_UNKNOW,
	TIME_UNIT_SECOND,
	TIME_UNIT_MINUTE,
	TIME_UNIT_HOUR,
	TIME_UNIT_DAY,
	TIME_UNIT_MONTH,
	TIME_UNIT_YEAR
};

/** @class
 *  @brief 时间操作集合
 *  @remarks 具有国网版和南网版
 */
class C_TIME
{
public:
	/** 取当前时钟(封装time(NULL)) */
	static S_TIME TimeNow(void);
	static S_TIME MakeTime(const S_DATE &date);
	static S_DATE *LocalTime(S_TIME mytime, S_DATE &date);

	/** 以下BCD串的格式为:ssmmhhDDMMYYYY 符合国网规约*/
	static char *CurTime2BCDStr(char *bcdStr7B){return Time2BCDStr(TimeNow(), bcdStr7B);}
	static char *Time2BCDStr(S_TIME time, char *bcdStr7B);
	static S_TIME BcdStr2Time(const char *bcdStr7B);
	static S_TIME BcddateAdd(S_TIME_UNIT interUnit, long offset, const char *srcBcd7B, char *dstBcd7B);
	static int SetSysHWClock(const char *bcdStr7B);

	/** 以下BCD串的格式为:YYYYMMDDhhmmss 符合南网规约*/
	static char *CurTime2BCDStr_Big(char *bcdStr7B){return Time2BCDStr_Big(TimeNow(), bcdStr7B);}
	static char *Time2BCDStr_Big(S_TIME time, char *bcdStr7B);
	static S_TIME BcdStr2Time_Big(const char *bcdStr7B);
	static int SetSysHWClock_Big(const char *bcdStr7B);
	static S_TIME BcdStr2Time5B_Big(const char *bcdStr5B);
	static char* Time2BcdStr5B_Big(S_TIME time, char *bcdStr5B);
	static S_TIME BcddateAdd_Big(S_TIME_UNIT interUnit, long offset, const char *srcBcd7B, char *dstBcd7B);


	/** @brief 日期对齐
	 *  @param[in] interUnit 时间单位
	 *  @param[in] date 日期
	 *  @return 规整后的日期
	 */
	static S_TIME DateTrim(S_TIME_UNIT interUnit, S_TIME date);
	/** @breif 日期单位时间增减
	 *  @param[in] interUnit 日期单位
	 *  @param[in] offset 时间偏移
	 *  @param[in] date 日期
	 *  @return 日期
	 */
	static S_TIME DateAdd(S_TIME_UNIT interUnit, long offset, S_TIME date);
	/** @brief 计算日期间的单位差
	 *  @param[in] interUnit 时间单位
	 *  @param[in] begin 开始时间
	 *  @param[in] end 结束时间
	 *  @return >= 0 (end>=begin)
	 *  @return <0 (end<begin)
	 */
	static long DateDiff(S_TIME_UNIT interUnit, S_TIME begin, S_TIME end);

	/** @brief 取时间串，格式:MM-DD hh:mm:ss
	 *  @param[out] ptimeBuf 格式串输出缓冲
	 *  @return ptimeBuf
	 */
	static char *Time2Stamp(char *strAscii14B);
	/** @brief 取时间串，格式:YYYYMMDDhhmm
	 *  @param[out] ptimeBuf 格式串输出缓冲
	 *  @param[in] size 缓冲大小
	 *  @return NULL size太小
	 *  @return ptimeBuf
	 */
	static char *Now2Ascii12(char *strAscii12B);

	/** @brief 取时间串，格式:YYYYMMDDhhmmss
	 *  @param[out] strAscii16B 格式串输出缓冲
	 *  @param[in] time 时针
	 *  @return strAscii16B
	 */
	static char* Time2Ascii14(S_TIME time, char *strAscii16B);
};

/** @class
 *  @brief 超时对象定义
 */
class C_MSTIMOUT
{
public:
	C_MSTIMOUT(S_DWORD ms);
	~C_MSTIMOUT();

	/** @brief 超时判断
	 *  @return true 超时；false 未超时
	 */
	S_BOOL timeout(void);

private:
	S_TIME m_timeStart; ///< 开始时间
	S_DWORD m_timout; ///< 超时间隔（单位秒）
};

#endif //__FND_TIMEOP_H__


