/** @file
 *  @brief Lua配置参数程序
 *  @author jinhailiao
 *  @date 2017/03/06
 *  @version 0.1
 *  @see Lua相关文档
 */
#ifndef __HAI_LUA_PARAM__H__
#define __HAI_LUA_PARAM__H__
#include "basetype.h"
#include "lua.hpp"

class C_ConfigFile
{
public:
	static int LoadMeterConfig(lua_State *L, S_MeterInfos &MeterInfos);
	static int LoadHttxConfig(lua_State *L, S_HttxInfos &HttxInfos);

private:
	static int _LoadMeterConfig(lua_State *L, int idx, S_MeterInfo &MeterInfo);
	static int _LoadHttxConfig(lua_State *L, int idx, S_HttxInfo &HttxInfo);

	static bool GetBoolValue(lua_State *L, const string &strKey);
	static int GetNumberValue(lua_State *L, const string &strKey); 
	static string GetStringValue(lua_State *L, const string &strKey);

	static S_BYTE MeterRule2ID(const string &strRule);
};

#endif//__HAI_LUA_PARAM__H__

