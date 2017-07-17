/** @file
 *  @brief Lua配置参数程序
 *  @author jinhailiao
 *  @date 2017/03/06
 *  @version 0.1
 *  @see Lua相关文档
 */
#include "luaparam.h"

int C_ConfigFile::LoadMeterConfig(lua_State *L, S_MeterInfos &MeterInfos)
{
	// Load Meters
	lua_getglobal(L, "Meters");

	if (lua_istable(L, -1) == true)
	{
		for (int i = 0; i < METER_COUNT_MAX; i++)
		{
			S_MeterInfo MeterInfo;
			if (_LoadMeterConfig(L, i+1, MeterInfo) < 0)
				break;
			MeterInfos.push_back(MeterInfo);
		}
	}

	// Pop Meters
	lua_pop(L, 1);
	return 0;
}

int C_ConfigFile::LoadHttxConfig(lua_State *L, S_HttxInfos &HttxInfos)
{
	// Load Meters
	lua_getglobal(L, "Httxs");

	if (lua_istable(L, -1) == true)
	{
		for (int i = 0; i < 4; i++)
		{
			S_HttxInfo HttxInfo;
			if (_LoadHttxConfig(L, i+1, HttxInfo) < 0)
				break;
			HttxInfos.push_back(HttxInfo);
		}
	}

	// Pop Meters
	lua_pop(L, 1);
	return 0;
}

int C_ConfigFile::_LoadMeterConfig(lua_State *L, int idx, S_MeterInfo &MeterInfo)
{
	// 获取索引为idx的表计
	lua_pushnumber(L, idx);
	lua_gettable(L, -2);
	if (lua_istable(L, -1) == false)
	{
		lua_pop(L, 1);// 弹出索引为idx的表计
		return -1;
	}

	MeterInfo.m_Enable = GetBoolValue(L, "Enable");
	MeterInfo.m_MeasureNo = (S_WORD)GetNumberValue(L, "No"); 
	MeterInfo.m_RuleID = MeterRule2ID(GetStringValue(L, "MeterRule"));
	MeterInfo.m_VipFlag = GetBoolValue(L, "VipFlag"); 
	MeterInfo.m_PortSet.m_Baud = GetNumberValue(L, "PortSetBaud");
	MeterInfo.m_PortSet.m_DataBit = GetNumberValue(L, "PortSetDataBit");
	MeterInfo.m_PortSet.m_Parity = GetNumberValue(L, "PortSetParity");
	MeterInfo.m_PortSet.m_PortNO = GetNumberValue(L, "PortSetNo");
	MeterInfo.m_PortSet.m_StopBit = GetNumberValue(L, "PortSetStopBit");
	MeterInfo.m_PortSet.m_CDCheck = 0;

	MeterInfo.m_RatedVoltage = GetNumberValue(L, "RatedVoltage");
	MeterInfo.m_RatedCurrent = GetNumberValue(L, "RatedCurrent");

	MeterInfo.m_strAddress = GetStringValue(L, "MeterAddr");
	MeterInfo.m_strPassword = GetStringValue(L, "Password");
	MeterInfo.m_strUserName = GetStringValue(L, "UserName");

	lua_pop(L, 1);// 弹出索引为idx的表计
	return 0;
}

int C_ConfigFile::_LoadHttxConfig(lua_State *L, int idx, S_HttxInfo &HttxInfo)
{
	// 获取索引为idx的表计
	lua_pushnumber(L, idx);
	lua_gettable(L, -2);
	if (lua_istable(L, -1) == false)
	{
		lua_pop(L, 1);// 弹出索引为idx的表计
		return -1;
	}

	HttxInfo.m_ReportEnable = GetBoolValue(L, "ReportEnable");
	HttxInfo.m_MsgConfirmCode = (S_BYTE)GetNumberValue(L, "MsgConfirmCode");
	HttxInfo.m_HeartBeatPeriodM = (S_BYTE)GetNumberValue(L, "HeartBeatPeriodM");
	HttxInfo.m_ResendTimes = (S_BYTE)GetNumberValue(L, "ResendTimes");
	HttxInfo.m_WaitResponseTimeoutS = (S_WORD)GetNumberValue(L, "WaitResponseTimeoutS");
	HttxInfo.m_LoginPeriodS = (S_WORD)GetNumberValue(L, "LoginPeriodS");
	HttxInfo.m_MonTrafficLimit = (S_DWORD)GetNumberValue(L, "MonTrafficLimit");
	HttxInfo.m_strChannlName = GetStringValue(L, "Channel");

	lua_pop(L, 1);// 弹出索引为idx的表计
	return 0;
}

bool C_ConfigFile::GetBoolValue(lua_State *L, const string &strKey)
{
	bool v = false;
	lua_pushstring(L, strKey.c_str());
	lua_gettable(L, -2);
	if (lua_isboolean(L, -1))
	{
		if (lua_toboolean(L, -1) != 0)
			v = true;
	}
	lua_pop(L, 1);
	return v;
}

int C_ConfigFile::GetNumberValue(lua_State *L, const string &strKey)
{
	int v = 0;
	lua_pushstring(L, strKey.c_str());
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1))
		v = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	return v;
}

string C_ConfigFile::GetStringValue(lua_State *L, const string &strKey)
{
	string strVal;
	lua_pushstring(L, strKey.c_str());
	lua_gettable(L, -2);
	if (lua_isstring(L, -1))
		strVal = lua_tostring(L, -1);
	lua_pop(L, 1);
	return strVal;
}

S_BYTE C_ConfigFile::MeterRule2ID(const string &strRule)
{
	if (strRule == "GB645_1997")
		return 1;
	if (strRule == "GB645_2007")
		return 2;
	return 0;
}


